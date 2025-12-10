#include "ColorSchemeBuilder.h"

#include "../palette/PaletteGenerator.h"
#include "../io/ThemeWriter.h"

#include <QDateTime>
#include <QRandomGenerator>
#include <QSettings>
#include <QDebug>
#include <cmath>

// ------------------ Helpers ----------------------
namespace {

inline double srgbToLinear(double c) {
    c /= 255.0;
    return (c <= 0.03928) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
}

inline double luminance(const QColor &c) {
    return 0.2126*srgbToLinear(c.red()) +
           0.7152*srgbToLinear(c.green()) +
           0.0722*srgbToLinear(c.blue());
}

QColor autoTextForBackground(const QColor &bg) {
    if (!bg.isValid()) return QColor("black");
    const double L = luminance(bg);
    return (L < 0.45) ? QColor("white") : QColor("black");
}

// Qt5+Qt6 compatible hue rotation
QColor rotateHue(const QColor &c, double deg)
{
    QColor h = c.toHsl();
    double H = h.hslHueF();
    if (H < 0.0) H = 0.0;

    H = std::fmod(H*360.0 + deg + 360.0, 360.0) / 360.0;

    double L = h.lightnessF();
    h.setHslF(H, h.hslSaturationF(), L);
    return h.toRgb();
}

} // namespace

// ------------------ Constructor ----------------------

ColorSchemeBuilder::ColorSchemeBuilder(QObject *parent)
    : QObject(parent)
{
    loadSettings();

    if (!m_baseColor.isValid())
        m_baseColor = QColor("white");

    if (!m_textColor.isValid())
        m_textColor = autoTextForBackground(m_baseColor);

    if (!m_accentColor.isValid())
        m_accentColor = QColor("#3daee9");

    regenerateFromState();
}

// ------------------ Getters ----------------------

QColor ColorSchemeBuilder::textColor() const {
    if (m_textColor.isValid())
        return m_textColor;
    return autoTextForBackground(m_baseColor);
}

// ------------------ Setters ----------------------

void ColorSchemeBuilder::setWindowColor(const QColor &c) {
    if (!c.isValid() || c == m_baseColor)
        return;

    m_baseColor = c;

    if (!m_textColor.isValid())
        m_textColor = autoTextForBackground(c);

    emit windowColorChanged();
    regenerateFromState();
}

void ColorSchemeBuilder::setTextColor(const QColor &c) {
    if (!c.isValid() || c == m_textColor)
        return;

    m_textColor = c;
    emit textColorChanged();
    regenerateFromState();
}

void ColorSchemeBuilder::setAccentColor(const QColor &c) {
    if (!c.isValid() || c == m_accentColor)
        return;

    m_accentColor = c;
    emit accentColorChanged();
    regenerateFromState();
}

void ColorSchemeBuilder::setColorBlindAware(bool value) {
    if (m_colorBlindAware == value)
        return;

    m_colorBlindAware = value;
    emit colorBlindAwareChanged();
    regenerateFromState();
}

void ColorSchemeBuilder::setRandomMode(bool value) {
    if (value == m_randomMode)
        return;

    m_randomMode = value;
    emit randomModeChanged();
}

void ColorSchemeBuilder::setRandomSeed(int seed) {
    quint32 s = static_cast<quint32>(seed);
    if (s == m_randomSeed)
        return;

    m_randomSeed = s;
    emit randomSeedChanged();
}

void ColorSchemeBuilder::setPaletteStyleInt(int style)
{
    PaletteStyle newStyle = PaletteStyle::BreezeStandard;

    switch (style) {
        case 1: newStyle = PaletteStyle::BreezeDimmed;       break;
        case 2: newStyle = PaletteStyle::HighContrastDark;   break;
        case 3: newStyle = PaletteStyle::HighContrastLight;  break;
        default: break;
    }

    if (newStyle == m_paletteStyle)
        return;

    m_paletteStyle = newStyle;
    emit paletteStyleChanged();
    regenerateFromState();
}

// ------------------ Palette exposure ----------------------

QVariantMap ColorSchemeBuilder::colorsVariant() const {
    QVariantMap out;
    for (auto it = m_colors.begin(); it != m_colors.end(); ++it)
        out.insert(it.key(), it.value());
    return out;
}

// ------------------ Legacy setColor ----------------------

void ColorSchemeBuilder::setColor(const QString &key, const QColor &value) {
    if (!value.isValid())
        return;

    if (key.compare("Window", Qt::CaseInsensitive) == 0) {
        setWindowColor(value);
        return;
    }
    if (key.compare("Text", Qt::CaseInsensitive) == 0) {
        setTextColor(value);
        return;
    }
    if (key.compare("Accent", Qt::CaseInsensitive) == 0) {
        setAccentColor(value);
        return;
    }

    m_colors[key] = value;
    emit colorsChanged();
}

// ------------------ generateScheme ----------------------

