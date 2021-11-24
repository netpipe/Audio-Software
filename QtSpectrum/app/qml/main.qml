import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Particles 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Rectangle {
    property real barHeight: 500;
    id: root
    width: 600
    height: 800


    Row {
        width: parent.width; height: parent.height
        Repeater{
            id: repeater
            model: 25
            ParticalView {
                width: root.width/25;
                height: parent.height;
            }
        }
    }

    FileDialog {
        id:fileSelector
        title: qsTr("选择一个音乐文件")
        selectMultiple: true;
        nameFilters: [  qsTr("*.wav") ]
        onAccepted: {
            var path = fileSelector.fileUrl.toString();
            // remove prefixed "file:///"
            path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            // unescape html codes like '%23' for '#'
            console.log(path)
            AudioPlayer.loadFile(path)
        }
    }

    Connections {

    }

    Button {
        id: btnFile
        text: "File"
        anchors.left: btnPause.right
        anchors.leftMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        onClicked: fileSelector.visible = true

    }

    Button {
        id: btnPause
        text: "Pause"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {

        }
    }

    Button {
        id: btnPlay
        text: "Play"
        anchors.right: btnPause.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        onClicked: {
            //                particalView.emitRate = 100
            //                particalView.enableEmitter = true


        }
    }
}

