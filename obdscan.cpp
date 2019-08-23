#include "obdscan.h"
#include "ui_obdscan.h"
#include "pid.h"

ObdScan::ObdScan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdScan)
{
    ui->setupUi(this);

    ui->labelVoltTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelVolt->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelRpmTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelRpm->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelLoadTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelLoad->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelSpeedTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelSpeed->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelCoolantTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelCoolant->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelOilTempTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelOilTemp->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelAmbientAirTempTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelAmbientAirTemp->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelIntakeAirTempTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelIntakeAirTemp->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelMafAirFlowTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelMafAirFlow->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelManifoldPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelManifoldPressure->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");    

    ui->labelFuelPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelFuelPressure->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelFuelRailLowPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelFuelRailLowPressure->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelFuelRailHighPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 6px;");
    ui->labelFuelRailHighPressure->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->pushExit->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #8F3A3A;");

    m_networkManager = NetworkManager::getInstance();

    if(m_networkManager)
    {
        connect(m_networkManager, &NetworkManager::dataReceived, this, &ObdScan::dataReceived);
        if(m_networkManager->isConnected())
        {
            m_Scan = true;
            send(runtimeCommands[commandOrder]);
            commandOrder++;
        }
    }
}

ObdScan::~ObdScan()
{
    delete ui;
}

void ObdScan::closeEvent (QCloseEvent *event)
{
    Q_UNUSED(event);
    m_Scan = false;
    emit on_close_scan();
}

void ObdScan::on_pushExit_clicked()
{
    m_Scan = false;
    close();
}

void ObdScan::send(QString &string)
{
    if(!m_networkManager->isConnected())return;
    m_networkManager->send(string);
}


void ObdScan::dataReceived(QString &data)
{
    if(!m_Scan)return;

    analysData(data);

    if(commandOrder < initializeCommands.size())
    {
        send(runtimeCommands[commandOrder]);
        commandOrder++;
    }

    if(runtimeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        send(runtimeCommands[commandOrder]);
    }
}

void ObdScan::analysData(const QString &dataReceived)
{
    if(dataReceived.isEmpty())return;

    unsigned A = 0;
    unsigned B = 0;
    unsigned PID = 0;
    double value = 0;
    bool valid;

    QString tmpmsg{};

    if(dataReceived.startsWith(QString("41")))
    {
        tmpmsg = dataReceived.mid(0, dataReceived.length());

        PID = tmpmsg.mid(2,2).toUInt(&valid,16);
        A = tmpmsg.mid(4,2).toUInt(&valid,16);
        B = tmpmsg.mid(6,2).toUInt(&valid,16);

        switch (PID)
        {
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            value = ((A * 256) + B) / 4;
            ui->labelRpm->setText(QString::number(value, 'f', 0) + " rpm");
            break;
        case 4://PID(04): Engine Load
            // A*100/255
            value = A * 100 / 255;
            ui->labelLoad->setText(QString::number(value, 'f', 0) + " %");
            break;
        case 13://PID(0D): KM Speed
            // A
            value = A;
            ui->labelSpeed->setText(QString::number(value, 'f', 0) + " km/h");
            break;
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            ui->labelCoolant->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 92://PID(05): Oil Temperature
            // A-40
            value = A - 40;
            ui->labelOilTemp->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 70://PID(46) Ambient Air Temperature
            // A-40 [DegC]
            value = A - 40;
            ui->labelAmbientAirTemp->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 15://PID(0F): Intake Air Temperature
            // A - 40
            value = A - 40;
            ui->labelIntakeAirTemp->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 11://PID(0B): Manifold Absolute Pressure
            // A - 40
            value = A;
            ui->labelManifoldPressure->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        case 16://PID(10): Air flowAir Flow Rate
             // ((256*A)+B) / 100  [g/s]
            value = ((256 * A) + B) / 100;;
            ui->labelMafAirFlow->setText(QString::number(value, 'f', 0) + " g/s");
            break;
        case 10://PID(0A): Fuel Pressure
            // A * 3
            value = A * 3;
            ui->labelFuelPressure->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        case 34://PID(22) The fuel guide rail is relative to the manifold vacuum pressureFuel
            // ((A*256)+B)*0.079
            value = ((A * 256) + B) * 0.079;
            ui->labelFuelRailLowPressure->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        case 35://PID(23) Fuel guide pressure
            // ((A*256)+B) * 10
            value = ((A*256)+B) * 10;
            ui->labelFuelRailHighPressure->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        default:
            //A
            value = A;
            break;
        }
    }
    else
    {
        if (dataReceived.contains(QRegExp("\\s*[0-9]{1,2}([.][0-9]{1,2})?V\\s*")))
        {
            ui->labelVolt->setText(dataReceived.mid(0,2) + "." + dataReceived.mid(2,1) + " V");
        }
    }
}

