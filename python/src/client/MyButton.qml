import QtQuick 2.15
import QtQuick.Window 2.15

Item{
    id: button
    signal clicked
    property string text: ""
    property string color: "#44A85F"

    width: 40; height: 40

    Rectangle{
        id: fill_rect
        anchors.fill: parent
        radius: 8
        color: "#44A85F"
    }

    Text{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: parent.text
        font.pixelSize: 18
        font.bold: true
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            parent.clicked()
        }

        hoverEnabled: true

        onPressed: {
            fill_rect.color = "#14683F"
        }

        onEntered: {
            fill_rect.color = "#24883F"
        }

        onExited: {
            fill_rect.color = "#44A85F"
        }
    }
}