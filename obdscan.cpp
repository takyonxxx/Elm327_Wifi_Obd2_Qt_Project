#include "obdscan.h"
#include "ui_obdscan.h"
#include "pid.h"

ObdScan::ObdScan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdScan)
{
    ui->setupUi(this);
        
    setWindowTitle("Elm327 Obd2");

    this->centralWidget()->setStyleSheet("background-image: url(:/img/carbon-fiber.jpg); border: none;");

    ui->labelVoltTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelVolt->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelRpmTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelRpm->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelLoadTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelLoad->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelSpeedTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelSpeed->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelCoolantTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelCoolant->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelIntakeAirTempTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelIntakeAirTemp->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelMafAirFlowTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelMafAirFlow->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelManifoldPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelManifoldPressure->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelFuelRailHighPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: white; padding: 6px;");
    ui->labelFuelRailHighPressure->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 6px;");

    ui->labelFuelConsumption->setStyleSheet("font-size: 48pt; font-weight: bold; color: yellow; background-color: #900C3F;  padding: 6px;");

    ui->pushExit->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #8F3A3A;");
    ui->pushExit->setMaximumHeight(100);

    m_networkManager = NetworkManager::getInstance();
    commandOrder = 0;

    if(m_networkManager)
    {
        connect(m_networkManager, &NetworkManager::dataReceived, this, &ObdScan::dataReceived);
        if(m_networkManager->isConnected())
        {
            mRunning = true;
            send(CHECK_DATA);
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
    mRunning = false;
    emit on_close_scan();
}

void ObdScan::on_pushExit_clicked()
{
    mRunning = false;
    close();
}

void ObdScan::send(QString &data)
{
    if(!mRunning)return;

    if(!m_networkManager->isConnected())return;
    m_networkManager->send(data);
}


void ObdScan::dataReceived(QString &dataReceived)
{   
    if(!mRunning)return;

    if(commandOrder < runtimeCommands.size())
    {
        send(runtimeCommands[commandOrder]);
        commandOrder++;
    }

    if(runtimeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        send(runtimeCommands[commandOrder]);
    }

    analysData(dataReceived);
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
            mSpeed = value;
            break;
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            ui->labelCoolant->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 92://PID(05): Oil Temperature
            // A-40
            value = A - 40;
            break;
        case 70://PID(46) Ambient Air Temperature
            // A-40 [DegC]
            value = A - 40;           
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
            if(!mDedectFuelPressure && mSpeed != 0)
            {
                auto fuelRate = (3600 * value)/(9069.90 * mSpeed);
                auto KmPL = (mSpeed * 1/ 3600) * (1/value * 14.6 * 710);

                ui->labelFuelConsumption->setText(QString::number(fuelRate, 'f', 1) + " L/100 km\n"
                                                  + QString::number(KmPL, 'f', 0) + " Km/L");
            }
            //
            break;
        case 10://PID(0A): Fuel Pressure
            // A * 3
            value = A * 3;            
            break;
        case 34://PID(22) The fuel guide rail is relative to the manifold vacuum pressureFuel
            // ((A*256)+B)*0.079
            value = ((A * 256) + B) * 0.079;            
            break;
        case 35://PID(23) Fuel guide pressure
            // ((A*256)+B) * 10
            value = ((A*256)+B) * 10;
            ui->labelFuelRailHighPressure->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        case 94://PID(5E) Fuel rate
            // ((A*256)+B) / 20
            if(!mDedectFuelPressure) mDedectFuelPressure = true;
            value = ((A*256)+B) / 20;
            ui->labelFuelConsumption->setText(QString::number(value, 'f', 1) + " L/h");
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

