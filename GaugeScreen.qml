import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

ApplicationWindow {

    id: gaugeWindow
    color: "black"

    Image {
        id: background
        source: "qrc:/img/carbon.jpg";
        Layout.fillWidth: true
        Layout.fillHeight: true;
        anchors.fill: parent;
        Layout.alignment: Qt.AlignHCenter|Qt.AlignVCenter;
        fillMode: Image.Stretch;
        asynchronous: true
    }

    ColumnLayout {
        id: gaugeLayout
        anchors.fill: parent
        anchors.margins: 10

        GridLayout {
            columns: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignHCenter|Qt.AlignVCenter

            SpeedGauge
            {
                id: speedometer
                property bool accelerating: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter|Qt.AlignVCenter

                value: 0
                maximumValue: 200

                Component.onCompleted: forceActiveFocus()

                Behavior on value { NumberAnimation { duration: 1000 }}

            }// speedometer

            RpmGauge
            {
                id: rpm
                property bool accelerating: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter|Qt.AlignVCenter

                value: 0
                maximumValue: 80

                Component.onCompleted: forceActiveFocus()

                Behavior on value { NumberAnimation { duration: 1000 }}

            }// rpm

        }//GridLayout
    }// ColumnLayout

    function setSpeed(value)
    {
        speedometer.value = value;
    }

    function setRpm(value)
    {
        rpm.value = value;
    }
}
