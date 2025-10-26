#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>

/**
 * @class Validator
 * @brief Validates KDE color scheme and metadata files before export.
 *
 * This class ensures that `.colors` and `metadata.json` files used by Kreate
 * meet the KDE Plasma 6 ColorScheme and KPackage structure requirements.
 *
 * Exposed to QML as `Validator` under the `Kreate 1.0` import.
 * Emits QML-friendly signals for validation results (success, warning, error).
 */
class Validator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a Validator instance.
     * @param parent Optional QObject parent.
     */
    explicit Validator(QObject *parent = nullptr);

    /**
     * @brief Validate a KDE `.colors` file.
     *
     * Checks for required sections such as `[General]` and verifies that
     * color lines match standard hex or RGBA syntax.
     *
     * @param path Absolute path to the `.colors` file.
     * @return True if validation passed, false otherwise.
     */
    Q_INVOKABLE bool validateColorsFile(const QString &path);

    /**
     * @brief Validate a `metadata.json` file for KPackage compliance.
     *
     * Ensures required fields are present: `KPackageStructure`, `Name`, and `Id`.
     *
     * @param path Absolute path to the JSON file.
     * @return True if valid, false otherwise.
     */
    Q_INVOKABLE bool validateMetadataFile(const QString &path);

Q_SIGNALS:
    /**
     * @brief Emitted when validation passes successfully.
     * @param message Summary message for the user.
     */
    void validationPassed(const QString &message);

    /**
     * @brief Emitted when non-critical issues are found.
     * @param message Details of the warning.
     */
    void validationWarning(const QString &message);

    /**
     * @brief Emitted when validation fails.
     * @param message Details of the error encountered.
     */
    void validationFailed(const QString &message);

private:
    /**
     * @brief Internal helper that checks if a color string is valid hex/RGBA.
     * @param value Color string (e.g. "#AABBCC" or "#AABBCCDD").
     * @return True if valid, false otherwise.
     */
    bool isValidColor(const QString &value) const;
};
