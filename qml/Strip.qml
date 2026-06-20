import QtQuick
import QtQuick.Effects

Item {
    id: strip
    property alias model: list.model
    property int current: -1
    signal activated(int index)
    signal nearEnd()
    implicitHeight: 437

    function moveNext() {
        if (list.count === 0) return;
        strip.current = strip.current < 0 ? 0 : Math.min(strip.current + 1, list.count - 1);
        list.currentIndex = strip.current;
    }
    function movePrev() {
        if (list.count === 0) return;
        strip.current = strip.current < 0 ? 0 : Math.max(strip.current - 1, 0);
        list.currentIndex = strip.current;
    }
    function activateCurrent() {
        if (strip.current >= 0 && strip.current < list.count) strip.activated(strip.current);
    }

    ListView {
        id: list
        anchors.fill: parent
        orientation: ListView.Horizontal
        spacing: 22
        leftMargin: 0; rightMargin: 0
        boundsBehavior: Flickable.StopAtBounds
        clip: false

        // Keyboard nav drives currentIndex; ApplyRange + a move duration makes the
        // strip scroll smoothly to keep the current tile in view instead of snapping.
        currentIndex: -1
        highlightRangeMode: ListView.ApplyRange
        preferredHighlightBegin: width * 0.3
        preferredHighlightEnd: width * 0.7
        highlightMoveDuration: 220

        delegate: Item {
            id: cell
            required property url thumbUrl
            required property bool selected
            required property int index
            width: tileItem.width
            height: strip.height
            Tile {
                id: tileItem
                anchors.verticalCenter: parent.verticalCenter
                thumb: cell.thumbUrl
                selected: cell.selected
                current: cell.index === strip.current
                onHoveredChanged: if (tileItem.hovered) strip.current = cell.index
                onClicked: strip.activated(cell.index)
            }
        }

        onContentXChanged: {
            if (contentWidth > width && contentX > contentWidth - width * 1.5)
                strip.nearEnd();
        }

        // Render to a layer and mask the alpha so tiles fade out at both ends
        // (revealing the blurred desktop) instead of being darkened.
        layer.enabled: true
        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: fadeMask
            maskThresholdMin: 0.0
            maskSpreadAtMin: 1.0
        }
    }

    // Horizontal alpha gradient used as the mask: transparent at edges, opaque in the middle.
    Item {
        id: fadeMask
        anchors.fill: list
        layer.enabled: true
        visible: false
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.00; color: "transparent" }
                GradientStop { position: 0.06; color: "white" }
                GradientStop { position: 0.94; color: "white" }
                GradientStop { position: 1.00; color: "transparent" }
            }
        }
    }

    // A new search or view switch (model reset) clears the keyboard highlight;
    // appends from loadMore() keep it.
    Connections {
        target: list.model
        function onModelReset() { strip.current = -1; list.currentIndex = -1 }
    }
}
