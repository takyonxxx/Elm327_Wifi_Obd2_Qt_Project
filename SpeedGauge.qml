import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtQuick.Extras.Private 1.0
import QtGraphicalEffects 1.0

CircularGauge {
    id: speedgauge

    style: CircularGaugeStyle {
        labelStepSize: 10
        labelInset: outerRadius / 2.2
        tickmarkInset: outerRadius / 4.2
        minorTickmarkInset: outerRadius / 4.2
        minimumValueAngle: -144
        maximumValueAngle: 144

        background: Rectangle {
            implicitHeight: speedgauge.height
            implicitWidth: speedgauge.width
            color: "black"
            anchors.centerIn: parent
            radius: 360

            Image {
                anchors.fill: parent
                source: "qrc:/img/background.svg"
                asynchronous: true
                sourceSize {
                    width: width
                }
            }

            Canvas {
                property int value: speedgauge.value

                anchors.fill: parent
                onValueChanged: requestPaint()

                function degreesToRadians(degrees) {
                  return degrees * (Math.PI / 180);
                }

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.beginPath();
                    ctx.strokeStyle = "black"
                    ctx.lineWidth = outerRadius
                    ctx.arc(outerRadius,
                          outerRadius,
                          outerRadius - ctx.lineWidth / 2,
                          degreesToRadians(valueToAngle(speedgauge.value) - 90),
                          degreesToRadians(valueToAngle(speedgauge.maximumValue + 1) - 90));
                    ctx.stroke();
                }
            }
        }

        needle: Item {
            y: -outerRadius * 0.78
            height: outerRadius * 0.27
            Image {
                id: needle
                source: "qrc:/img/needle.svg"
                height: parent.height
                width: height * 0.1
                asynchronous: true
                antialiasing: true
            }

            Glow {
              anchors.fill: needle
              radius: 5
              samples: 10
              color: "white"
              source: needle
          }
        }

        foreground: Item
        {
            Text {
                id: speedLabel
                anchors.centerIn: parent
                text: speedgauge.value.toFixed(0)
                font.pixelSize: outerRadius * 0.3
                color: "#E02014"
                antialiasing: true
            }

            Text {
                id: unitLable
                anchors.top: speedLabel.bottom;
                anchors.horizontalCenter: speedLabel.horizontalCenter
                text: "km/h"
                font.pixelSize: outerRadius * 0.1
                color: "yellow"
                antialiasing: true
            }
        }

        tickmarkLabel:  Text {
            font.pixelSize: Math.max(10, outerRadius * 0.05)
            text: styleData.value
            color: styleData.value <= speedgauge.value ? "yellow" : "white"
            antialiasing: true
        }

        tickmark: Image {
            source: "qrc:/img/tickmark.svg"
            width: outerRadius * 0.018
            height: outerRadius * 0.15
            antialiasing: true
            asynchronous: true
        }

        minorTickmark: Rectangle {
            implicitWidth: outerRadius * 0.01
            implicitHeight: outerRadius * 0.03

            antialiasing: true
            smooth: true
            color: styleData.value <= speedgauge.value ? "red" : "white"
        }
    }
}
