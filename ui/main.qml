import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: win
    width: 900
    height: 640
    visible: true
    title: "Kreate Theme Studio"

    pageStack.initialPage: Qt.resolvedUrl("MainPage.qml")

    globalDrawer: Kirigami.GlobalDrawer {
        actions: [
            Kirigami.Action {
                text: "Home"
                onTriggered: pageStack.replace(Qt.resolvedUrl("MainPage.qml"))
            },
            Kirigami.Action {
                text: "Export"
                onTriggered: pageStack.replace(Qt.resolvedUrl("ExportPage.qml"))
            }
        ]
    }
}
