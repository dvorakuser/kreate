#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <KLocalizedContext>
#include <KLocalizedString>

#include "core/ColorSchemeBuilder.h"
#include "core/WallpaperAnalyzer.h"
#include "io/WallpaperLoader.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kreate");

    // --- Register singletons and QML types ---
    qmlRegisterSingletonType<ColorSchemeBuilder>(
        "Kreate", 1, 0, "ColorBuilder",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new ColorSchemeBuilder();
        }
    );

    qmlRegisterSingletonType<WallpaperAnalyzer>(
        "Kreate", 1, 0, "WallpaperAnalyzer",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new WallpaperAnalyzer();
        }
    );

    qmlRegisterType<WallpaperLoader>("Kreate", 1, 0, "WallpaperLoader");

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    const QUrl url(QStringLiteral("qrc:/qt/qml/Kreate/main.qml"));

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && objUrl == url)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );

    engine.load(url);
    return app.exec();
}
