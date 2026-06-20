import QtQuick

Item {
    id: tile
    property url thumb
    property bool selected: false
    property bool current: false
    property bool hovered: hover.hovered
    signal clicked()

    readonly property real k: Math.tan(20 * Math.PI / 180)
    readonly property int restW: 220
    readonly property int hoverW: 364
    readonly property int overscan: Math.ceil(height * k) + 8

    width: restW
    height: 395
    clip: true

    Behavior on width { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }

    // Centered horizontal shear (matches CSS skewX(-20deg)): top leans right, bottom left,
    // shear pivots on the vertical center so neighbours stay parallel with uniform gaps.
    transform: Matrix4x4 {
        matrix: Qt.matrix4x4(1, -tile.k, 0, tile.k * tile.height / 2,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             0, 0, 0, 1)
    }

    Image {
        id: img
        source: tile.thumb
        asynchronous: true
        cache: true
        fillMode: Image.PreserveAspectCrop
        x: -tile.overscan
        y: 0
        width: tile.width + 2 * tile.overscan
        height: tile.height
        sourceSize.width: tile.hoverW + 2 * tile.overscan
        // Counter-shear (matching centered translation) keeps the photo upright and centered.
        transform: Matrix4x4 {
            matrix: Qt.matrix4x4(1, tile.k, 0, -tile.k * tile.height / 2,
                                 0, 1, 0, 0,
                                 0, 0, 1, 0,
                                 0, 0, 0, 1)
        }
    }

    // Keyboard-current highlight (focus cue; subtler than the applied ring)
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: tile.current ? 2 : 0
        border.color: Qt.rgba(1, 1, 1, 0.7)
    }

    // Selected ring (shears with the tile -> parallelogram outline)
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: tile.selected ? 3 : 0
        border.color: "#6ea8fe"
    }

    // ✓ badge, kept upright (cancel the shear tilt; stays centered via anchors)
    Rectangle {
        visible: tile.selected
        width: 34; height: 34; radius: 17
        anchors.centerIn: parent
        color: "#6ea8fe"
        transform: Matrix4x4 {
            matrix: Qt.matrix4x4(1, tile.k, 0, 0, 0,1,0,0, 0,0,1,0, 0,0,0,1)
        }
        Text { anchors.centerIn: parent; text: "✓"; color: "#0b0d12"; font.bold: true; font.pixelSize: 18 }
    }

    HoverHandler { id: hover }
    states: State {
        when: tile.current
        PropertyChanges { tile.width: tile.hoverW }
    }

    TapHandler { onTapped: tile.clicked() }
}
