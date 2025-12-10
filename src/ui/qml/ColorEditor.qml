import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Dialogs as Dialogs
import QtQuick.Layouts
import QtCore
import org.kde.kirigami as Kirigami
import Kreate 1.0

Kirigami.Page {
    id: root
    title: i18n("Color Editor")

    property var previewColorMap: ColorBuilder.colorMap()

    Kirigami.OverlaySheet {
        id: statusSheet
        title: ""
        contentItem: Column {
            spacing: Kirigami.Units.smallSpacing
            Controls.Label { id: statusText; wrapMode: Text.Wrap }
        }
        Timer {
            id: statusTimer
            interval: 2500
            onTriggered: statusSheet.close()
        }
        onOpened: statusTimer.start()
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.largeSpacing

        // ================= LEFT PANEL =================
        ColumnLayout {
            id: leftPanel
            Layout.preferredWidth: Math.min(root.width * 0.40, Kirigami.Units.gridUnit * 42)
            Layout.fillHeight: true
            spacing: Kirigami.Units.largeSpacing

            // ---------------- CORE COLORS ----------------
            Kirigami.Heading { text: i18n("Core Colors"); level: 2 }

            RowLayout {
                spacing: Kirigami.Units.largeSpacing

                // BASE
                ColumnLayout {
                    Controls.Label { text: i18n("Base") }
                    Rectangle {
                        width: Kirigami.Units.gridUnit * 6
                        height: width
                        radius: 6
                        border.width: 1
                        color: ColorBuilder.windowColor
                        MouseArea { anchors.fill: parent; onClicked: baseDialog.open() }
                    }
                }
                Dialogs.ColorDialog {
                    id: baseDialog
                    title: i18n("Select Base Color")
                    selectedColor: ColorBuilder.windowColor
                    onAccepted: {
                        ColorBuilder.setWindowColor(selectedColor)
                        root.previewColorMap = ColorBuilder.colorMap()
                    }
                }

                // TEXT
                ColumnLayout {
                    Controls.Label { text: i18n("Text") }
                    Rectangle {
                        width: Kirigami.Units.gridUnit * 6
                        height: width
                        radius: 6
                        border.width: 1
                        color: ColorBuilder.textColor
                        MouseArea { anchors.fill: parent; onClicked: textDialog.open() }
                    }
                }
                Dialogs.ColorDialog {
                    id: textDialog
                    title: i18n("Select Text Color")
                    selectedColor: ColorBuilder.textColor
                    onAccepted: {
                        ColorBuilder.setTextColor(selectedColor)
                        root.previewColorMap = ColorBuilder.colorMap()
                    }
                }

                // ACCENT
                ColumnLayout {
                    Controls.Label { text: i18n("Accent") }
                    Rectangle {
                        width: Kirigami.Units.gridUnit * 6
                        height: width
                        radius: 6
                        border.width: 1
                        color: ColorBuilder.accentColor
                        MouseArea { anchors.fill: parent; onClicked: accentDialog.open() }
                    }
                }
                Dialogs.ColorDialog {
                    id: accentDialog
                    title: i18n("Select Accent Color")
                    selectedColor: ColorBuilder.accentColor
                    onAccepted: {
                        ColorBuilder.setAccentColor(selectedColor)
                        root.previewColorMap = ColorBuilder.colorMap()
                    }
                }
            }

            Kirigami.Separator {}

            // ------------- RANDOMIZATION & ACCESSIBILITY -------------
            Kirigami.Heading { text: i18n("Randomization & Accessibility"); level: 2 }

            GridLayout {
                columns: 2
                rowSpacing: Kirigami.Units.smallSpacing * 1.5
                columnSpacing: Kirigami.Units.smallSpacing * 3

                // Seed
                Controls.Label { text: i18n("Seed") }
                Controls.TextField {
                    id: seedField
                    placeholderText: i18n("Optional")
                    inputMethodHints: Qt.ImhDigitsOnly
                }

                // Random Theme (now immediately below seed)
                Controls.Label {}
                Controls.Button {
                    text: i18n("Random Theme")
                    onClicked: {
                        ColorBuilder.setRandomMode(true)
                        ColorBuilder.setRandomSeed(parseInt(seedField.text) || 0)
                        ColorBuilder.generateScheme(ColorBuilder.windowColor, ColorBuilder.textColor, true)
                        previewColorMap = ColorBuilder.colorMap()
                    }
                }

                // Color-blind
                Controls.Label { text: i18n("Color-Blind Friendly") }
                Controls.CheckBox {
                    id: cbToggle
                    checked: ColorBuilder.colorBlindAware
                    onToggled: {
                        ColorBuilder.setRandomMode(false)
                        ColorBuilder.setColorBlindAware(checked)
                        ColorBuilder.generateScheme(ColorBuilder.windowColor, ColorBuilder.textColor, false)
                        previewColorMap = ColorBuilder.colorMap()
                    }
                }

                // Style
                Controls.Label { text: i18n("Style") }
                Controls.ComboBox {
                    id: styleCombo
                    model: [
                        i18n("Breeze (Standard)"),
                        i18n("Breeze (Dimmed)"),
                        i18n("High-Contrast Dark"),
                        i18n("High-Contrast Light")
                    ]
                    currentIndex: ColorBuilder.paletteStyle
                    onCurrentIndexChanged: {
                        ColorBuilder.setPaletteStyleInt(currentIndex)
                        ColorBuilder.generateScheme(ColorBuilder.windowColor, ColorBuilder.textColor, false)
                        previewColorMap = ColorBuilder.colorMap()
                    }
                }

                // Generate Theme
                Controls.Label { text: i18n("Generate Theme") }
                Controls.Button {
                    text: i18n("Generate")
                    onClicked: {
                        ColorBuilder.setRandomMode(false)
                        ColorBuilder.generateScheme(ColorBuilder.windowColor, ColorBuilder.textColor, false)
                        previewColorMap = ColorBuilder.colorMap()
                    }
                }

                Controls.Label {}
                Controls.Button {
                    text: i18n("Save .colors File")
                    onClicked: nameDialog.open()
                }
            }

            Kirigami.Separator {}

            // ------------- WALLPAPER GEN -------------
            Kirigami.Heading { text: i18n("Wallpaper Theme Creation"); level: 2 }

            RowLayout {
                spacing: Kirigami.Units.largeSpacing

                ColumnLayout {
                    Controls.Button {
                        text: i18n("Import Wallpaper")
                        onClicked: wallpaperDialog.open()
                    }
                }

                ColumnLayout {
                    Rectangle {
                        width: Kirigami.Units.gridUnit * 18
                        height: Kirigami.Units.gridUnit * 10
                        border.width: 1
                        border.color: Kirigami.Theme.textColor

                        Image {
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectCrop
                            source: loader.sourceUrl
                            visible: loader.hasImage
                        }

                        Controls.Label {
                            anchors.centerIn: parent
                            visible: !loader.hasImage
                            text: i18n("No Image")
                        }
                    }


                    Controls.Button {
                        text: i18n("Generate From Wallpaper")
                        enabled: loader.hasImage
                        onClicked: {
                            WallpaperAnalyzer.setImage(loader.image)
                            WallpaperAnalyzer.analyze()

                            var pal = WallpaperAnalyzer.generatedPalette
                            console.log("[DEBUG] Wallpaper palette:", pal)

                            ColorBuilder.applyAuto(pal)
                            previewColorMap = ColorBuilder.colorMap()

                        }
                    }
                }
            }

            Dialogs.FileDialog {
                id: wallpaperDialog
                title: i18n("Select Wallpaper")
                nameFilters: ["Images (*.jpg *.jpeg *.png *.bmp *.webp)"]
                onAccepted: loader.load(selectedFile)
            }

            WallpaperLoader { id: loader }
        }

        // ================= DIVIDER =================
        Rectangle {
            width: 1
            Layout.fillHeight: true
            color: "#cccccc40"
        }

        // ================= PREVIEW PANEL =================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Kirigami.Units.smallSpacing
            border.width: 1
            border.color: "#00000040"
            color: Kirigami.Theme.backgroundColor

            ThemePreview {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing
                colorMap: previewColorMap
            }
        }
    }

    // ---------------- SAVE DIALOG ----------------
    Controls.Dialog {
        id: nameDialog
        title: i18n("Save Theme")
        modal: true
        standardButtons: Controls.DialogButtonBox.Ok | Controls.DialogButtonBox.Cancel

        Column {
            Controls.Label { text: i18n("Enter a theme name:") }
            Controls.TextField { id: themeName; placeholderText: "MyTheme" }
        }

        onAccepted: {
            const ok = ColorBuilder.save(themeName.text)
            statusSheet.title = ok ? i18n("Saved") : i18n("Error")
            statusText.text = ok
                ? i18n("Theme saved successfully.")
                : i18n("Failed to save theme.")
            statusSheet.open()
        }
    }

    Component.onCompleted: previewColorMap = ColorBuilder.colorMap()
}
