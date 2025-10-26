#include "metadatawriter.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

MetadataWriter::MetadataWriter(QObject *parent)
: QObject(parent)
{
}

bool MetadataWriter::writeMetadata(const QString &outputDir,
                                   const QString &themeName,
                                   const QString &author,
                                   const QString &email,
                                   const QString &license)
{
    QDir dir(outputDir);
    if (!dir.exists())
        dir.mkpath(QStringLiteral("."));

    const QString outputPath = dir.filePath(QStringLiteral("metadata.json"));
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << makeMetadataJson(themeName, author, email, license);
    file.close();
    return true;
}

QString MetadataWriter::makeMetadataJson(const QString &themeName,
                                         const QString &author,
                                         const QString &email,
                                         const QString &license) const
                                         {
                                             QJsonObject root;
                                             root.insert(QStringLiteral("KPackageStructure"), QStringLiteral("Plasma/ColorScheme"));
                                             root.insert(QStringLiteral("Name"), themeName);
                                             root.insert(QStringLiteral("Description"), QStringLiteral("A custom Plasma 6 color scheme"));
                                             root.insert(QStringLiteral("Version"), QStringLiteral("1.0"));
                                             root.insert(QStringLiteral("License"), license);

                                             QJsonArray authors;
                                             authors.append(QStringLiteral("%1 <%2>").arg(author, email));
                                             root.insert(QStringLiteral("Authors"), authors);

                                             root.insert(QStringLiteral("Id"), themeName.toLower().replace(QStringLiteral(" "), QStringLiteral("-")));

                                             QJsonDocument doc(root);
                                             return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
                                         }
