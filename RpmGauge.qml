import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4
import QtQuick.Extras.Private 1.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

CircularGauge {
    id: rpmgauge

    style: CircularGaugeStyle {
        labelStepSize: 10
        labelInset: outerRadius / 2.2
        tickmarkInset: outerRadius / 4.2
        minorTickmarkInset: outerRadius / 4.2
        minimumValueAngle: -144
        maximumValueAngle: 144

        background: Rectangle {
            implicitHeight: rpmgauge.height
            implicitWidth: rpmgauge.width
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
                property int value: rpmgauge.value

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
                            degreesToRadians(valueToAngle(rpmgauge.value) - 90),
                            degreesToRadians(valueToAngle(rpmgauge.maximumValue + 1) - 90));
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

        foreground: Item {

            Text {
                id: rpmLabel
                anchors.centerIn: parent
                text: rpmgauge.value.toFixed(0)
                font.pixelSize: outerRadius * 0.3
                color: "#E02014"
                antialiasing: true
            }

            Text {
                id: unitLable
                anchors.top: rpmLabel.bottom;
                anchors.horizontalCenter: rpmLabel.horizontalCenter
                text: "RPM"
                font.pixelSize: outerRadius * 0.1
                color: "yellow"
                antialiasing: true
            }

            Text {
                id: unitLable100
                anchors.top: unitLable.bottom;
                anchors.horizontalCenter: unitLable.horizontalCenter
                text: "X 100"
                font.pixelSize: outerRadius * 0.1
                color: "yellow"
                antialiasing: true
            }
        }

        tickmarkLabel:  Text {
            font.pixelSize: Math.max(16, outerRadius * 0.05)
            text: styleData.value
            color: styleData.value <= rpmgauge.value ? "yellow" : "white"
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
            color: styleData.value <= rpmgauge.value ? "red" : "white"
        }
    }
}
