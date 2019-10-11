#include "obdscan.h"
#include "ui_obdscan.h"
#include "pid.h"

ObdScan::ObdScan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdScan)
{
    ui->setupUi(this);

    setWindowTitle("Elm327 Obd2");

    ui->labelVoltTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelVolt->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelRpmTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelRpm->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelLoadTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelLoad->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelSpeedTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelSpeed->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelCoolantTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelCoolant->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelIntakeAirTempTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelIntakeAirTemp->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelMafAirFlowTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelMafAirFlow->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelManifoldPressureTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelManifoldPressure->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelActualTorqueTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelActualTorque->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #900C3F;  padding: 2px;");

    ui->labelEngineDisplacement->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->comboEngineDisplacement->setStyleSheet("font-size: 16pt; font-weight: bold; color: black;background-color: lightgray;  padding: 2px;");
    ui->comboEngineDisplacement->setCurrentIndex(1);

    ui->labelFuelConsumption->setStyleSheet("font-weight: bold; font: 48pt 'Trebuchet MS'; color: white; background-color: #900C3F;  padding: 2px;");
    ui->labelFuelConsumption->setText(QString::number(0, 'f', 1)
                                      + " / "
                                      + QString::number(0, 'f', 1)
                                      + "\nl / h");

    ui->pushClear->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushExit->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #8F3A3A;");

    ui->labelStatusTitle->setStyleSheet("font-size: 16pt; font-weight: bold; color: black; padding: 2px;");
    ui->labelStatus->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color:#074666;;  padding: 2px;");

    ui->labelFuelConsumption->setFocus();

    m_networkManager = NetworkManager::getInstance();
    commandOrder = 0;
    EngineDisplacement = ui->comboEngineDisplacement->currentText().toInt();

    if(m_networkManager)
    {
        connect(m_networkManager, &NetworkManager::dataReceived, this, &ObdScan::dataReceived);
        if(m_networkManager->isConnected())
        {
            mRunning = true;
            mAvarageFuelConsumption.clear();
            send(VOLTAGE);
        }
    }
}

ObdScan::~ObdScan()
{
    mAvarageFuelConsumption.clear();
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
    ui->labelStatus->setText(data);
}


void ObdScan::dataReceived(QString &dataReceived)
{

    if(!mRunning)return;

    if(dataReceived.toUpper().contains("SEARCHING"))
        return;

    if(runtimeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        send(runtimeCommands[commandOrder]);
    }
    else
    {
        send(runtimeCommands[commandOrder]);
        commandOrder++;
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

        case 4://PID(04): Engine Load
            // A*100/255
            mLoad = A * 100 / 255;
            ui->labelLoad->setText(QString::number(mLoad) + " %");
            break;
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            ui->labelCoolant->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 10://PID(0A): Fuel Pressure
            // A * 3
            value = A * 3;
            break;
        case 11://PID(0B): Manifold Absolute Pressure
            // A - 40
            value = A;
            ui->labelManifoldPressure->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            mRpm = ((A * 256) + B) / 4;
            ui->labelRpm->setText(QString::number(mRpm) + " rpm");
            break;
        case 13://PID(0D): KM Speed
            // A
            mSpeed = A;
            ui->labelSpeed->setText(QString::number(mSpeed) + " km/h");
            break;
        case 15://PID(0F): Intake Air Temperature
            // A - 40
            value = A - 40;
            ui->labelIntakeAirTemp->setText(QString::number(value, 'f', 0) + " C°");
            break;
        case 16://PID(10): MAF air flow rate grams/sec
            // ((256*A)+B) / 100  [g/s]
            mMAF = ((256 * A) + B) / 100;
            ui->labelMafAirFlow->setText(QString::number(mMAF) + " g/s");
            //
            break;
        case 17://PID(11): Throttle position
            // (100 * A) / 255 %
            mTPos = (100 * A) / 255;           
            break;
        case 33://PID(21) Distance traveled with malfunction indicator lamp (MIL) on
            // ((A*256)+B)
            value = ((A * 256) + B);
            break;
        case 34://PID(22) Fuel Rail Pressure (relative to manifold vacuum)
            // ((A*256)+B) * 0.079 kPa
            value = ((A * 256) + B) * 0.079;
            break;
        case 35://PID(23) Fuel Rail Gauge Pressure (diesel, or gasoline direct injection)
            // ((A*256)+B) * 10 kPa
            value = ((A * 256) + B) * 10;
            break;
        case 49://PID(31) Distance traveled since codes cleared
            //((A*256)+B) km
            value = ((A*256)+B);
            break;
        case 70://PID(46) Ambient Air Temperature
            // A-40 [DegC]
            value = A - 40;
            break;
        case 90://PID(5A): Relative accelerator pedal position
            // (100 * A) / 255 %
            mTPos = (100 * A) / 255;           
            break;
        case 92://PID(5C): Oil Temperature
            // A-40
            value = A - 40;
            break;
        case 94://PID(5E) Fuel rate
            // ((A*256)+B) / 20
            value = ((A*256)+B) / 20;
            ui->labelFuelConsumption->setText(QString::number(value, 'f', 1) + " L/h");
            break;
        case 98://PID(62) Actual engine - percent torque
            // A-125
            value = A-125;
            ui->labelActualTorque->setText(QString::number(value, 'f', 0) + " %");
            break;
        default:
            //A
            value = A;
            break;
        }

        if(PID == 4 || PID == 12 || PID == 13) // LOAD, RPM, SPEED
        {
            auto AL = mMAF * mLoad;                                 // Airflow * Load
            auto coeff = 0.0021;                                    // Fuel flow coefficient
            auto LH = AL * coeff + EngineDisplacement / 1000;       // Fuel flow L/h
            mFuelConsumption = LH / 2;                              // Correction for renault dacia
            //mFuelConsumption = LH * 100 / mSpeed;   // FuelConsumption in l per 100km

            mAvarageFuelConsumption.append(mFuelConsumption);
            ui->labelFuelConsumption->setText(QString::number(mFuelConsumption, 'f', 1)
                                              + " / "
                                              + QString::number(calculateAverage(mAvarageFuelConsumption), 'f', 1)
                                              + "\nl / h");
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

qreal ObdScan::calculateAverage(QVector<qreal> &listavg)
{
    qreal sum = 0.0;
    for (auto &val : listavg) {
        sum += val;
    }
    return sum / listavg.size();
}


void ObdScan::on_pushClear_clicked()
{
    ui->labelFuelConsumption->setText(QString::number(0, 'f', 1)
                                      + " / "
                                      + QString::number(0, 'f', 1)
                                      + "\nl / 100km");
    mAvarageFuelConsumption.clear();
}

void ObdScan::on_comboEngineDisplacement_currentIndexChanged(const QString &arg1)
{
    EngineDisplacement = arg1.toInt();
}
