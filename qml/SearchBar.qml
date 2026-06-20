import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

// KRunner-style search bar (modelled on plasma-workspace krunner/qml/RunCommand.qml,
// minus the pin button). Pure QtQuick; icons are bundled Breeze SVGs recoloured to
// the Nord foreground so it matches the active Colloid Dark Nord scheme.
Rectangle {
    id: bar

    signal queryChanged(string text)
    signal settingsClicked()
    signal helpClicked()
    signal navigatePrev()
    signal navigateNext()
    signal activateRequested()
    property alias text: field.text

    function focusInput() { field.forceActiveFocus() }

    // --- Nord / Breeze palette (matches KRunner under Colloid Dark Nord) ---
    readonly property color colWindow:      Qt.rgba(0.180, 0.204, 0.251, 0.85) // nord0
    readonly property color colField:       Qt.rgba(0.146, 0.163, 0.205, 0.60) // sunken
    readonly property color colBorder:      Qt.rgba(1, 1, 1, 0.12)
    readonly property color colAccent:      "#89a3c2"                            // live scheme highlight (Colors:Selection)
    readonly property color colText:        "#e5e9f0"                            // nord5
    readonly property color colPlaceholder: "#7b8394"
    readonly property color colHover:       Qt.rgba(1, 1, 1, 0.08)

    implicitHeight: 46
    radius: 8
    color: colWindow
    border.color: colBorder
    border.width: 1

    // flat ToolButton, Breeze style: transparent, background highlight on hover
    component ToolBtn: Rectangle {
        id: btn
        property alias source: img.source
        signal clicked()
        Layout.preferredWidth: 34
        Layout.preferredHeight: 34
        radius: 6
        color: hover.hovered ? bar.colHover : "transparent"
        Image {
            id: img
            anchors.centerIn: parent
            width: 18; height: 18
            sourceSize: Qt.size(width, height)
            smooth: true
        }
        HoverHandler { id: hover }
        TapHandler { onTapped: btn.clicked() }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        // configure (left)
        ToolBtn {
            source: "icons/configure.svg"
            onClicked: bar.settingsClicked()
        }

        // search field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 34
            radius: 6
            color: bar.colField
            border.width: 1
            border.color: field.activeFocus ? bar.colAccent : bar.colBorder

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 6
                spacing: 6

                // left search icon (muted, like KRunner)
                Image {
                    source: "icons/search.svg"
                    width: 16; height: 16
                    sourceSize: Qt.size(width, height)
                    opacity: 0.65
                    smooth: true
                }

                TextField {
                    id: field
                    Layout.fillWidth: true
                    background: null
                    leftPadding: 0; rightPadding: 0
                    color: bar.colText
                    placeholderText: "Search…"
                    placeholderTextColor: bar.colPlaceholder
                    font.pixelSize: 14
                    selectByMouse: true
                    onTextChanged: bar.queryChanged(text)
                    // Left/Right/Enter drive the tile strip instead of the text cursor
                    Keys.onLeftPressed: (event) => { bar.navigatePrev(); event.accepted = true }
                    Keys.onRightPressed: (event) => { bar.navigateNext(); event.accepted = true }
                    Keys.onReturnPressed: (event) => { bar.activateRequested(); event.accepted = true }
                    Keys.onEnterPressed: (event) => { bar.activateRequested(); event.accepted = true }
                    // let the parent window handle Esc (dismiss)
                    Keys.onEscapePressed: (event) => { event.accepted = false }
                }

                // clear button, visible only while there is text
                Rectangle {
                    Layout.preferredWidth: 22
                    Layout.preferredHeight: 22
                    radius: 4
                    visible: field.text.length > 0
                    color: clearHover.hovered ? bar.colHover : "transparent"
                    Image {
                        anchors.centerIn: parent
                        source: "icons/edit-clear.svg"
                        width: 14; height: 14
                        sourceSize: Qt.size(width, height)
                        opacity: 0.75
                        smooth: true
                    }
                    HoverHandler { id: clearHover }
                    TapHandler {
                        onTapped: {
                            field.clear()
                            field.forceActiveFocus()
                        }
                    }
                }
            }
        }

        // help (right), no pin button
        ToolBtn {
            source: "icons/help.svg"
            onClicked: bar.helpClicked()
        }
    }
}
