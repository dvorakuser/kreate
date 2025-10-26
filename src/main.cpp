#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "backend/colorschemebuilder.h"
#include "backend/metadatawriter.h"
#include "backend/validator.h"
#include "backend/packagewriter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Kreate"));
    app.setOrganizationName(QStringLiteral("MiraCostaCSIT"));
    app.setOrganizationDomain(QStringLiteral("edu.miracosta.csit.kreate"));

    QQmlApplicationEngine engine;

    // Register our backend with QML
    qmlRegisterType<ColorSchemeBuilder>("Kreate", 1, 0, "ColorSchemeBuilder");
    qmlRegisterType<MetadataWriter>("Kreate.Backend", 1, 0, "MetadataWriter");
    qmlRegisterType<Validator>("Kreate", 1, 0, "Validator");
    qmlRegisterType<PackageWriter>("Kreate", 1, 0, "PackageWriter");


    const QUrl mainUrl(QStringLiteral("qrc:/ui/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [mainUrl](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == mainUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.load(mainUrl);

    return app.exec();
}
