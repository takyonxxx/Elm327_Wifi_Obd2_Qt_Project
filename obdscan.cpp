#include "obdscan.h"
#include "ui_obdscan.h"
#include "pid.h"
#include "elm.h"
#include "settingsmanager.h"

ObdScan::ObdScan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdScan)
{
    ui->setupUi(this);
    this->centralWidget()->setStyleSheet("background-color:#17202A ; border: none;");

    this->runtimeCommands = runtimeCommands;

    setWindowTitle("Elm327 Obd2");

    ui->labelRpmTitle->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelRpm->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelLoadTitle->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelLoad->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1;background-color: #154360;   padding: 6px; spacing: 6px;");

    ui->labelSpeedTitle->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelSpeed->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;;");

    ui->labelCoolantTitle->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelCoolant->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelManifoldTitle->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelManifold->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelVoltageTitle->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelVoltage->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelEngineDisplacement->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->comboEngineDisplacement->setStyleSheet("font-size: 18pt; font-weight: bold; color:#ECF0F1; background-color: #154360; padding: 6px; spacing: 6px;");
    ui->comboEngineDisplacement->setCurrentText(" " + QString::number(SettingsManager::getInstance()->getEngineDisplacement()));

    ui->labelFuelConsumption->setStyleSheet("font: 32pt 'Trebuchet MS'; font-weight: bold; color: #ECF0F1 ; background-color: #2E4053 ;  padding: 6px; spacing: 6px;");
    ui->labelFuel100->setStyleSheet("font: 32pt 'Trebuchet MS'; font-weight: bold; color: #ECF0F1 ; background-color: #2E4053 ;  padding: 6px; spacing: 6px;");

    ui->labelFuelConsumption->setText(QString::number(0, 'f', 1) + "  l / h");
    ui->labelFuel100->setText(QString::number(0, 'f', 1) + "  l / 100km");
    ui->labelVoltage->setText(QString::number(0, 'f', 1) + " V");

    ui->pushClear->setStyleSheet("font-size: 22pt; font-weight: bold; color: #ECF0F1; background-color: #512E5F; padding: 6px; spacing: 6px;");
    ui->pushExit->setStyleSheet("font-size: 22pt; font-weight: bold; color: #ECF0F1; background-color: #512E5F; padding: 6px; spacing: 6px;");

    ui->labelFuelConsumption->setFocus();

    mAvarageFuelConsumption.clear();
    mAvarageFuelConsumption100.clear();
    mEngineDisplacement = SettingsManager::getInstance()->getEngineDisplacement();

    runtimeCommands.clear();
    runtimeCommands.append(VOLTAGE);
    runtimeCommands.append(VEHICLE_SPEED);
    runtimeCommands.append(ENGINE_RPM);
    runtimeCommands.append(ENGINE_LOAD);
    runtimeCommands.append(COOLANT_TEMP);
    runtimeCommands.append(MAN_ABSOLUTE_PRESSURE);
    runtimeCommands.append(MAF_AIR_FLOW);

    if(ConnectionManager::getInstance() && ConnectionManager::getInstance()->isConnected())
    {
        connect(ConnectionManager::getInstance(),&ConnectionManager::dataReceived,this, &ObdScan::dataReceived);
        mRunning = true;
        send(PIDS_SUPPORTED20);
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

QString ObdScan::send(const QString &command)
{
    if(mRunning && ConnectionManager::getInstance())
    {       
        ConnectionManager::getInstance()->send(command);
    }

    return QString();
}

void ObdScan::dataReceived(QString dataReceived)
{
    if(!mRunning)return;

    if(runtimeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        send(runtimeCommands[commandOrder]);
    }

    if(commandOrder < runtimeCommands.size())
    {
        send(runtimeCommands[commandOrder]);
        commandOrder++;
    }

    if(dataReceived.isEmpty())return;

    if(dataReceived.toUpper().startsWith("UNABLETOCONNECT"))
        return;

    try
    {
        analysData(dataReceived);
    }
    catch (const std::exception& e)
    {       
    }
    catch (...)
    {        
    }

}

void ObdScan::analysData(const QString &dataReceived)
{
    unsigned A = 0;
    unsigned B = 0;
    unsigned PID = 0;
    double value = 0;

    std::vector<QString> vec;
    auto resp= elm->prepareResponseToDecode(dataReceived);

    if(resp.size()>2 && !resp[0].compare("41",Qt::CaseInsensitive))
    {
        QRegularExpression hexMatcher("^[0-9A-F]{2}$", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = hexMatcher.match(resp[1]);
        if (!match.hasMatch())
            return;

        PID =std::stoi(resp[1].toStdString(),nullptr,16);
        std::vector<QString> vec;

        vec.insert(vec.begin(),resp.begin()+2, resp.end());
        if(vec.size()>=2)
        {
            A = std::stoi(vec[0].toStdString(),nullptr,16);
            B = std::stoi(vec[1].toStdString(),nullptr,16);
        }
        else if(vec.size()>=1)
        {
            A = std::stoi(vec[0].toStdString(),nullptr,16);
            B = 0;
        }

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
            // A
            value = A;
            ui->labelManifold->setText(QString::number(value) + " kPa");
            break;
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            value = ((A * 256) + B) / 4;
            ui->labelRpm->setText(QString::number(value) + " rpm");
            break;
        case 13://PID(0D): KM Speed
            // A
            mSpeed = A;
            ui->labelSpeed->setText(QString::number(mSpeed) + " km/h");
            break;
        case 15://PID(0F): Intake Air Temperature
            // A - 40
            value = A - 40;
            break;
        case 16://PID(10): MAF air flow rate grams/sec
            // ((256*A)+B) / 100  [g/s]
            mMAF = ((256 * A) + B) / 100;
            break;
        case 17://PID(11): Throttle position
            // (100 * A) / 255 %
            value = (100 * A) / 255;
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
            value = (100 * A) / 255;
            break;
        case 92://PID(5C): Oil Temperature
            // A-40
            value = A - 40;
            break;
        case 94://PID(5E) Fuel rate
            // ((A*256)+B) / 20
        {
            value = ((A*256)+B) / 20;
            /*mAvarageFuelConsumption.append(value);
            ui->labelFuelConsumption->setText(QString::number(calculateAverage(mAvarageFuelConsumption), 'f', 1) + " l / h");*/
        }

            break;
        case 98://PID(62) Actual engine - percent torque
            // A-125
            value = A-125;
            break;
        default:
            //A
            value = A;
            break;
        }

        if(PID == 4 || PID == 12 || PID == 13) // LOAD, RPM, SPEED
        {
            auto AL = mMAF * mLoad;  // Airflow * Load
            auto coeff = (mEngineDisplacement / 1000.0) / 714.0; // Fuel flow coefficient
            auto FuelFlowLH = AL * coeff + 1.0;   // Fuel flow L/h

            if(FuelFlowLH > 99)
                FuelFlowLH = 99;

            if(mLoad == 0)
                FuelFlowLH = 0;           

            if(mSpeed > 0)
            {
                auto mFuelLPer100 = FuelFlowLH * 100 / mSpeed;   // FuelConsumption in l per 100km
                if(mFuelLPer100 > 99)
                    mFuelLPer100 = 99;

                mAvarageFuelConsumption100.append(mFuelLPer100);
                ui->labelFuel100->setText(QString::number(calculateAverage(mAvarageFuelConsumption100), 'f', 1) + "  l / 100km");
            }
            else
            {
                mAvarageFuelConsumption.append(FuelFlowLH);
                ui->labelFuelConsumption->setText(QString::number(calculateAverage(mAvarageFuelConsumption), 'f', 1) + "  l / h");
            }
        }
    }

    if (dataReceived.contains(QRegExp("\\s*[0-9]{1,2}([.][0-9]{1,2})?V\\s*")))
    {
        auto voltData = dataReceived;
        voltData.remove("ATRV").remove("atrv");
        if(voltData.length() > 3)
        {
            ui->labelVoltage->setText(voltData.mid(0,2) + "." + voltData.mid(2,1) + " V");
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
    ui->labelFuelConsumption->setText(QString::number(0, 'f', 1) + "  l / h");
    ui->labelFuel100->setText(QString::number(0, 'f', 1) + "  l / 100km");
    ui->labelVoltage->setText(QString::number(0, 'f', 1) + " V");
    mAvarageFuelConsumption.clear();
    mAvarageFuelConsumption100.clear();
}

void ObdScan::on_comboEngineDisplacement_currentIndexChanged(const QString &arg1)
{
    SettingsManager::getInstance()->setEngineDisplacement(arg1.trimmed().toInt());
    SettingsManager::getInstance()->saveSettings();
    mEngineDisplacement = SettingsManager::getInstance()->getEngineDisplacement();
}
