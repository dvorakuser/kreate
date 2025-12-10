#include "WallpaperAnalyzer.h"

#include <QtMath>
#include <QDebug>
#include <QVector>
#include <cmath>

// ----------------- helpers & types -----------------
namespace {

// sRGB (0–1) to linear
inline double srgbChannelToLinear(double c)
{
    if (c <= 0.04045)
        return c / 12.92;
    return std::pow((c + 0.055) / 1.055, 2.4);
}

// WCAG luminance, used for dark/light theme and text color
inline double luminance(const QColor &c)
{
    const double r = srgbChannelToLinear(c.redF());
    const double g = srgbChannelToLinear(c.greenF());
    const double b = srgbChannelToLinear(c.blueF());
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

// sRGB → OKLab
inline void rgbToOklab(const QColor &c, double &L, double &A, double &B)
{
    double r = srgbChannelToLinear(c.redF());
    double g = srgbChannelToLinear(c.greenF());
    double b = srgbChannelToLinear(c.blueF());

    double l = 0.4122214708 * r + 0.5363325363 * g + 0.0514459929 * b;
    double m = 0.2119034982 * r + 0.6806995451 * g + 0.1073969566 * b;
    double s = 0.0883024619 * r + 0.2817188376 * g + 0.6299787005 * b;

    double l_ = std::cbrt(l);
    double m_ = std::cbrt(m);
    double s_ = std::cbrt(s);

    L =  0.2104542553 * l_ + 0.7936177850 * m_ - 0.0040720468 * s_;
    A =  1.9779984951 * l_ - 2.4285922050 * m_ + 0.4505937099 * s_;
    B =  0.0259040371 * l_ + 0.7827717662 * m_ - 0.8086757660 * s_;
}

inline QColor autoTextForBackground(const QColor &bg)
{
    if (!bg.isValid())
        return QColor(Qt::black);
    double L = luminance(bg);
    return (L < 0.45) ? QColor(Qt::white) : QColor(Qt::black);
}

inline double clamp(double v, double lo, double hi)
{
    return (v < lo) ? lo : (v > hi ? hi : v);
}

// “Emotional” warmth heuristic in OKLab:
//   - standard warm: a > 0.015
//   - magenta/violet override: b is quite negative, a near zero → warm-ish sunset pink
inline bool emotionallyWarm(double a_ok, double b_ok)
{
    if (a_ok > 0.015)
        return true;
    if (b_ok < -0.05 && a_ok > -0.02)
        return true;
    return false;
}

struct PixelInfo {
    double L_ok;
    double a_ok;
    double b_ok;
    double C_ok;
    double L_lum;
    QColor c;
    int zone;   // 0 = top, 1 = mid, 2 = bottom
};

enum SceneType {
    Scene_Generic = 0,
    Scene_Nature = 1,
    Scene_Sunset = 2,
    Scene_Parchment = 3,
    Scene_DarkFantasy = 4
};

} // namespace

// ----------------- ctor -----------------

WallpaperAnalyzer::WallpaperAnalyzer(QObject *parent)
    : QObject(parent)
{
}

// ----------------- API -----------------

void WallpaperAnalyzer::setImage(const QImage &img)
{
    m_image = img;
    m_palette.clear();
    emit imageChanged();
}

void WallpaperAnalyzer::clear()
{
    m_image = QImage();
    m_palette.clear();
    emit imageChanged();
    emit analysisChanged();
}

void WallpaperAnalyzer::analyze()
{
    if (m_image.isNull()) {
        qWarning() << "WallpaperAnalyzer::analyze: no image set";
        return;
    }

    // Downsample for speed & stability
    QImage img = m_image.convertToFormat(QImage::Format_RGBA8888)
                          .scaled(96, 96, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    const int w = img.width();
    const int h = img.height();
    if (w <= 0 || h <= 0) {
        qWarning() << "WallpaperAnalyzer::analyze: invalid scaled image";
        return;
    }

    // ---------- Horizon detection (same as v7) ----------
    QVector<double> rowLumSum(h, 0.0), rowChrSum(h, 0.0);
    QVector<int>    rowCount(h, 0);

    for (int y = 0; y < h; ++y) {
        const QRgb *line = reinterpret_cast<const QRgb *>(img.constScanLine(y));
        for (int x = 0; x < w; ++x) {
            QColor c = QColor::fromRgba(line[x]);
            if (c.alpha() < 24)
                continue;
            double L_l = luminance(c);
            double L_ok, a_ok, b_ok;
            rgbToOklab(c, L_ok, a_ok, b_ok);
            double C_ok = std::sqrt(a_ok*a_ok + b_ok*b_ok);

            rowLumSum[y] += L_l;
            rowChrSum[y] += C_ok;
            rowCount[y]  += 1;
        }
    }

    QVector<double> rowLum(h, 0.0), rowChr(h, 0.0);
    for (int y = 0; y < h; ++y) {
        if (rowCount[y] > 0) {
            double inv = 1.0 / double(rowCount[y]);
            rowLum[y] = rowLumSum[y] * inv;
            rowChr[y] = rowChrSum[y] * inv;
        }
    }

    int horizonY = h / 2;
    double maxGrad = 0.0;
    for (int y = 0; y < h - 1; ++y) {
        double gL = std::abs(rowLum[y + 1] - rowLum[y]);
        double gC = std::abs(rowChr[y + 1] - rowChr[y]);
        double g  = gL * 0.7 + gC * 0.3;
        if (g > maxGrad) {
            maxGrad = g;
            horizonY = y;
        }
    }

    bool horizonValid = (maxGrad > 0.03 &&
                         horizonY > int(h * 0.15) &&
                         horizonY < int(h * 0.85));

    int topEnd = horizonValid ? horizonY        : int(h * 0.33);
    int bottomStart = horizonValid ? int(h*0.66): int(h * 0.66);
    if (horizonValid && bottomStart <= topEnd)
        bottomStart = qMin(h - 1, topEnd + (h - topEnd) / 2);

    qDebug() << "WallpaperAnalyzer v7 zoning: h=" << h
             << "horizonY=" << horizonY << "horizonValid=" << horizonValid
             << "maxGrad=" << maxGrad << "topEnd=" << topEnd
             << "bottomStart=" << bottomStart;

    // ---------- Classify pixels ----------
    QVector<PixelInfo> basePixels;
    QVector<PixelInfo> neutralPixels;
    QVector<PixelInfo> highlightPixels;

    basePixels.reserve(w*h);
    neutralPixels.reserve(w*h/4);
    highlightPixels.reserve(w*h/4);

    double sumLumAll = 0.0;
    int    countAll  = 0;

    // Zone-wise OKLab stats
    double sumL_zone[3] = {0.0, 0.0, 0.0};
    double suma_zone[3] = {0.0, 0.0, 0.0};
    double sumb_zone[3] = {0.0, 0.0, 0.0};
    int    count_zone[3] = {0, 0, 0};

    // Neutral stats
    double sumL_neutral = 0.0;
    double suma_neutral = 0.0;
    double sumb_neutral = 0.0;
    int    count_neutral = 0;

    // For dark fantasy: warm mid-zone shadows
    int darkWarmMid = 0;
    int darkMidTotal = 0;

    for (int y = 0; y < h; ++y) {
        int zone = 1;
        if (y <= topEnd) zone = 0;
        else if (y >= bottomStart) zone = 2;

        const QRgb *line = reinterpret_cast<const QRgb *>(img.constScanLine(y));
        for (int x = 0; x < w; ++x) {
            QColor c = QColor::fromRgba(line[x]);
            if (c.alpha() < 24)
                continue;

            double L_l = luminance(c);
            double L_ok, a_ok, b_ok;
            rgbToOklab(c, L_ok, a_ok, b_ok);
            double C_ok = std::sqrt(a_ok*a_ok + b_ok*b_ok);

            sumLumAll += L_l;
            ++countAll;

            sumL_zone[zone] += L_ok;
            suma_zone[zone] += a_ok;
            sumb_zone[zone] += b_ok;
            count_zone[zone]++;

            PixelInfo info{L_ok, a_ok, b_ok, C_ok, L_l, c, zone};

            bool isNeutral   = (C_ok < 0.03);
            bool isHighlight = (!isNeutral && L_ok > 0.75 && C_ok > 0.04);
            bool isBody      = (!isNeutral && !isHighlight &&
                                L_ok > 0.15 && L_ok < 0.90);

            if (isNeutral) {
                neutralPixels.push_back(info);
                sumL_neutral += L_ok;
                suma_neutral += a_ok;
                sumb_neutral += b_ok;
                ++count_neutral;
                continue;
            }

            if (isHighlight) {
                highlightPixels.push_back(info);
                continue;
            }

            if (isBody) {
                basePixels.push_back(info);

                if (zone == 1 && L_ok < 0.35) {
                    ++darkMidTotal;
                    if (emotionallyWarm(a_ok, b_ok))
                        ++darkWarmMid;
                }
            }
        }
    }

    if (countAll == 0) {
        qWarning() << "WallpaperAnalyzer::analyze: no visible pixels after classification";
        return;
    }

    double avgLum = sumLumAll / double(countAll);
    bool darkWallpaper = (avgLum < 0.30);

    qDebug() << "WallpaperAnalyzer v7 roles: basePixels=" << basePixels.size()
             << "neutralPixels=" << neutralPixels.size()
             << "highlightPixels=" << highlightPixels.size()
             << "avgLum=" << avgLum
             << "darkWallpaper=" << darkWallpaper;

    // ---------- Scene detection (OKLab + emotional warm) ----------

    auto safeAvg = [](double sum, int count) -> double {
        return (count > 0) ? (sum / double(count)) : 0.0;
    };

    double L_top = safeAvg(sumL_zone[0], count_zone[0]);
    double L_mid = safeAvg(sumL_zone[1], count_zone[1]);
    double L_bot = safeAvg(sumL_zone[2], count_zone[2]);

    double a_top = safeAvg(suma_zone[0], count_zone[0]);
    double a_mid = safeAvg(suma_zone[1], count_zone[1]);
    double a_bot = safeAvg(suma_zone[2], count_zone[2]);

    double b_top = safeAvg(sumb_zone[0], count_zone[0]);
    double b_mid = safeAvg(sumb_zone[1], count_zone[1]);
    double b_bot = safeAvg(sumb_zone[2], count_zone[2]);

    double neutralFrac = double(neutralPixels.size()) / double(countAll);
    double baseFrac    = double(basePixels.size())    / double(countAll);

    double L_neutral = safeAvg(sumL_neutral, count_neutral);
    double a_neutral = safeAvg(suma_neutral, count_neutral);
    double b_neutral = safeAvg(sumb_neutral, count_neutral);

    double darkWarmMidFrac = (darkMidTotal > 0)
        ? double(darkWarmMid) / double(darkMidTotal)
        : 0.0;

    SceneType scene = Scene_Generic;

    // Parchment: lots of neutral, mid-high L, slightly warm or near-neutral
    if (count_neutral > 0) {
        bool parchmentL = (L_neutral > 0.45 && L_neutral < 0.9);
        bool parchmentWarmish = (a_neutral > -0.01 && a_neutral < 0.10); // v7.1 expanded
        if (neutralFrac > 0.20 && parchmentL && parchmentWarmish) {
            scene = Scene_Parchment;
        }
    }

    // Dark fantasy: dark wallpaper, mid-zone dark, warm-ish mid zone
    if (scene == Scene_Generic &&
        darkWallpaper &&
        L_mid < 0.35 &&
        darkWarmMidFrac > 0.20 &&
        emotionallyWarm(a_mid, b_mid)) {
        scene = Scene_DarkFantasy;
    }

    // Nature: greens in mid/bottom → a negative
    if (scene == Scene_Generic &&
        a_mid < -0.02 && a_bot < -0.02 &&
        L_mid > 0.25 &&
        !darkWallpaper) {
        scene = Scene_Nature;
    }

    // Sunset: emotionally warm top, brighter than mid, not too neutral
    bool warmTop = emotionallyWarm(a_top, b_top);
    if (scene == Scene_Generic &&
        warmTop &&
        L_top > L_mid + 0.03 &&
        neutralFrac < 0.40) {
        scene = Scene_Sunset;
    }

    qDebug() << "WallpaperAnalyzer v7.1 scene detection:"
             << "scene=" << scene
             << "neutralFrac=" << neutralFrac
             << "baseFrac=" << baseFrac
             << "L_top/mid/bot=" << L_top << L_mid << L_bot
             << "a_top/mid/bot=" << a_top << a_mid << a_bot
             << "b_top/mid/bot=" << b_top << b_mid << b_bot
             << "L_neutral=" << L_neutral
             << "a_neutral=" << a_neutral
             << "b_neutral=" << b_neutral
             << "darkWarmMidFrac=" << darkWarmMidFrac;

    // ---------- Base color selection ----------

    QColor baseOut;

    if (scene == Scene_Parchment && count_neutral > 0) {
        // Strongly parchment: neutral dominant
        double sumR = 0, sumG = 0, sumB = 0;
        for (const auto &p : neutralPixels) {
            sumR += p.c.redF();
            sumG += p.c.greenF();
            sumB += p.c.blueF();
        }
        double inv = 1.0 / double(neutralPixels.size());
        baseOut.setRgbF(sumR * inv, sumG * inv, sumB * inv);
    } else if (!basePixels.isEmpty()) {
        // Weighted average of base pixels with zone and mid-lightness bias
        double sumR = 0, sumG = 0, sumB = 0;
        double sumW = 0;

        for (const auto &p : basePixels) {
            double zoneWeight[3] = {1.0, 1.0, 1.0};
            switch (scene) {
            case Scene_Nature:
                zoneWeight[0] = 0.7;
                zoneWeight[1] = 1.0;
                zoneWeight[2] = 1.4;
                break;
            case Scene_Sunset:
                zoneWeight[0] = 1.8;
                zoneWeight[1] = 1.0;
                zoneWeight[2] = 0.5;
                break;
            case Scene_DarkFantasy:
                zoneWeight[0] = 0.7;
                zoneWeight[1] = 1.5;
                zoneWeight[2] = 1.0;
                break;
            case Scene_Parchment:
                zoneWeight[0] = 1.2;
                zoneWeight[1] = 1.2;
                zoneWeight[2] = 1.0;
                break;
            case Scene_Generic:
            default:
                zoneWeight[0] = 0.9;
                zoneWeight[1] = 1.1;
                zoneWeight[2] = 1.0;
                break;
            }

            double w = zoneWeight[p.zone];

            // Prefer mid-lightness
            double midBias = 1.0 - std::fabs(p.L_ok - 0.5);
            w *= (0.4 + 0.6 * midBias);

            // Prefer moderate chroma, not grey and not neon
            double chrFactor = clamp(p.C_ok / 0.18, 0.2, 1.5);
            w *= chrFactor;

            sumR += p.c.redF()   * w;
            sumG += p.c.greenF() * w;
            sumB += p.c.blueF()  * w;
            sumW += w;
        }

        if (sumW > 0.0) {
            baseOut.setRgbF(sumR / sumW, sumG / sumW, sumB / sumW);
        } else {
            baseOut = QColor(Qt::gray);
        }
    } else if (!neutralPixels.isEmpty()) {
        // Fallback: neutral average
        double sumR = 0, sumG = 0, sumB = 0;
        for (const auto &p : neutralPixels) {
            sumR += p.c.redF();
            sumG += p.c.greenF();
            sumB += p.c.blueF();
        }
        double inv = 1.0 / double(neutralPixels.size());
        baseOut.setRgbF(sumR * inv, sumG * inv, sumB * inv);
    } else {
        // Absolute fallback
        baseOut = QColor(Qt::gray);
    }

    // Slight L adjustment based on dark/light
    {
        double Lcur = luminance(baseOut);
        if (darkWallpaper) {
            if (Lcur > 0.35) {
                double k = 0.35 / Lcur;
                baseOut.setRgbF(
                    clamp(baseOut.redF()   * k, 0.0, 1.0),
                    clamp(baseOut.greenF() * k, 0.0, 1.0),
                    clamp(baseOut.blueF()  * k, 0.0, 1.0)
                );
            }
        } else {
            if (Lcur < 0.45) {
                double k = 0.45 / qMax(Lcur, 0.01);
                baseOut.setRgbF(
                    clamp(baseOut.redF()   * k, 0.0, 1.0),
                    clamp(baseOut.greenF() * k, 0.0, 1.0),
                    clamp(baseOut.blueF()  * k, 0.0, 1.0)
                );
            }
        }
    }

    // Compute base OKLab for accents
    double baseL_ok, baseA_ok, baseB_ok;
    rgbToOklab(baseOut, baseL_ok, baseA_ok, baseB_ok);

    // ---------- Accent 1 ----------
    QColor accentOut;
    {
        double bestScore = -1.0;
        QColor bestColor;

        if (!highlightPixels.isEmpty()) {
            for (const auto &p : highlightPixels) {
                // Filter out very dull or extreme values
                if (p.C_ok < 0.03)
                    continue;
                if (p.L_ok < 0.25 || p.L_ok > 0.95)
                    continue;

                double dL = p.L_ok - baseL_ok;
                double dA = p.a_ok - baseA_ok;
                double dB = p.b_ok - baseB_ok;
                double distSq = dL*dL + dA*dA + dB*dB;

                double midBias = 1.0 - std::fabs(p.L_ok - 0.5);
                double chrBias = clamp(p.C_ok / 0.20, 0.4, 1.6);

                double score = distSq * (0.4 + 0.6 * midBias) * chrBias;

                // Scene tweaks
                if (scene == Scene_DarkFantasy && emotionallyWarm(p.a_ok, p.b_ok))
                    score *= 1.8;
                if (scene == Scene_Sunset && emotionallyWarm(p.a_ok, p.b_ok))
                    score *= 1.8;
                if (scene == Scene_Parchment) {
                    // prefer gold-ish and blue-ish
                    if (emotionallyWarm(p.a_ok, p.b_ok) && p.b_ok > 0.0)
                        score *= 1.4;
                    if (p.b_ok < 0.0)
                        score *= 1.4;
                }
                if (scene == Scene_Nature && p.b_ok < 0.0) {
                    // sky-ish accent
                    score *= 1.3;
                }

                if (score > bestScore) {
                    bestScore = score;
                    bestColor = p.c;
                }
            }
        }

        if (bestColor.isValid())
            accentOut = bestColor;
        else
            accentOut = autoTextForBackground(baseOut); // fallback
    }

    // ---------- Accent 2 ----------
    QColor accent2Out;
    {
        double aL, aA, aB;
        rgbToOklab(accentOut, aL, aA, aB);

        double bestScore = -1.0;
        QColor bestColor;

        if (!highlightPixels.isEmpty()) {
            for (const auto &p : highlightPixels) {
                if (p.c == accentOut)
                    continue;
                if (p.C_ok < 0.03)
                    continue;
                if (p.L_ok < 0.25 || p.L_ok > 0.90)
                    continue;

                // separation from base
                double dLb = p.L_ok - baseL_ok;
                double dAb = p.a_ok - baseA_ok;
                double dBb = p.b_ok - baseB_ok;
                double distBaseSq = dLb*dLb + dAb*dAb + dBb*dBb;

                // separation from accent1
                double dLa = p.L_ok - aL;
                double dAa = p.a_ok - aA;
                double dBa = p.b_ok - aB;
                double distAccSq = dLa*dLa + dAa*dAa + dBa*dBa;

                double sepf = distBaseSq + distAccSq;
                double midBias = 1.0 - std::fabs(p.L_ok - 0.5);

                double score = sepf * (0.4 + 0.6 * midBias);

                if (scene == Scene_Parchment && p.b_ok < 0.0)
                    score *= 1.3;

                if (score > bestScore) {
                    bestScore = score;
                    bestColor = p.c;
                }
            }
        }

        if (bestColor.isValid())
            accent2Out = bestColor;
        else
            accent2Out = accentOut;
    }

    QColor textOut = autoTextForBackground(baseOut);

    baseOut.setAlpha(255);
    textOut.setAlpha(255);
    accentOut.setAlpha(255);
    accent2Out.setAlpha(255);

    QVariantMap map;
    map.insert(QStringLiteral("base"),   baseOut);
    map.insert(QStringLiteral("text"),   textOut);
    map.insert(QStringLiteral("accent"), accentOut);
    map.insert(QStringLiteral("accent2"), accent2Out);

    m_palette = map;

    emit analysisChanged();
    emit colorsExtracted();

    qDebug() << "WallpaperAnalyzer (v7.1 final): base=" << baseOut
             << "text=" << textOut
             << "accent=" << accentOut
             << "accent2=" << accent2Out
             << "scene=" << scene
             << "darkWallpaper=" << darkWallpaper
             << "avgLum=" << avgLum;
}
