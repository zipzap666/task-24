import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4

ApplicationWindow  {
    width: 750; height: 400
    minimumWidth: 750; minimumHeight: 400
    maximumWidth: 750;
    visible: true
    color: "#202020"
    title: qsTr("TCP client")

    MyTextInput{
        id: address
        width: 225; height: 40
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20

        placeHolder: "address"
        text: "localhost"
    }

    MyTextInput{
        id: port
        width: 100; height: 40
        anchors.left: address.right
        anchors.top: parent.top
        anchors.margins: 20

        placeHolder: "port"
        text: "33333"
        valid: IntValidator{}
    }

    MyTextInput{
        id: timeOut
        width: 225; height: 40
        anchors.left: port.right
        anchors.top: parent.top
        anchors.margins: 20

        placeHolder: "Time out"
        valid: IntValidator{
            bottom: 1
            top: 10
        }
    }

    MyButton{
        id: connect
        width: 100; height: 40
        anchors.left: timeOut.right
        anchors.top: parent.top
        anchors.margins: 20

        text: "Connect"

        onClicked: {
            client.connectToHost(address.text, parseInt(port.text), parseInt(timeOut.text))
        }
    }

    MyTextInput{
        id: slowRequest
        width: 345; height: 40
        anchors.left: parent.left
        anchors.top: address.bottom
        anchors.margins: 20

        valid: IntValidator{
            bottom: 0
        }
        placeHolder: "Time to sleep"
    }

    MyButton{
        id: sendSlow
        width: 345; height: 40
        anchors.left: parent.left
        anchors.top: slowRequest.bottom
        anchors.margins: 20

        text: "Slow request"

        onClicked: {
            client.sendSlowRequest(parseInt(slowRequest.text), parseInt(reSendTime.text))
        }
    }

    MyButton{
        id: stopSending
        width: 345; height: 40
        anchors.left: sendSlow.right
        anchors.top: slowRequest.bottom
        anchors.margins: 20

        text: "Stop sending"

        onClicked: {
            client.stopSending()
        }
    }

    MyButton{
        id: sendFast
        width: 345; height: 40
        anchors.left: sendSlow.right
        anchors.top: connect.bottom
        anchors.margins: 20

        text: "Fast request"

        onClicked: {
            client.sendFastRequest(parseInt(reSendTime.text))
        }
    }

    MyTextInput{
        id: reSendTime
        width: 345; height: 40
        anchors.left: parent.left
        anchors.top: sendSlow.bottom
        anchors.margins: 20

        placeHolder: "Time out for resend"

        valid: IntValidator{
            bottom: 10
            top: 1000
        }
    }

    MyTextInput{
        id: answer
        height: 40
        anchors.left: reSendTime.right
        anchors.top: sendSlow.bottom
        anchors.right: parent.right
        anchors.margins: 20

        readOnly: true
        placeHolder: "Answer"
    }

    TextArea{
        id: log
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: answer.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }

    Connections {
        target: client

        function onMessageParsed(message) {
            answer.text = message
        }

        function onLogGui(message){
            log.append(message)
        }
    }
}
