#pragma once

#include <QColor>
#include <QMap>
#include <QString>

/**
 * GeneratedPalette
 *
 * Minimal struct holding the full derived palette.
 * Keys follow logical KDE-ish roles (Window, View, Button, Selection, Tooltip, Accent, etc.)
 */
struct GeneratedPalette
{
    QMap<QString, QColor> colors;
};

/**
 * PaletteGenerator
 *
 * C+ model: takes a small set of inputs (base/text/accent + flags)
 * and expands them into a full Plasma-style color map.
 *
 * This is deliberately opinionated but deterministic.
 */
class PaletteGenerator
{
public:
    /**
     * Build a full palette.
     *
     * @param base            Core window/base color
     * @param text            Preferred text color (may be overridden if contrast is too low)
     * @param accent          Accent color (if invalid, derived from base)
     * @param colorBlindAware Apply extra separation & hue shifts for red/green-ish safety
     * @param styleKey        Template key, e.g. "breeze", "breeze-dimmed",
     *                        "hc-dark", "hc-light". Unknown → "breeze".
     */
    static GeneratedPalette build(const QColor &base,
                                  const QColor &text,
                                  const QColor &accent,
                                  bool colorBlindAware,
                                  const QString &styleKey);
};
