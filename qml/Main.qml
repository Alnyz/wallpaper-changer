import QtQuick
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: win
    visible: false
    color: "transparent"
    flags: Qt.FramelessWindowHint
    signal dismissRequested()

    onVisibleChanged: if (visible) { Qt.callLater(win.updateBlur); Qt.callLater(bar.focusInput) }
    onWidthChanged: Qt.callLater(win.updateBlur)
    onHeightChanged: Qt.callLater(win.updateBlur)
    function updateBlur() {
        if (typeof blur === "undefined" || blur === null) return;
        var p = bar.mapToItem(null, 0, 0);
        blur.setBarRect(p.x, p.y, bar.width, bar.height);
    }

    // Fullscreen catcher (bottom of the z-stack): clicking the empty desktop dismisses.
    MouseArea { anchors.fill: parent; onClicked: win.dismissRequested() }

    // Floating content: a centered column of [search bar] + [strip] + [status].
    Item {
        id: content
        width: win.width
        height: col.implicitHeight
        anchors.horizontalCenter: parent.horizontalCenter
        y: Math.round(parent.height * 0.16)

        // Swallow clicks anywhere on the content region so they don't dismiss
        // (tiles/bar sit above this and handle their own clicks; gaps are swallowed).
        MouseArea { anchors.fill: parent }

        ColumnLayout {
            id: col
            anchors.fill: parent
            spacing: 26

            SearchBar {
                id: bar
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: Math.min(640, content.width)
                onQueryChanged: (t) => app.setQuery(t)
                onSettingsClicked: settings.open()
                onHelpClicked: help.open()
                onNavigatePrev: strip.movePrev()
                onNavigateNext: strip.moveNext()
                onActivateRequested: strip.activateCurrent()
                onWidthChanged: Qt.callLater(win.updateBlur)
                onHeightChanged: Qt.callLater(win.updateBlur)
            }

            Strip {
                id: strip
                Layout.fillWidth: true
                model: app.model
                onActivated: (i) => app.activate(i)
                onNearEnd: app.loadMore()
            }

            Text {
                Layout.fillWidth: true
                visible: app.status.length > 0
                text: app.status
                color: "#e6e9ef"
                style: Text.Outline; styleColor: "#000000"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    // Parent the modal panels to the window's full-size contentItem so their
    // anchors.centerIn:parent centers on screen. (A Popup's default parent in
    // this layer-shell Window is unsized, which pinned them to the top-left.)
    SettingsPanel { id: settings; parent: win.contentItem }
    HelpPanel { id: help; parent: win.contentItem }
    Shortcut {
        sequence: "Escape"
        enabled: !(settings.visible || help.visible)
        onActivated: win.dismissRequested()
    }

    Item {
        anchors.fill: parent
        focus: true
        enabled: !(settings.visible || help.visible)
        Keys.onEscapePressed: win.dismissRequested()
    }
}
