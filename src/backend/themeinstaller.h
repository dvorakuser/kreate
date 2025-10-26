#pragma once

#include <QObject>
#include <QString>

class ThemeInstaller : public QObject
{
    Q_OBJECT

public:
    explicit ThemeInstaller(QObject *parent = nullptr);

    Q_INVOKABLE void installTheme(const QString &packagePath);

signals:
    void installationFinished(bool success, const QString &message);
};
