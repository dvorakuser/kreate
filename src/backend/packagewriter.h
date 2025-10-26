#pragma once

#include <QObject>
#include <QString>

/*
 * PackageWriter
 * -----------------------------
 * Responsible for writing a KDE color scheme package (.colors + metadata).
 * Exposed to QML for use in ExportPage.qml and toolbar actions.
 */
class PackageWriter : public QObject
{
    Q_OBJECT

public:
    explicit PackageWriter(QObject *parent = nullptr);

    Q_INVOKABLE bool writePackage(const QString &outputDir,
                                  const QString &themeName);

Q_SIGNALS:
    void packageWritten(const QString &path);
    void errorOccurred(const QString &message);
};
