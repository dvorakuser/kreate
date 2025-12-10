import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Item {
    id: root
    property var colorMap: ({})   // provided by ColorEditor

    // Safe color lookup
    function col(key, fallback) {
        return (colorMap && key in colorMap) ? colorMap[key] : fallback;
    }

    // perceived brightness 0..1
    function brightness(c) {
        return (c.r * 0.299 + c.g * 0.587 + c.b * 0.114);
    }

    // auto-detect if theme is light or dark
    function isThemeLight() {
        // Prefer Window; fallback to View if needed
        const w = col("Window", null);
        if (w) return brightness(w) > 0.55;

        const v = col("View", null);
        if (v) return brightness(v) > 0.55;

        return false; // fallback dark
    }

    function chromeLight() {
        return isThemeLight();
    }

    // Chrome color sets
    readonly property color darkBase: "#262b33"
    readonly property color darkAlt:  "#31363b"
    readonly property color darkDeep: "#20252b"

    readonly property color lightBase: "#f0f0f0"
    readonly property color lightAlt:  "#e6e6e6"
    readonly property color lightDeep: "#ffffff"

    Rectangle {
        anchors.fill: parent
        radius: Kirigami.Units.smallSpacing
        color: chromeLight() ? lightBase : darkBase
        border.width: 1
        border.color: "#00000070"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Kirigami.Units.largeSpacing
            spacing: Kirigami.Units.largeSpacing

            // ==================== WINDOW PREVIEW ====================
            Rectangle {
                Layout.preferredWidth: Math.min(root.width, Kirigami.Units.gridUnit * 42)
                Layout.preferredHeight: Kirigami.Units.gridUnit * 30
                radius: 6
                color: chromeLight() ? lightBase : darkBase
                border.width: 1
                border.color: "#00000090"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Kirigami.Units.smallSpacing

                    // ---------------- TITLEBAR ----------------
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                        radius: 4
                        color: col("Header", col("Accent","#3daee9"))
                        border.color: "#00000080"

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: Kirigami.Units.smallSpacing
                            anchors.rightMargin: Kirigami.Units.smallSpacing

                            Controls.Label {
                                text: qsTr("Active Window")
                                color: "white"
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            // Simple window buttons
                            Rectangle { width: 12; height: 12; radius: 6; color: "#d9534f" }
                            Rectangle { width: 12; height: 12; radius: 6; color: "#f0ad4e" }
                            Rectangle { width: 12; height: 12; radius: 6; color: "#5cb85c" }
                        }
                    }

                    // ---------------- MENUBAR ----------------
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                        color: chromeLight()? lightAlt : darkAlt
                        border.color: "#00000040"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Kirigami.Units.smallSpacing

                            Repeater {
                                model: ["File","Edit","View","Tools","Help"]
                                delegate: Controls.Label {
                                    text: modelData
                                    color: chromeLight()? "#2b2b2b" : "#e6e6e6"
                                    padding: Kirigami.Units.smallSpacing
                                }
                            }
                        }
                    }

                    // ---------------- TOOLBAR ----------------
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                        color: chromeLight()? lightAlt : darkAlt
                        border.color: "#00000040"

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: Kirigami.Units.smallSpacing
                            spacing: Kirigami.Units.gridUnit

                            Controls.ToolButton { icon.name: "document-open" }
                            Controls.ToolButton { icon.name: "edit-undo" }
                            Controls.ToolButton { icon.name: "view-refresh" }
                            Controls.ToolButton { icon.name: "go-previous" }
                            Controls.ToolButton { icon.name: "go-next" }
                        }
                    }

                    // ---------------- CONTENT AREA ----------------
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        radius: 4
                        color: chromeLight()? lightDeep : darkDeep
                        border.color: "#00000040"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: Kirigami.Units.largeSpacing
                            spacing: Kirigami.Units.largeSpacing

                            // View label
                            Controls.Label {
                                text: qsTr("Window background / View area")
                                color: chromeLight()? "#2b2b2b" : "#e6e6e6"
                            }

                            // Selection highlight
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                                radius: 3
                                color: col("Selection","#3daee9")

                                Controls.Label {
                                    anchors.centerIn: parent
                                    text: qsTr("Selection highlight")
                                    color: "white"
                                }
                            }

                            // Buttons row
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: Kirigami.Units.largeSpacing

                                // Button
                                Rectangle {
                                    Layout.preferredWidth: Kirigami.Units.gridUnit * 8
                                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.6
                                    radius: 4
                                    color: col("Button","#4d5156")

                                    Controls.Label {
                                        anchors.centerIn: parent
                                        text: qsTr("Button")
                                        color: "white"
                                    }
                                }

                                // Complementary surface
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.6
                                    radius: 4
                                    color: col("Complementary",
                                               chromeLight()? "#e6e6e6" : "#31363b")

                                    Controls.Label {
                                        anchors.centerIn: parent
                                        text: qsTr("Complementary Surface")
                                        color: chromeLight()? "#2b2b2b" : "white"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
