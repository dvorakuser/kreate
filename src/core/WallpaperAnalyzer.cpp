#include "WallpaperAnalyzer.h"
#include <QtMath>

// --- Helpers --------------------------------------------------------------
static float luminance(const QColor &c)
{
    return 0.2126f * c.redF()
         + 0.7152f * c.greenF()
         + 0.0722f * c.blueF();
}

static float hueDegrees(const QColor &c)
{
    return c.hsvHueF() < 0 ? 0.f : c.hsvHueF() * 360.f;
}

// --- Constructor -----------------------------------------------------------
WallpaperAnalyzer::WallpaperAnalyzer(QObject *parent)
    : QObject(parent)
{
}

// --- QML API ---------------------------------------------------------------
void WallpaperAnalyzer::setImage(const QImage &img)
{
    m_image = img;
}

void WallpaperAnalyzer::analyze()
{
    if (m_image.isNull())
        return;

    QVector<PixelInfo> px = extractChromaticMidtones(m_image);

    if (px.isEmpty()) {
        float L = luminance(m_image.pixelColor(m_image.width()/2,
                                               m_image.height()/2));
        m_base = QColor::fromHslF(0, 0, L);
        m_accent = m_base.lighter(150);
        m_accent2 = m_base.darker(150);
        m_text = (L < 0.45f ? Qt::white : Qt::black);
        m_dark = (L < 0.45f);
        emit paletteChanged();
        return;
    }

    QVector<float> hist(36, 0.f);
    buildHueHistogram(px, hist);

    chooseBaseAccent(px, hist);

    emit paletteChanged();
}

// --- Return combined palette as QVariantMap --------------------------------
QVariantMap WallpaperAnalyzer::generatedPalette() const
{
    QVariantMap map;
    map["base"]   = m_base;
    map["text"]   = m_text;
    map["accent"] = m_accent;
    map["accent2"] = m_accent2;
    return map;
}

// --- Midtone Extraction -----------------------------------------------------
QVector<WallpaperAnalyzer::PixelInfo>
WallpaperAnalyzer::extractChromaticMidtones(const QImage &img)
{
    QVector<PixelInfo> out;
    out.reserve(img.width() * img.height() / 4);

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {

            QColor c = img.pixelColor(x, y);
            float L = luminance(c);

            // Expanded midtone band (0.10–0.90)
            if (L < 0.10f || L > 0.90f)
                continue;

            float S = c.hsvSaturationF();
            if (S < 0.12f) // ignore near-neutrals
                continue;

            PixelInfo p;
            p.lum = L;
            p.sat = S;
            p.hue = hueDegrees(c);
            p.rgb = c;
            out.append(p);
        }
    }
    return out;
}

// --- Build Hue Histogram ----------------------------------------------------
void WallpaperAnalyzer::buildHueHistogram(
    const QVector<PixelInfo> &px, QVector<float> &hist)
{
    for (const PixelInfo &p : px) {
        int bin = int(p.hue / 10.f) % 36;
        hist[bin] += p.sat;  // weight by saturation
    }
}

// --- Average Color for a Hue Cluster ---------------------------------------
QColor WallpaperAnalyzer::averageColorInHueRange(
    const QVector<PixelInfo> &px, float centerDeg, float rangeDeg)
{
    float total = 0.f;
    float r = 0, g = 0, b = 0;

    for (const PixelInfo &p : px) {
        float d = qAbs(p.hue - centerDeg);
        if (d > 180.f) d = 360.f - d;
        if (d <= rangeDeg) {
            r += p.rgb.redF()   * p.sat;
            g += p.rgb.greenF() * p.sat;
            b += p.rgb.blueF()  * p.sat;
            total += p.sat;
        }
    }

    if (total <= 0.f)
        return QColor(128,128,128);

    return QColor::fromRgbF(r/total, g/total, b/total);
}

// --- Select Base / Accent ---------------------------------------------------
void WallpaperAnalyzer::chooseBaseAccent(
    const QVector<PixelInfo> &px,
    const QVector<float> &hist)
{
    QVector<int> idx(36);
    for (int i = 0; i < 36; ++i) idx[i] = i;

    std::sort(idx.begin(), idx.end(),
              [&](int a, int b){ return hist[a] > hist[b]; });

    float baseHue   = idx[0] * 10.f;
    float accentHue = idx[1] * 10.f;
    float acc2Hue   = idx[2] * 10.f;

    m_base    = averageColorInHueRange(px, baseHue,   15.f);
    m_accent  = averageColorInHueRange(px, accentHue, 15.f);
    m_accent2 = averageColorInHueRange(px, acc2Hue,   15.f);

    float Lb = luminance(m_base);
    m_text = (Lb < 0.45f ? Qt::white : Qt::black);
    m_dark = (Lb < 0.45f);
}
