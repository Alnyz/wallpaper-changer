import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: root
    modal: true
    // centerIn: parent (the window's contentItem), NOT Overlay.overlay, which
    // is unsized in a plain Window and pins the popup to the top-left corner.
    anchors.centerIn: parent
    width: 440
    padding: 20
    background: Rectangle { radius: 12; color: Qt.rgba(0.13,0.16,0.20,0.96); border.color: Qt.rgba(1,1,1,0.1) }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        Label { text: "Wallpaper Changer Help"; color: "white"; font.bold: true; font.pixelSize: 18 }
        Label { color: "#cfd3dc"; wrapMode: Text.WordWrap; Layout.fillWidth: true; text:
            "• Blank search shows your local collection.\n" +
            "• Type to search Wallhaven (SFW). Clear to go back.\n" +
            "• Single-click a tile to set it on all monitors.\n" +
            "• Wallhaven picks are saved into <your folder>/wallhaven/.\n" +
            "• Esc or click outside closes the overlay.\n" +
            "• Summon with the global hotkey (default Meta+O); rebind in System Settings → Shortcuts." }
        Button { Layout.alignment: Qt.AlignRight; text: "Close"; onClicked: root.close() }
    }
}
