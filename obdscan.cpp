#include "obdscan.h"
#include "ui_obdscan.h"

ObdScan::ObdScan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdScan)
{
    ui->setupUi(this);
    this->centralWidget()->setStyleSheet("background-color:#17202A ; border: none;");

    setWindowTitle("Elm327 Obd2");

    ui->labelRpmTitle->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelRpm->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelMapTitle->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelMap->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelSpeedTitle->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelSpeed->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;;");

    ui->labelCoolantTitle->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; padding: 6px; spacing: 6px;");
    ui->labelCoolant->setStyleSheet("font-size: 32pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->labelVoltage->setStyleSheet("font-size: 36pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");   
    ui->labelVoltage->setText(QString::number(0, 'f', 1) + " V");
    ui->labelCommand->setStyleSheet("font-size: 24pt; font-weight: bold; color: #ECF0F1; background-color: #154360 ; padding: 6px; spacing: 6px;");

    ui->pushExit->setStyleSheet("font-size: 22pt; font-weight: bold; color: #ECF0F1; background-color: #512E5F; padding: 6px; spacing: 6px;");

    runtimeCommands.clear();

    if(runtimeCommands.isEmpty())
    {
        runtimeCommands.append(VOLTAGE);
        runtimeCommands.append(MAN_ABSOLUTE_PRESSURE);
        runtimeCommands.append(VEHICLE_SPEED);
        runtimeCommands.append(ENGINE_RPM);
        runtimeCommands.append(ENGINE_LOAD);
        runtimeCommands.append(COOLANT_TEMP);
        //0104, 0105, 010B, 010C, 010D, 010F, 0110, 0111, 011C
    }

     startQueue();

    //    if(ConnectionManager::getInstance() && ConnectionManager::getInstance()->isConnected())
    //    {
    //        connect(ConnectionManager::getInstance(),&ConnectionManager::dataReceived,this, &ObdScan::dataReceived);
    //        mRunning = true;
    //        send(VOLTAGE);
    //    }
}

ObdScan::~ObdScan()
{
    delete ui;
}

void ObdScan::startQueue()
{
    m_realTime = 0;
    m_timerId  = startTimer(interval);
    m_time.start();
}

void ObdScan::stopQueue()
{
    if ( m_timerId ) killTimer( m_timerId );
}

void ObdScan::closeEvent (QCloseEvent *event)
{
    Q_UNUSED(event);
    mRunning = false;
    stopQueue();
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


bool ObdScan::isError(std::string msg) {
    std::vector<std::string> errors(ERROR, ERROR + 18);
    for(unsigned int i=0; i < errors.size(); i++) {
        if(msg.find(errors[i]) != std::string::npos)
            return true;
    }
    return false;
}

QString ObdScan::getData(const QString &command)
{
    auto dataReceived =ConnectionManager::getInstance()->readData(command);

    dataReceived.remove("\r");
    dataReceived.remove(">");
    dataReceived.remove("?");
    dataReceived.remove(",");

    if(isError(dataReceived.toUpper().toStdString()))
    {
        return "error";
    }

    dataReceived = dataReceived.trimmed().simplified();
    dataReceived.remove(QRegExp("[\\n\\t\\r]"));
    dataReceived.remove(QRegExp("[^a-zA-Z0-9]+"));
    return dataReceived;
}

void ObdScan::timerEvent( QTimerEvent *event )
{
    Q_UNUSED(event)

    if(!ConnectionManager::getInstance()->isConnected())
        return;

    if(runtimeCommands.size() == 0)
        return;

    if(runtimeCommands.size() == commandOrder)
    {
        commandOrder = 0;
    }

    if(commandOrder < runtimeCommands.size())
    {
        auto dataReceived = getData(runtimeCommands[commandOrder]);
        if(dataReceived != "error")
            analysData(dataReceived);
        ui->labelCommand->setText(runtimeCommands[commandOrder]);
        commandOrder++;
    }
}


void ObdScan::dataReceived(QString dataReceived)
{
    if(!mRunning)return;

    if(runtimeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        send(runtimeCommands[commandOrder]);
        //ui->labelCommand->setText(runtimeCommands.join(", ") + "\n" + runtimeCommands[commandOrder]);
    }

    if(commandOrder < runtimeCommands.size())
    {
        send(runtimeCommands[commandOrder]);
        //ui->labelCommand->setText(runtimeCommands.join(", ") + "\n" + runtimeCommands[commandOrder]);
        commandOrder++;
    }

    try
    {
        dataReceived = dataReceived.trimmed().simplified();
        dataReceived.remove(QRegExp("[\\n\\t\\r]"));
        dataReceived.remove(QRegExp("[^a-zA-Z0-9]+"));
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
            value = A * 100 / 255;
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
            ui->labelMap->setText(QString::number(value, 'f', 0) + " kPa");
            break;
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            value = ((A * 256) + B) / 4;
            ui->labelRpm->setText(QString::number(value) + " rpm");
            break;
        case 13://PID(0D): KM Speed
            // A
            value = A;
            ui->labelSpeed->setText(QString::number(value) + " km/h");
            break;
        case 15://PID(0F): Intake Air Temperature
            // A - 40
            value = A - 40;
            break;
        case 16://PID(10): MAF air flow rate grams/sec
            // ((256*A)+B) / 100  [g/s]           
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
        case 51://PID(33) Absolute Barometric Pressure
            //A kPa
            value = A;
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
