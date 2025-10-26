#pragma once

#include <QObject>
#include <QString>

class MetadataWriter : public QObject
{
    Q_OBJECT
public:
    explicit MetadataWriter(QObject *parent = nullptr);

    Q_INVOKABLE bool writeMetadata(const QString &outputDir,
                                   const QString &themeName,
                                   const QString &author,
                                   const QString &email,
                                   const QString &license = QStringLiteral("GPL-3.0"));

private:
    QString makeMetadataJson(const QString &themeName,
                             const QString &author,
                             const QString &email,
                             const QString &license) const;
};
