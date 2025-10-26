#ifndef COLORSCHEMEBUILDER_H
#define COLORSCHEMEBUILDER_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QMap>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>

/**
 * @class ColorSchemeBuilder
 * @brief Provides backend logic for reading, writing, and validating KDE Plasma 6 .colors files.
 *
 * Exposes themeName, accentColor, and backgroundColor to QML.
 * Follows Qt 6/KF6 property-notification and QStringLiteral conventions.
 */
class ColorSchemeBuilder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY themeNameChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
    explicit ColorSchemeBuilder(QObject *parent = nullptr);

    // Accessors
    QString themeName() const { return m_themeName; }
    QColor accentColor() const { return m_accentColor; }
    QColor backgroundColor() const { return m_backgroundColor; }

    // Mutators
    Q_INVOKABLE void setThemeName(const QString &name);
    Q_INVOKABLE void setAccentColor(const QColor &color);
    Q_INVOKABLE void setBackgroundColor(const QColor &color);

    // File operations
    Q_INVOKABLE bool saveToFile(const QString &filePath);
    Q_INVOKABLE bool loadFromFile(const QString &filePath);

    // Helpers
    static bool isValidColorString(const QString &colorString);

Q_SIGNALS:
    void themeNameChanged();
    void accentColorChanged();
    void backgroundColorChanged();

private:
    QString m_themeName;
    QColor  m_accentColor;
    QColor  m_backgroundColor;
    QMap<QString, QColor> m_colors;
};

#endif // COLORSCHEMEBUILDER_H
