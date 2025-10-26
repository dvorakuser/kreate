import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs as Dialogs
import org.kde.kirigami as Kirigami
import Kreate 1.0
import Kreate.Backend 1.0

Kirigami.Page {
    id: root
    title: "Kreate Theme Builder"

    ColorSchemeBuilder { id: builder }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: "Color Scheme Preview"
            level: 2
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Rectangle {
                width: 80; height: 80
                color: builder.backgroundColor
                border.color: "black"
                radius: 6
                Text { anchors.centerIn: parent; text: "Background" }
            }

            Rectangle {
                width: 80; height: 80
                color: builder.accentColor
                border.color: "black"
                radius: 6
                Text { anchors.centerIn: parent; text: "Accent" }
            }
        }

        Kirigami.FormLayout {
            Layout.fillWidth: true

            TextField {
                Kirigami.FormData.label: "Theme Name"
                text: builder.themeName
                onTextChanged: builder.themeName = text
            }

            Button {
                text: "Select Accent Color…"
                onClicked: accentDialog.open()
            }

            Button {
                text: "Select Background Color…"
                onClicked: bgDialog.open()
            }

            Button {
                text: "Load .colors"
                onClicked: loadDialog.open()
            }

            Button {
                text: "Save .colors"
                onClicked: saveDialog.open()
            }
        }

        Kirigami.InlineMessage {
            id: status
            visible: false
            type: Kirigami.MessageType.Positive
            text: ""
        }

        Kirigami.InlineMessage {
            id: errorMsg
            visible: false
            type: Kirigami.MessageType.Error
            text: ""
        }
    }

    // Color and file dialogs
    Dialogs.ColorDialog {
        id: accentDialog
        title: "Select Accent Color"
        selectedColor: builder.accentColor
        onAccepted: builder.accentColor = selectedColor
    }

    Dialogs.ColorDialog {
        id: bgDialog
        title: "Select Background Color"
        selectedColor: builder.backgroundColor
        onAccepted: builder.backgroundColor = selectedColor
    }

    Dialogs.FileDialog {
        id: loadDialog
        title: "Open KDE .colors"
        fileMode: Dialogs.FileDialog.OpenFile
        nameFilters: [ "KDE Color Scheme (*.colors)", "All files (*)" ]
        onAccepted: {
            const p = selectedFile.toString().replace("file://", "")
            const ok = builder.loadFromFile(p)
            status.visible = ok
            status.text = ok ? "Loaded color scheme successfully." : ""
            errorMsg.visible = !ok
            errorMsg.text = ok ? "" : "Failed to load color scheme."
        }
    }

    Dialogs.FileDialog {
        id: saveDialog
        title: "Save KDE .colors"
        fileMode: Dialogs.FileDialog.SaveFile
        nameFilters: [ "KDE Color Scheme (*.colors)" ]
        onAccepted: {
            var p = selectedFile.toString().replace("file://", "")
            if (!p.endsWith(".colors"))
                p += ".colors"
                const ok = builder.saveToFile(p)
                status.visible = ok
                status.text = ok ? "Saved .colors successfully." : ""
                errorMsg.visible = !ok
                errorMsg.text = ok ? "" : "Failed to save color scheme."
        }
    }
}
