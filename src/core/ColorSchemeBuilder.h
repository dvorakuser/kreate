#pragma once

#include <QObject>
#include <QColor>
#include <QVariantMap>
#include <QMap>

class ColorSchemeBuilder : public QObject
{
    Q_OBJECT

    // Derived palette exposed to QML
    Q_PROPERTY(QVariantMap colors READ colorsVariant NOTIFY colorsChanged)

    // Seed colors exposed to QML
    Q_PROPERTY(QColor windowColor READ windowColor NOTIFY windowColorChanged)
    Q_PROPERTY(QColor textColor   READ textColor   NOTIFY textColorChanged)
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY accentColorChanged)

    // UI flags
    Q_PROPERTY(bool colorBlindAware READ colorBlindAware NOTIFY colorBlindAwareChanged)
    Q_PROPERTY(bool randomMode READ randomMode NOTIFY randomModeChanged)
    Q_PROPERTY(int randomSeed READ randomSeed NOTIFY randomSeedChanged)

    // Required by styleCombo
    Q_PROPERTY(int paletteStyle READ paletteStyle NOTIFY paletteStyleChanged)

public:
    enum class PaletteStyle {
        BreezeStandard = 0,
        BreezeDimmed   = 1,
        HighContrastDark  = 2,
        HighContrastLight = 3
    };
    Q_ENUM(PaletteStyle)

    explicit ColorSchemeBuilder(QObject *parent = nullptr);

    // ---- QML-safe getters ----
    QColor windowColor() const { return m_baseColor.isValid() ? m_baseColor : QColor("white"); }
    QColor textColor()   const;
    QColor accentColor() const { return m_accentColor.isValid() ? m_accentColor : QColor("#3daee9"); }

    bool colorBlindAware() const { return m_colorBlindAware; }
    bool randomMode()      const { return m_randomMode; }
    int  randomSeed()      const { return static_cast<int>(m_randomSeed); }
    int  paletteStyle()    const { return static_cast<int>(m_paletteStyle); }

    // ---- Setters ----
    Q_INVOKABLE void setWindowColor(const QColor &c);
    Q_INVOKABLE void setTextColor(const QColor &c);
    Q_INVOKABLE void setAccentColor(const QColor &c);

    Q_INVOKABLE void setColorBlindAware(bool value);
    Q_INVOKABLE void setRandomMode(bool value);
    Q_INVOKABLE void setRandomSeed(int seed);
    Q_INVOKABLE void setPaletteStyleInt(int style);

    // ---- Palette access ----
    QVariantMap colorsVariant() const;
    Q_INVOKABLE QVariantMap colorMap() const { return colorsVariant(); }

    // ---- Palette generation ----
    Q_INVOKABLE void generateScheme(const QColor &base,
                                    const QColor &text,
                                    bool useRandom);

    Q_INVOKABLE void applyAuto(const QVariantMap &map);

    Q_INVOKABLE void setColor(const QString &key, const QColor &value);

    Q_INVOKABLE bool save(const QString &name);

signals:
    void windowColorChanged();
    void textColorChanged();
    void accentColorChanged();
    void colorBlindAwareChanged();
    void randomModeChanged();
    void randomSeedChanged();
    void paletteStyleChanged();
    void colorsChanged();
    void saveProgress(int percent);
    void lastUsedSeedChanged();

private:
    void regenerateFromState();
    void saveSettings();
    void loadSettings();

private:
    QColor m_baseColor;
    QColor m_textColor;
    QColor m_accentColor;

    bool m_colorBlindAware = false;
    bool m_randomMode = false;

    quint32 m_randomSeed = 0;
    quint32 m_lastUsedSeed = 0;

    PaletteStyle m_paletteStyle = PaletteStyle::BreezeStandard;

    QMap<QString, QColor> m_colors;
};
