#pragma once

#include <QColor>
#include <QMap>
#include <QString>
#include <functional>

class ThemeWriter
{
public:
    static bool write(
        const QString &name,
        const QMap<QString, QColor> &colors,
        const QColor &windowColor,
        std::function<void(int)> progressCallback
    );
};
