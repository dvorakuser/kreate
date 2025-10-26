// FILE: kreate_5/ui/ExportPage.qml
// ------------------------------------------
// Export page for saving, validating, and packaging KDE color schemes
// Updated: Phase 3.5 Integration Sprint
// ------------------------------------------

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
import org.kde.kirigami as Kirigami
import Kreate 1.0
import Kreate.Backend 1.0

Kirigami.Page {
    id: root
    title: i18n("Export")

    // --- Backends ---
    ColorSchemeBuilder { id: builder }
    MetadataWriter { id: metadata }
    Validator { id: validator }

    // --- Export directory ---
    property string exportDir: StandardPaths.writableLocation(StandardPaths.DocumentsLocation) + "/KreateExports"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        // --- Theme metadata form ---
        Kirigami.FormLayout {
            Layout.fillWidth: true

            TextField {
                Kirigami.FormData.label: i18n("Theme name")
                text: builder.themeName
                onTextChanged: builder.themeName = text
            }

            TextField {
                Kirigami.FormData.label: i18n("Author")
                text: builder.author
                onTextChanged: builder.author = text
            }

            TextField {
                Kirigami.FormData.label: i18n("Version")
                text: builder.version
                onTextChanged: builder.version = text
            }
        }

        // --- Action buttons row ---
        RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Button {
                text: i18n("Load .colors…")
                icon.name: "document-open"
                onClicked: loadDialog.open()
            }

            Button {
                text: i18n("Save .colors…")
                icon.name: "document-save"
                onClicked: saveDialog.open()
            }

            Button {
                text: i18n("Generate metadata.json")
                icon.name: "document-export"
                onClicked: {
                    const ok = metadata.writeMetadata(
                        exportDir,
                        builder.themeName,
                        builder.author,
                        "example@email.com"
                    )
                    status.visible = ok
                    status.text = ok ? i18n("metadata.json created successfully.") : ""
                    errorMsg.visible = !ok
                    errorMsg.text = ok ? "" : i18n("Failed to create metadata.json.")
                }
            }

            Button {
                text: i18n("Open Export Folder")
                icon.name: "folder"
                onClicked: Qt.openUrlExternally("file://" + exportDir)
            }

            // --- Validate Button ---
            Button {
                text: i18n("Validate 🧩")
                icon.name: "tools-check-spelling"
                onClicked: {
                    const colorsPath = exportDir + "/" + builder.themeName + ".colors"
                    const metadataPath = exportDir + "/metadata.json"
                    const colorsOk = validator.validateColorsFile(colorsPath)
                    const metaOk = validator.validateMetadataFile(metadataPath)

                    if (colorsOk && metaOk) {
                        status.visible = true
                        status.text = i18n("Validation passed for both files.")
                        status.type = Kirigami.MessageType.Positive
                        errorMsg.visible = false
                    } else {
                        status.visible = false
                        errorMsg.visible = true
                        errorMsg.text = i18n("Validation failed. Check details below.")
                    }
                }
            }
        }

        // --- Signal connections for Validator feedback ---
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

        // --- Inline status messages ---
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

    // --- File dialogs for loading/saving color schemes ---
    FileDialog {
        id: loadDialog
        title: i18n("Open KDE .colors")
        fileMode: FileDialog.OpenFile
        nameFilters: [ i18n("KDE Color Scheme (*.colors)"), i18n("All files (*)") ]
        onAccepted: {
            const path = selectedFile.toString().replace("file://", "")
            const ok = builder.loadColorsFile(path)
            status.visible = ok
            status.text = ok ? i18n("Loaded color scheme.") : ""
            errorMsg.visible = !ok
            errorMsg.text = ok ? "" : i18n("Failed to load color scheme.")
        }
    }

    FileDialog {
        id: saveDialog
        title: i18n("Save KDE .colors")
        fileMode: FileDialog.SaveFile
        nameFilters: [ i18n("KDE Color Scheme (*.colors)") ]
        onAccepted: {
            var path = selectedFile.toString().replace("file://", "")
            if (!path.endsWith(".colors"))
                path += ".colors"
                const ok = builder.saveColorsFile(path)
                status.visible = ok
                status.text = ok ? i18n("Saved color scheme.") : ""
                errorMsg.visible = !ok
                errorMsg.text = ok ? "" : i18n("Failed to save color scheme.")
        }
    }
}
