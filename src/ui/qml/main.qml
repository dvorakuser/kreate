import QtQuick
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: root
    width: 960
    height: 640
    visible: true
    title: i18n("Kreate")


    // Shared backend instance

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("Kreate")
        actions: [
            Kirigami.Action {
                text: i18n("Overview")
                onTriggered: pageStack.replace("qrc:/qt/qml/Kreate/OverviewPage.qml")
            },
            Kirigami.Action {
                text: i18n("Color Editor")
                onTriggered: pageStack.replace("qrc:/qt/qml/Kreate/ColorEditor.qml")
            },
            Kirigami.Action {
                text: i18n("Export")
                onTriggered: pageStack.replace("qrc:/qt/qml/Kreate/ExportPage.qml")
            }
        ]
    }

    pageStack.initialPage: Qt.resolvedUrl("qrc:/qt/qml/Kreate/OverviewPage.qml")
}
