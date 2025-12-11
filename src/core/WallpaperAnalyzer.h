#pragma once

#include <QObject>
#include <QImage>
#include <QColor>
#include <QVariantMap>
#include <QVector>

class WallpaperAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit WallpaperAnalyzer(QObject *parent = nullptr);

    // QML API — matches your existing calls
    Q_INVOKABLE void setImage(const QImage &img);
    Q_INVOKABLE void analyze();

    // Exposed palette results
    Q_PROPERTY(QVariantMap generatedPalette READ generatedPalette NOTIFY paletteChanged)
    QVariantMap generatedPalette() const;

    // Optional individual accessors
    Q_PROPERTY(QColor baseColor READ baseColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor accentColor2 READ accentColor2 NOTIFY paletteChanged)
    Q_PROPERTY(bool darkWallpaper READ darkWallpaper NOTIFY paletteChanged)

    QColor baseColor()    const { return m_base; }
    QColor textColor()    const { return m_text; }
    QColor accentColor()  const { return m_accent; }
    QColor accentColor2() const { return m_accent2; }
    bool darkWallpaper()  const { return m_dark; }

signals:
    void paletteChanged();

private:
    struct PixelInfo {
        float lum;
        float hue;
        float sat;
        QColor rgb;
    };

    QVector<PixelInfo> extractChromaticMidtones(const QImage &img);
    void buildHueHistogram(const QVector<PixelInfo> &px, QVector<float> &hist);
    QColor averageColorInHueRange(const QVector<PixelInfo> &px,
                                  float centerDeg, float rangeDeg);
    void chooseBaseAccent(const QVector<PixelInfo> &px,
                          const QVector<float> &hist);

    // Stored QImage
    QImage m_image;

    // Palette results
    QColor m_base;
    QColor m_text;
    QColor m_accent;
    QColor m_accent2;
    bool m_dark = false;
};
