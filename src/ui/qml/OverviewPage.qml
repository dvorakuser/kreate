import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    title: i18n("Overview")

    Column {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing
        anchors.margins: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: i18n("Welcome to Kreate")
            level: 1
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        Controls.Label {
            text: i18n("Stage 1 verified — Kirigami shell and QML resources loaded successfully.\nUse the sidebar to explore test pages.")
            wrapMode: Text.Wrap
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
