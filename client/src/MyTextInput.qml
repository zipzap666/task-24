import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4


Item {
    id: textInputItem
    property string text
    property string placeHolder
    property int fontSize: 24
    property var valid
    property bool readOnly: false

    onValidChanged: {
        input.validator = valid
    }

    width: 40; height: 40;

    Rectangle {
        anchors.fill: parent
        color: "white"
        radius: 8
    }

    TextInput {
        id: input
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.topMargin: 5
        clip: true
        color: "black"
        font.pixelSize: fontSize
        text: textInputItem.text
        readOnly: parent.readOnly

        onTextChanged: {
            textInputItem.text = text
        }
    }

    Text {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.topMargin: 5
        text: textInputItem.placeHolder
        color: "#aaa"
        visible: !input.text
        font: input.font
        clip: true
    }
}
