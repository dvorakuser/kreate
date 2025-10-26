// FILE: kreate_5/ui/ExportPage.qml (MODIFIED)

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import Kreate.Backend 1.0
import Kreate 1.0 // Need to import Kreate 1.0 for Validator and ColorSchemeBuilder (which is here)

Kirigami.Page {
    id: root
    title: i18n("Export")

    // Backends
    ColorSchemeBuilder { id: builder },
    MetadataWriter { id: metadata },
    Validator { id: validator } // <--- ADDED: Instantiate Validator backend

    property string exportDir: StandardPaths.writableLocation(StandardPaths.DocumentsLocation) + "/KreateExports"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing [cite: 14]

        Kirigami.FormLayout {
            Layout.fillWidth: true

            TextField {
                Kirigami.FormData.label: i18n("Theme name")
                text: builder.themeName
                onTextChanged: builder.themeName = text [cite: 15]
            }

            TextField {
                Kirigami.FormData.label: i18n("Author")
                text: builder.author
                onTextChanged: builder.author = text
            }

            TextField {
                Kirigami.FormData.label: i18n("Version") [cite: 16]
                text: builder.version
                onTextChanged: builder.version = text
            }
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Button {
                text: i18n("Load .colors…")
                icon.name: "document-open"
                onClicked: loadDialog.open() [cite: 17]
            }

            Button {
                text: i18n("Save .colors…") [cite: 18]
                icon.name: "document-save"
                onClicked: saveDialog.open()
            }

            // <--- ADDED: Validate Button ---
            Button {
                text: i18n("Validate Theme")
                icon.name: "document-properties"
                onClicked: {
                    const colorsPath = builder.colorsFilePath // Assumes builder has a path to the last saved/loaded .colors
                    const metadataPath = exportDir + "/metadata.json"

                    // 1. Clear messages
                    status.visible = false
                    errorMsg.visible = false

                    // 2. Validate .colors file (optional, depends on file being saved/loaded)
                    // We'll skip file path validation for simplicity here, assuming the user saved the file first.

                    // 3. Validate metadata.json file
                    const metaOk = validator.validateMetadataFile(metadataPath)

                    if (metaOk) {
                        status.type = Kirigami.MessageType.Positive
                        status.text = i18n("Theme validation passed.")
                        status.visible = true
                    }
                }
            }
            // -----------------------------

            Button {
                text: i18n("Generate metadata.json")
                icon.name: "document-export"
                onClicked: { [cite: 19]
                    const ok = metadata.writeMetadata(
                        exportDir,
                        builder.themeName,
                        builder.author, [cite: 20]
                        "example@email.com"
                    )
                    status.visible = ok
                    status.text = ok ?
                    i18n("metadata.json created successfully.") : "" [cite: 21]
                    errorMsg.visible = !ok
                    errorMsg.text = ok ?
                    "" : i18n("Failed to create metadata.json.") [cite: 22]
                }
            }

            Button {
                text: i18n("Open Export Folder")
                icon.name: "folder"
                onClicked: Qt.openUrlExternally("file://" [cite: 23] + exportDir)
            }
        }

        // <--- ADDED: Signal connections to Validator ---
        Component.onCompleted: {
            validator.validationPassed.connect(function(message) {
                status.type = Kirigami.MessageType.Positive
                status.text = i18n("Validation Passed: ") + message
                status.visible = true
                errorMsg.visible = false
            })
            validator.validationWarning.connect(function(message) {
                status.type = Kirigami.MessageType.Warning
                status.text = i18n("Validation Warning: ") + message
                status.visible = true
                errorMsg.visible = false
            })
            validator.validationFailed.connect(function(message) {
                errorMsg.text = i18n("Validation Failed: ") + message
                errorMsg.visible = true
                status.visible = false
            })
        }
        // ---------------------------------------------

        Kirigami.InlineMessage {
            id: status
            visible: false [cite: 24]
            type: Kirigami.MessageType.Positive
            text: ""
        }

        Kirigami.InlineMessage {
            id: errorMsg
            visible: false
            type: Kirigami.MessageType.Error
            text: "" [cite: 24]
        }
    }

    FileDialog {
        id: loadDialog
        title: i18n("Open KDE .colors")
        fileMode: FileDialog.OpenFile
        nameFilters: [ i18n("KDE Color Scheme (*.colors)"), i18n("All files (*)") ] [cite: 25]
        onAccepted: {
            const path = selectedFile.toString().replace("file://", "")
            const ok = builder.loadColorsFile(path)
            status.visible = ok
            status.text = ok ?
            i18n("Loaded color scheme.") : "" [cite: 26]
            errorMsg.visible = !ok
            errorMsg.text = ok ?
            "" : i18n("Failed to load color scheme.") [cite: 27]
        }
    }

    FileDialog {
        id: saveDialog
        title: i18n("Save KDE .colors")
        fileMode: FileDialog.SaveFile
        nameFilters: [ i18n("KDE Color Scheme (*.colors)") ]
        onAccepted: {
            var path = selectedFile.toString().replace("file://", "")
            if (!path.endsWith(".colors")) [cite: 28]
                path += ".colors"
                const ok = builder.saveColorsFile(path)
                status.visible = ok
                status.text = ok ?
                i18n("Saved color scheme.") : "" [cite: 29]
                errorMsg.visible = !ok
                errorMsg.text = ok ?
                "" : i18n("Failed to save color scheme.") [cite: 30]
        }
    }
}
