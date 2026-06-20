import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    modal: true
    // centerIn: parent (the window's contentItem, set in Main.qml), NOT
    // Overlay.overlay, which is unsized in this layer-shell window.
    anchors.centerIn: parent
    padding: 0
    // The visible box IS the content container: the layout is anchored inside
    // this Rectangle with margins, so content width derives from the box and
    // can't overflow it.
    background: null
    contentItem: Rectangle {
        id: box
        implicitWidth: 560
        implicitHeight: col.implicitHeight + 40   // 20px margin top + bottom
        radius: 12
        color: Qt.rgba(0.13, 0.16, 0.20, 0.96)
        border.color: Qt.rgba(1, 1, 1, 0.1)
        border.width: 1

        ColumnLayout {
            id: col
            anchors.fill: parent
            anchors.margins: 20
            spacing: 12

            Label { text: "Settings"; color: "white"; font.bold: true; font.pixelSize: 18 }

            Label { text: "Wallpapers folder"; color: "#cfd3dc" }
            RowLayout {
                Layout.fillWidth: true
                TextField { id: folder; Layout.fillWidth: true; text: app.folderPath() }
                Button { text: "Browse…"; onClicked: blur.browseFolder(folder.text) }
            }

            // The summon shortcut is a cold-start global shortcut owned by the
            // installed .desktop (X-KDE-Shortcuts) and managed by KDE, NOT an
            // in-app setting. Show it read-only and point at System Settings.
            Label { text: "Summon shortcut"; color: "#cfd3dc" }
            Label {
                Layout.fillWidth: true
                Layout.preferredWidth: 0   // don't let the unwrapped implicitWidth inflate the column
                wrapMode: Text.WordWrap
                color: "#9aa0ac"
                text: "Default Meta+O. Rebind it in System Settings → Shortcuts → "
                      + "“Wallpaper Changer” (requires the app to be installed)."
            }

            RowLayout {
                Layout.fillWidth: true
                ColumnLayout {
                    Layout.fillWidth: true
                    Label { text: "Category"; color: "#cfd3dc" }
                    ComboBox {
                        id: category
                        Layout.fillWidth: true
                        model: [ {t:"All",v:"111"}, {t:"General",v:"100"}, {t:"Anime",v:"010"}, {t:"People",v:"001"} ]
                        textRole: "t"; valueRole: "v"
                        Component.onCompleted: currentIndex = Math.max(0, indexOfValue(app.wallhavenCategories()))
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    Label { text: "Sort"; color: "#cfd3dc" }
                    ComboBox {
                        id: sort
                        Layout.fillWidth: true
                        model: ["relevance","date_added","toplist","views","favorites","random"]
                        Component.onCompleted: currentIndex = Math.max(0, model.indexOf(app.wallhavenSort()))
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    Label { text: "Fit"; color: "#cfd3dc" }
                    ComboBox {
                        id: fit
                        Layout.fillWidth: true
                        model: ["Fill","Fit","Center","Tile"]
                        Component.onCompleted: currentIndex = Math.max(0, model.indexOf(app.fitMode()))
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }   // push buttons to the right
                Button { text: "Cancel"; onClicked: root.close() }
                Button {
                    text: "Save"
                    onClicked: {
                        // hotkey is not user-editable here (managed by KDE); pass the
                        // stored value through unchanged to keep the C++ signature.
                        app.saveSettings(folder.text, app.hotkey(), category.currentValue,
                                         sort.currentText, fit.currentText)
                        root.close()
                    }
                }
            }
        }
    }

    onOpened: {
        folder.text = app.folderPath()
        category.currentIndex = Math.max(0, category.indexOfValue(app.wallhavenCategories()))
        sort.currentIndex     = Math.max(0, sort.model.indexOf(app.wallhavenSort()))
        fit.currentIndex      = Math.max(0, fit.model.indexOf(app.fitMode()))
        folder.forceActiveFocus()
    }

    // Result of the native KDE folder picker (kdialog), wired from BlurBridge.
    Connections {
        target: blur
        function onFolderChosen(path) { folder.text = path }
    }
}
