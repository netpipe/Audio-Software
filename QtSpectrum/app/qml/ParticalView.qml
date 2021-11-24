import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Particles 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Rectangle {
    id: root
    color: "#1f1f1f"
    property alias enableEmitter: emitter.enabled
    property alias emitRate: emitter.emitRate

    ParticleSystem {
        id: particleSystem
    }

    Emitter {
        id: emitter
        enabled: true
        width: 1; height: 1;
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -100
        anchors.horizontalCenter: parent.horizontalCenter
        system: particleSystem
        emitRate: 0
        lifeSpan: 700
        //            lifeSpanVariation: 10000
        size: 32
        endSize: 32
        velocity: AngleDirection {
            angle: 270
            angleVariation: 0
            magnitude: 2000
            magnitudeVariation: 0
        }
        acceleration: AngleDirection {
            angle: 90
            magnitude: 3000
        }
    }

    ImageParticle {
        source: "qrc:/images/star.png"
        system: particleSystem
//        color: '#FFD700'
        alpha: 0
        colorVariation: 0.9
        anchors.bottomMargin: 200
        rotation: 45
        rotationVariation: 0
//        rotationVelocity: 45
//        rotationVelocityVariation: 15
        entryEffect: ImageParticle.Scale
    }



}
