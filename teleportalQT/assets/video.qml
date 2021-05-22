import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import org.freedesktop.gstreamer.GLVideoItem 1.0

//Rectangle {
Item {
    objectName:"rectangle"
    property var imgw: 166.0/3.0
    property var imgh: 165.0/3.0
    property var keyimgtop:475
    property var keyinterval: 3
    property var keyimgleft: 176
    property var selectcolor:"#7F888888"
    property var nomalcolor: "#00FFFFFF"
    property var arm: false
    property var mode: false
    property var speed: false

    Item {
        anchors.fill: parent

        GstGLVideoItem {
            id: video
            objectName: "videoItem"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }
        Text
         {
           id:tlabel
           objectName:"videoLabel"
           text: ""
           font.pointSize: 22
           font.bold: true
           font.family:"Arial Black"
           color: "red"
           horizontalAlignment: Text.AlignHCenter
           x:parent.width/2-contentWidth/2
           y:10

         }
         Button
        {
            id:bArm
            objectName: "ArmState"
            text:"DISARMED"
            font.pointSize: 12
            font.bold: false
            font.family:"Arial Black"
            width: 140
            height: 40
            enabled: false
            x:parent.width-150
            y:330

            background: Rectangle {             
                    //border.color: "black"
                    border.color: arm ? "green" : "red"
                    border.width: 3

            }
            //onClicked: {arm=!arm;} 
        }
        Button
        {
            id:bMode
            objectName: "DiveMode"
            text:"MANUAL"
            font.pointSize: 12
            font.bold: true
            font.family:"Arial Black"
            width: 140
            height: 40
            enabled: false
            x:parent.width-150
            y:375

            background: Rectangle {             
                    //border.color: "black"
                    border.color: mode ? "green" : "red"
                    border.width: 3
            }
           // onClicked: {mode=!mode}
        }
        Button
        {
            id:bSpeed
            objectName: "Speed"
            text:"LOW SPEED"
            font.pointSize: 12
            font.bold: true
            font.family:"Arial Black"
            width: 140
            height: 40
            enabled: false
            x:parent.width-150
            y:420
            background:Rectangle {
                    //border.color: "black"
                    border.color: speed ? "green" : "red"
                    border.width: 3}
             //onClicked: {speed=!speed}
        }
        //one
        Image
        {

                id:image_fw
                x:parent.width-keyimgleft+imgw+keyinterval
                y:keyimgtop
                source:"qrc:/assets/icons/UP.png"
                objectName:"ForWard"
                width: imgw
                height: imgh

        }
        Image
        {

                id:image_ml
                x:parent.width-keyimgleft
                y:keyimgtop+keyinterval+imgh
                source:"qrc:/assets/icons/LEFT.png"
                objectName:"MoveLeft"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_bw
                x:parent.width-keyimgleft+imgw+keyinterval
                y:keyimgtop+keyinterval+imgh
                source:"qrc:/assets/icons/DOWN.png"
                objectName:"BackWard"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_mr
                x:parent.width-keyimgleft+imgw*2+keyinterval*2
                y:keyimgtop+keyinterval+imgh
                source:"qrc:/assets/icons/RIGHT.png"
                objectName:"MoveRight"
                width: imgw
                height: imgh
        }
        //two
        Image
        {

                id:image_up
                x:image_fw.x
                y:image_mr.y+15+imgh
                source:"qrc:/assets/icons/W.png"
                objectName:"Up"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_tl
                x:image_ml.x
                y:image_up.y+imgh+keyinterval
                source:"qrc:/assets/icons/A.png"
                objectName:"TurnLeft"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_down
                x:image_bw.x
                y:image_tl.y
                source:"qrc:/assets/icons/S.png"
                objectName:"Down"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_tr
                x:image_mr.x
                y:image_tl.y
                source:"qrc:/assets/icons/D.png"
                objectName:"TurnRight"
                width: imgw
                height: imgh
        }
        //three
        Image
        {

                id:image_cu
                x:image_tl.x+imgw/2
                y:image_tl.y+imgh+20
                source:"qrc:/assets/icons/R.png"
                objectName:"CamUp"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_lu
                x:image_cu.x+imgw+keyinterval
                y:image_cu.y
                source:"qrc:/assets/icons/T.png"
                objectName:"LightsUp"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_cd
                x:image_tl.x+imgw/2
                y:image_lu.y+imgh+keyinterval
                source:"qrc:/assets/icons/F.png"
                objectName:"CamDown"
                width: imgw
                height: imgh
        }
        Image
        {

                id:image_ld
                x:image_cu.x+imgw+keyinterval
                y:image_cd.y
                source:"qrc:/assets/icons/G.png"
                objectName:"LightsDown"
                width: imgw
                height: imgh

        }

    }
}
