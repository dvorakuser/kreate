#include "colorschemebuilder.h"

#include <QDebug>
#include <QFileInfo>

ColorSchemeBuilder::ColorSchemeBuilder(QObject *parent)
: QObject(parent)
, m_themeName(QStringLiteral("Untitled"))
, m_accentColor(Qt::blue)
, m_backgroundColor(Qt::white)
{
    m_colors.insert(QStringLiteral("Accent"), m_accentColor);
    m_colors.insert(QStringLiteral("Background"), m_backgroundColor);
}

void ColorSchemeBuilder::setThemeName(const QString &name)
{
    if (m_themeName == name)
        return;
    m_themeName = name;
    Q_EMIT themeNameChanged();
}

void ColorSchemeBuilder::setAccentColor(const QColor &color)
{
    if (m_accentColor == color)
        return;
    m_accentColor = color;
    m_colors.insert(QStringLiteral("Accent"), color);
    Q_EMIT accentColorChanged();
}

void ColorSchemeBuilder::setBackgroundColor(const QColor &color)
{
    if (m_backgroundColor == color)
        return;
    m_backgroundColor = color;
    m_colors.insert(QStringLiteral("Background"), color);
    Q_EMIT backgroundColorChanged();
}

bool ColorSchemeBuilder::saveToFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << QStringLiteral("[General]\n");
    out << QStringLiteral("Name=%1\n").arg(m_themeName);
    out << QStringLiteral("ColorSchemeVersion=6\n\n");
    out << QStringLiteral("[Colors]\n");

    for (auto it = m_colors.constBegin(); it != m_colors.constEnd(); ++it)
        out << it.key() << '=' << it.value().name(QColor::HexRgb) << '\n';

    file.close();
    return true;
}

bool ColorSchemeBuilder::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('[')))
            continue;

        const QStringList parts = line.split(QLatin1Char('='));
        if (parts.size() < 2)
            continue;

        const QString key = parts[0].trimmed();
        const QString value = parts[1].trimmed();

        if (key.compare(QStringLiteral("Name"), Qt::CaseInsensitive) == 0)
            setThemeName(value);
        else if (key.compare(QStringLiteral("Accent"), Qt::CaseInsensitive) == 0)
            setAccentColor(QColor(value));
        else if (key.compare(QStringLiteral("Background"), Qt::CaseInsensitive) == 0)
            setBackgroundColor(QColor(value));
    }

    file.close();
    return true;
}

bool ColorSchemeBuilder::isValidColorString(const QString &colorString)
{
    static const QRegularExpression hexColor(QStringLiteral("^#([A-Fa-f0-9]{6})$"));
    return hexColor.match(colorString).hasMatch();
}
