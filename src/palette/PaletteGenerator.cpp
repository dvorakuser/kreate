#include "PaletteGenerator.h"

#include <algorithm>
#include <cmath>

// ----------------- helpers (anonymous namespace) -----------------
namespace
{

inline double srgbToLinear(double c)
{
    c /= 255.0;
    if (c <= 0.03928)
        return c / 12.92;
    return std::pow((c + 0.055) / 1.055, 2.4);
}

inline double luminance(const QColor &c)
{
    const double r = srgbToLinear(c.red());
    const double g = srgbToLinear(c.green());
    const double b = srgbToLinear(c.blue());
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

inline double contrastRatio(const QColor &fg, const QColor &bg)
{
    const double L1 = luminance(fg) + 0.05;
    const double L2 = luminance(bg) + 0.05;
    return (L1 > L2) ? (L1 / L2) : (L2 / L1);
}

QColor autoTextForBackground(const QColor &bg)
{
    const QColor black(Qt::black);
    const QColor white(Qt::white);

    const double cBlack = contrastRatio(black, bg);
    const double cWhite = contrastRatio(white, bg);

    // Prefer whichever is better, but keep a minimum threshold.
    if (cBlack >= cWhite)
        return (cBlack >= 3.0) ? black : (cWhite > 2.2 ? white : black);
    else
        return (cWhite >= 3.0) ? white : (cBlack > 2.2 ? black : white);
}

QColor mix(const QColor &a, const QColor &b, double t)
{
    t = std::clamp(t, 0.0, 1.0);
    QColor out;
    out.setRedF  (a.redF()   * (1.0 - t) + b.redF()   * t);
    out.setGreenF(a.greenF() * (1.0 - t) + b.greenF() * t);
    out.setBlueF (a.blueF()  * (1.0 - t) + b.blueF()  * t);
    out.setAlphaF(a.alphaF() * (1.0 - t) + b.alphaF() * t);
    return out;
}

QColor withLightness(const QColor &c, double L)
{
    QColor hsl = c.toHsl();
    L = std::clamp(L, 0.0, 1.0);
    hsl.setHslF(hsl.hslHueF(), hsl.hslSaturationF(), L, c.alphaF());
    return hsl.toRgb();
}

QColor withSaturation(const QColor &c, double S)
{
    QColor hsl = c.toHsl();
    S = std::clamp(S, 0.0, 1.0);
    hsl.setHslF(hsl.hslHueF(), S, hsl.lightnessF(), c.alphaF());
    return hsl.toRgb();
}

QColor rotateHue(const QColor &c, double deg)
{
    QColor hsl = c.toHsl();
    double h = hsl.hslHueF();
    if (h < 0.0)
        h = 0.0;
    double newHue = std::fmod(h * 360.0 + deg + 360.0, 360.0) / 360.0;
    hsl.setHslF(newHue, hsl.hslSaturationF(), hsl.lightnessF(), c.alphaF());
    return hsl.toRgb();
}

QColor ensureContrastText(const QColor &candidate, const QColor &bg)
{
    if (contrastRatio(candidate, bg) >= 4.0)
        return candidate;
    return autoTextForBackground(bg);
}

} // namespace

// ----------------- PaletteGenerator::build -------------------

GeneratedPalette PaletteGenerator::build(const QColor &base,
                                         const QColor &text,
                                         const QColor &accent,
                                         bool colorBlindAware,
                                         const QString &styleKey)
{
    GeneratedPalette out;
    QMap<QString, QColor> &c = out.colors;

    // Base sanity
    QColor window = base.isValid() ? base : QColor("#404040");

    // Decide dark vs light
    const double Lbase = luminance(window);
    const bool dark = (Lbase < 0.35);

    // Use provided text color if reasonable, otherwise auto
    QColor textCandidate = text.isValid() ? text : autoTextForBackground(window);
    QColor windowText = ensureContrastText(textCandidate, window);

    // Accent: prefer user, otherwise derived from base
    QColor acc = accent.isValid()
        ? accent
        : rotateHue(withSaturation(window, 0.9), dark ? 80.0 : 60.0);

    // Style selection
    const QString key = styleKey.isEmpty()
                            ? QStringLiteral("breeze")
                            : styleKey.toLower();

    // Base semantic surfaces
    QColor view;
    QColor button;
    QColor headerBg;
    QColor headerFg;
    QColor selection;
    QColor tooltip;
    QColor complementary;

    if (key == QLatin1String("hc-dark")) {
        // Very high-contrast dark
        window       = withLightness(window, 0.07);
        view         = withLightness(window, 0.08);
        button       = withLightness(window, 0.12);
        headerBg     = withLightness(window, 0.10);
        selection    = withLightness(acc, 0.38);
        tooltip      = withLightness(window, 0.16);
        complementary= withLightness(window, 0.12);
        windowText   = QColor("#ffffff");
    } else if (key == QLatin1String("hc-light")) {
        // High-contrast light
        window       = withLightness(window, 0.90);
        view         = withLightness(window, 0.96);
        button       = withLightness(window, 0.92);
        headerBg     = withLightness(window, 0.88);
        selection    = withLightness(acc, 0.55);
        tooltip      = withLightness(window, 0.98);
        complementary= withLightness(window, 0.85);
        windowText   = QColor("#000000");
    } else if (key == QLatin1String("breeze-dimmed")) {
        if (dark) {
            window       = withLightness(window, 0.16);
            view         = withLightness(window, 0.19);
            button       = withLightness(window, 0.20);
            headerBg     = withLightness(window, 0.20);
            selection    = withLightness(acc, 0.40);
            tooltip      = withLightness(window, 0.24);
            complementary= withLightness(window, 0.18);
        } else {
            window       = withLightness(window, 0.90);
            view         = withLightness(window, 0.95);
            button       = withLightness(window, 0.92);
            headerBg     = withLightness(window, 0.88);
            selection    = withLightness(acc, 0.60);
            tooltip      = withLightness(window, 0.98);
            complementary= withLightness(window, 0.86);
        }
    } else { // "breeze" default
        if (dark) {
            window       = withLightness(window, 0.13);
            view         = withLightness(window, 0.17);
            button       = withLightness(window, 0.20);
            headerBg     = withLightness(window, 0.19);
            selection    = withLightness(acc, 0.42);
            tooltip      = withLightness(window, 0.24);
            complementary= withLightness(window, 0.18);
        } else {
            window       = withLightness(window, 0.92);
            view         = withLightness(window, 0.97);
            button       = withLightness(window, 0.94);
            headerBg     = withLightness(window, 0.89);
            selection    = withLightness(acc, 0.58);
            tooltip      = withLightness(window, 0.99);
            complementary= withLightness(window, 0.86);
        }
    }

    // Color-blind tweaks: more hue distance & extra saturation
    QColor accentAdjusted = acc;
    if (colorBlindAware) {
        accentAdjusted = rotateHue(accentAdjusted, dark ? 35.0 : 25.0);
        accentAdjusted = withSaturation(
            accentAdjusted,
            std::min(1.0, accentAdjusted.toHsl().hslSaturationF() * 1.2)
        );
        selection = accentAdjusted;
    }

    // Derive some semantic "positive/neutral/negative" from accent/base
    QColor positive = rotateHue(accentAdjusted, -30.0);
    QColor neutral  = mix(window, accentAdjusted, 0.35);
    QColor negative = rotateHue(accentAdjusted, +160.0);

    // Foregrounds
    QColor viewText    = ensureContrastText(windowText, view);
    QColor buttonText  = ensureContrastText(windowText, button);
    QColor selText     = ensureContrastText(windowText, selection);
    QColor tooltipText = ensureContrastText(windowText, tooltip);
    QColor headerText  = ensureContrastText(windowText, headerBg);
    QColor compText    = ensureContrastText(windowText, complementary);

    QColor posText     = ensureContrastText(windowText, positive);
    QColor neuText     = ensureContrastText(windowText, neutral);
    QColor negText     = ensureContrastText(windowText, negative);

    // Store main surfaces
    c["Window"]        = window;
    c["View"]          = view;
    c["Button"]        = button;
    c["Header"]        = headerBg;
    c["HeaderText"]    = headerText;
    c["Selection"]     = selection;
    c["Tooltip"]       = tooltip;
    c["Accent"]        = accentAdjusted;
    c["Complementary"] = complementary;

    // Foregrounds (primary)
    c["FG_Window"]        = windowText;
    c["FG_View"]          = viewText;
    c["FG_Button"]        = buttonText;
    c["FG_Selection"]     = selText;
    c["FG_Tooltip"]       = tooltipText;
    c["FG_Header"]        = headerText;
    c["FG_Complementary"] = compText;

    // Semantic roles
    c["Positive"]      = positive;
    c["Neutral"]       = neutral;
    c["Negative"]      = negative;
    c["FG_Positive"]   = posText;
    c["FG_Neutral"]    = neuText;
    c["FG_Negative"]   = negText;

    // Window manager-ish colors (so ThemeWriter can map easily)
    c["WM_ActiveBG"]   = headerBg;
    c["WM_ActiveFG"]   = headerText;
    c["WM_InactiveBG"] = complementary;
    c["WM_InactiveFG"] = compText;

    return out;
}
