#include "validator.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QRegularExpression>

/**
 * @file validator.cpp
 * @brief Implements validation logic for KDE color scheme and metadata files.
 *
 * This class enforces the format guidelines described in the KDE
 * Plasma ColorScheme and KPackage documentation.
 */

Validator::Validator(QObject *parent)
: QObject(parent)
{
}

/**
 * @brief Checks whether the given string is a valid hex or RGBA color definition.
 *
 * Supports both `#RRGGBB` and `#RRGGBBAA` forms.
 */
bool Validator::isValidColor(const QString &value) const
{
    static const QRegularExpression hexColor(QStringLiteral("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$"));
    return hexColor.match(value).hasMatch();
}

/**
 * @brief Validates a KDE `.colors` file.
 *
 * Ensures the file exists, contains a `[General]` section, and defines
 * at least one valid color key-value pair. Issues warnings for missing
 * optional metadata fields such as `Name=`.
 */
bool Validator::validateColorsFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Q_EMIT validationFailed(QStringLiteral("Unable to open .colors file."));
        return false;
    }

    QTextStream in(&file);
    const QString content = in.readAll();

    // Required section
    if (!content.contains(QStringLiteral("[General]"))) {
        Q_EMIT validationFailed(QStringLiteral("Missing [General] section."));
        return false;
    }

    // Optional metadata field
    if (!content.contains(QStringLiteral("Name="))) {
        Q_EMIT validationWarning(QStringLiteral("Missing Name= entry in [General]."));
    }

    // Look for valid hex color lines
    const QRegularExpression colorLine(QStringLiteral("=#[A-Fa-f0-9]{6,8}$"));
    if (!colorLine.match(content).hasMatch()) {
        Q_EMIT validationWarning(QStringLiteral("No valid color definitions found."));
    }

    Q_EMIT validationPassed(QStringLiteral(".colors file passed validation."));
    return true;
}

/**
 * @brief Validates a `metadata.json` file.
 *
 * Confirms that the JSON structure is valid and that required KDE fields
 * are present. Reports missing keys as errors.
 */
bool Validator::validateMetadataFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Q_EMIT validationFailed(QStringLiteral("Unable to open metadata.json."));
        return false;
    }

    const QByteArray data = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        Q_EMIT validationFailed(QStringLiteral("Invalid JSON format."));
        return false;
    }

    const QJsonObject obj = doc.object();
    const QStringList required = {
        QStringLiteral("KPackageStructure"),
        QStringLiteral("Name"),
        QStringLiteral("Id")
    };

    for (const QString &key : required) {
        if (!obj.contains(key)) {
            Q_EMIT validationFailed(QStringLiteral("Missing field: %1").arg(key));
            return false;
        }
    }

    Q_EMIT validationPassed(QStringLiteral("metadata.json passed validation."));
    return true;
}
