#pragma once

#include <QObject>
#include <QImage>
#include <QVariantMap>
#include <QColor>

/**
 * WallpaperAnalyzer v6.5
 *
 * Scene-aware palette extractor using pure C++ heuristics with adaptive vertical zoning.
 *
 * Pipeline:
 *  1. Downsample wallpaper.
 *  2. Compute per-row luminance and approximate horizon/zone split (top/mid/bottom).
 *  3. Classify pixels into:
 *      - Body (mid-luminance, has color)
 *      - Neutral (very low saturation: parchment/stone/greys)
 *      - Highlight (bright & saturated)
 *     and assign them to zones.
 *  4. Derive coarse scene type (Generic, Sunset, Nature, Parchment, DarkFantasy)
 *     from zone-weighted hue & luminance statistics.
 *  5. Choose base color from body/neutral buckets with scene- and zone-aware weighting.
 *  6. Choose accent1 & accent2 from highlights with scene-aware tweaks.
 *
 * QML usage:
 *   WallpaperAnalyzer.setImage(loader.image)
 *   WallpaperAnalyzer.analyze()
 *   var pal = WallpaperAnalyzer.generatedPalette
 *   ColorBuilder.applyAuto(pal)
 */
class WallpaperAnalyzer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool hasImage READ hasImage NOTIFY imageChanged)
    Q_PROPERTY(QVariantMap generatedPalette READ generatedPalette NOTIFY analysisChanged)

public:
    explicit WallpaperAnalyzer(QObject *parent = nullptr);

    bool hasImage() const { return !m_image.isNull(); }
    QVariantMap generatedPalette() const { return m_palette; }

    Q_INVOKABLE void setImage(const QImage &img);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void analyze();

signals:
    void imageChanged();
    void analysisChanged();
    void colorsExtracted();

private:
    QImage m_image;
    QVariantMap m_palette;
};
