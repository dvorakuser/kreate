#include "themeinstaller.h"
#include <QProcess>

ThemeInstaller::ThemeInstaller(QObject *parent)
: QObject(parent)
{
}

void ThemeInstaller::installTheme(const QString &packagePath)
{
    QProcess process;
    process.start(QStringLiteral("kpackagetool6"),
                  QStringList() << QStringLiteral("--type") << QStringLiteral("Plasma/LookAndFeel")
                  << QStringLiteral("--install") << packagePath);
    process.waitForFinished();

    bool success = (process.exitCode() == 0);
    Q_EMIT installationFinished(success,
                                success ? QStringLiteral("Theme installed successfully.")
                                : QStringLiteral("Installation failed."));
}