void ColorSchemeBuilder::generateScheme(const QColor &base,
                                        const QColor &text,
                                        bool useRandom)
{
    QColor baseC   = base.isValid() ? base : m_baseColor;
    QColor textC   = text.isValid() ? text : m_textColor;
    QColor accentC = m_accentColor;

    const bool random = useRandom || m_randomMode;

    if (random) {
        quint32 seed = m_randomSeed;
        if (seed == 0) {
            seed = static_cast<quint32>(QDateTime::currentMSecsSinceEpoch() & 0xffffffff);
        }

        m_lastUsedSeed = seed;
        emit lastUsedSeedChanged();

        QRandomGenerator rng(seed);

        bool dark = (rng.bounded(2) == 0);

        double h = rng.generateDouble() * 360.0;
        double s = 0.35 + rng.generateDouble() * 0.55;
        double l = dark
                 ? 0.14 + rng.generateDouble() * 0.14
                 : 0.70 + rng.generateDouble() * 0.22;

        baseC = QColor::fromHslF(h / 360.0, s, l);

        accentC = rotateHue(baseC, 40.0 + rng.generateDouble() * 60.0);

        textC = autoTextForBackground(baseC);
    }

    if (!textC.isValid())
        textC = autoTextForBackground(baseC);

    if (!accentC.isValid())
        accentC = QColor("#3daee9");

    m_baseColor   = baseC;
    m_textColor   = textC;
    m_accentColor = accentC;

    emit windowColorChanged();
    emit textColorChanged();
    emit accentColorChanged();

    regenerateFromState();
}

// ------------------ applyAuto (wallpaper) ----------------------

void ColorSchemeBuilder::applyAuto(const QVariantMap &map)
{
    auto getColor = [&](const QString &key, const QColor &fallback) {
        if (!map.contains(key))
            return fallback;
        const QVariant v = map.value(key);
        if (v.canConvert<QColor>())
            return v.value<QColor>();
        return QColor(v.toString());
    };

    QColor baseC   = getColor(QStringLiteral("base"),   m_baseColor);
    QColor textC   = getColor(QStringLiteral("text"),   m_textColor);
    QColor accentC = getColor(QStringLiteral("accent"), m_accentColor);

    if (!textC.isValid())
        textC = autoTextForBackground(baseC);

    if (!accentC.isValid())
        accentC = QColor("#3daee9");

    m_baseColor   = baseC;
    m_textColor   = textC;
    m_accentColor = accentC;

    emit windowColorChanged();
    emit textColorChanged();
    emit accentColorChanged();

    m_randomMode = false;
    emit randomModeChanged();

    regenerateFromState();
}

// ------------------ save ----------------------

bool ColorSchemeBuilder::save(const QString &name)
{
    return ThemeWriter::write(
        name,
        m_colors,
        m_baseColor,
        [this](int p) { emit saveProgress(p); }
    );
}

// ------------------ regenerate ----------------------

void ColorSchemeBuilder::regenerateFromState()
{
    QString styleKey = QStringLiteral("breeze");

    switch (m_paletteStyle) {
    case PaletteStyle::BreezeDimmed:
        styleKey = QStringLiteral("breeze-dimmed");
        break;
    case PaletteStyle::HighContrastDark:
        styleKey = QStringLiteral("hc-dark");
        break;
    case PaletteStyle::HighContrastLight:
        styleKey = QStringLiteral("hc-light");
        break;
    case PaletteStyle::BreezeStandard:
    default:
        break;
    }

    GeneratedPalette gp = PaletteGenerator::build(
        m_baseColor,
        m_textColor,
        m_accentColor,
        m_colorBlindAware,
        styleKey
    );

    m_colors = gp.colors;
    emit colorsChanged();
}

// ------------------ settings ----------------------

void ColorSchemeBuilder::saveSettings()
{
    QSettings s(QStringLiteral("Kreate"), QStringLiteral("ColorSchemeBuilder"));

    s.setValue(QStringLiteral("baseColor"),   m_baseColor);
    s.setValue(QStringLiteral("textColor"),   m_textColor);
    s.setValue(QStringLiteral("accentColor"), m_accentColor);
    s.setValue(QStringLiteral("colorBlind"),  m_colorBlindAware);
    s.setValue(QStringLiteral("randomMode"),  m_randomMode);
    s.setValue(QStringLiteral("randomSeed"),  m_randomSeed);
    s.setValue(QStringLiteral("lastUsedSeed"), m_lastUsedSeed);
    s.setValue(QStringLiteral("paletteStyle"), static_cast<int>(m_paletteStyle));
}

void ColorSchemeBuilder::loadSettings()
{
    QSettings s(QStringLiteral("Kreate"), QStringLiteral("ColorSchemeBuilder"));

    m_baseColor       = s.value(QStringLiteral("baseColor")).value<QColor>();
    m_textColor       = s.value(QStringLiteral("textColor")).value<QColor>();
    m_accentColor     = s.value(QStringLiteral("accentColor")).value<QColor>();
    m_colorBlindAware = s.value(QStringLiteral("colorBlind"), false).toBool();
    m_randomMode      = s.value(QStringLiteral("randomMode"), false).toBool();
    m_randomSeed      = s.value(QStringLiteral("randomSeed"), 0).toUInt();
    m_lastUsedSeed    = s.value(QStringLiteral("lastUsedSeed"), 0).toUInt();

    int idx = s.value(QStringLiteral("paletteStyle"), 0).toInt();
    switch (idx) {
    case 1: m_paletteStyle = PaletteStyle::BreezeDimmed;       break;
    case 2: m_paletteStyle = PaletteStyle::HighContrastDark;   break;
    case 3: m_paletteStyle = PaletteStyle::HighContrastLight;  break;
    default: m_paletteStyle = PaletteStyle::BreezeStandard;    break;
    }
}
