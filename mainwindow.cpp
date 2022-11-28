#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->centralWidget()->setStyleSheet("background-color:#17202A ; border: none;");

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Elm327 Obd2");

    desktopRect = QApplication::desktop()->availableGeometry(this);

    if(osName() == "android" || osName() == "ios")
        setGeometry(desktopRect);

    ui->textTerminal->setStyleSheet("font: 16pt; color: #00cccc; background-color: #001a1a;");

//    QString protocols;

//    protocols.append("0 Automatic protocol detection\n");
//    protocols.append("1 SAE J1850 PWM (41.6 kbaud)\n");
//    protocols.append("2 SAE J1850 VPW (10.4 kbaud)\n");
//    protocols.append("3 ISO 9141-2 (5 baud init, 10.4 kbaud)\n");
//    protocols.append("4 ISO 14230-4 KWP (5 baud init, 10.4 kbaud)\n");
//    protocols.append("5 ISO 14230-4 KWP (fast init, 10.4 kbaud)\n");
//    protocols.append("6 ISO 15765-4 CAN (11 bit ID, 500 kbaud)\n");
//    protocols.append("7 ISO 15765-4 CAN (29 bit ID, 500 kbaud)\n");
//    protocols.append("8 ISO 15765-4 CAN (11 bit ID, 250 kbaud)\n");
//    protocols.append("9 ISO 15765-4 CAN (29 bit ID, 250 kbaud)\n");
//    protocols.append("A SAE J1939 CAN (29 bit ID, 250* kbaud)\n");
//    protocols.append("B User1 CAN (11* bit ID, 125* kbaud)\n");
//    protocols.append("C User2 CAN (11* bit ID, 50* kbaud)\n");


    ui->pushConnect->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color:#154360; padding: 12px; spacing: 12px;");
    ui->pushSend->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #154360; padding: 12px; spacing: 12px;");
    ui->pushSetProtocol->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #154360; padding: 12px; spacing: 12px;");
    ui->pushGetProtocol->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #154360; padding: 12px; spacing: 12px;");
    ui->pushClear->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #154360; padding: 12px; spacing: 12px");
    ui->pushReadFault->setStyleSheet("font-size: 24pt; font-weight: bold; color: white; background-color: #0B5345; padding: 12px; spacing: 12px");
    ui->pushClearFault->setStyleSheet("font-size: 24pt; font-weight: bold; color: white; background-color: #0B5345; padding: 12px; spacing: 12px");
    ui->pushScan->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #154360 ; padding: 12px; spacing: 12px");
    ui->pushExit->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #512E5F; padding: 12px; spacing: 12px");
    ui->checkSearchPids->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ECF0F1; background-color: orange ; padding: 6px; spacing: 6px;");

    ui->sendEdit->setStyleSheet("font-size: 22pt; font-weight: bold; color:white; padding: 12px; spacing: 12px");
    ui->protocolEdit->setStyleSheet("font-size: 22pt; font-weight: bold; color:white; padding: 12px; spacing: 12px");

    ui->sendEdit->setText("0101");
    ui->pushSend->setEnabled(false);
    ui->pushReadFault->setEnabled(false);
    ui->pushClearFault->setEnabled(false);
    ui->pushScan->setEnabled(false);
    ui->checkSearchPids->setEnabled(false);

    m_settingsManager = SettingsManager::getInstance();
    if(m_settingsManager)
    {
        saveSettings();
        ui->textTerminal->append("Wifi Ip: " + m_settingsManager->getWifiIp() + " : " + QString::number(m_settingsManager->getWifiPort()));
    }

    elm = new ELM();
    elm->resetPids();

    m_connectionManager = ConnectionManager::getInstance();
    if(m_connectionManager)
    {
        connect(m_connectionManager,&ConnectionManager::connected,this, &MainWindow::connected);
        connect(m_connectionManager,&ConnectionManager::disconnected,this,&MainWindow::disconnected);
        connect(m_connectionManager,&ConnectionManager::dataReceived,this,&MainWindow::dataReceived);       
        connect(m_connectionManager, &ConnectionManager::stateChanged, this, &MainWindow::stateChanged);

        m_connectionManager->setCType(ConnectionType::Wifi);
    }

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        screen->setOrientationUpdateMask(Qt::PortraitOrientation);

        QObject::connect(screen, &QScreen::orientationChanged, this, &MainWindow::orientationChanged);
    }

#if defined (Q_OS_ANDROID)
    //Request requiered permissions at runtime
    for(const QString &permission : permissions){
        auto result = QtAndroid::checkPermission(permission);

        if(result == QtAndroid::PermissionResult::Denied)
        {
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied)
                ui->textTerminal->append(permission + " denied!");
            else
                ui->textTerminal->append(permission + " granted!");
        }
        else if(result == QtAndroid::PermissionResult::Granted)
        {
            ui->textTerminal->append(permission + " granted!");
        }
    }
    keep_screen_on(true);
    //setScreenOrientation(SCREEN_ORIENTATION_LANDSCAPE);
#endif
    ui->textTerminal->append("Resolution : " + QString::number(desktopRect.width()) + "x" + QString::number(desktopRect.height()));
    ui->textTerminal->append("Press Connect Button");
    ui->pushConnect->setFocus();
}

MainWindow::~MainWindow()
{
    if(m_connectionManager)
    {
        m_connectionManager->disConnectElm();
        delete m_connectionManager;
    }

    if(m_settingsManager)
    {
        m_settingsManager->saveSettings();
        delete m_settingsManager;
    }

    if(elm)
        delete elm;

    delete ui;
}

void MainWindow::connectElm()
{
    if(m_connectionManager)
        m_connectionManager->connectElm();
}

void MainWindow::disConnectElm()
{    
    if(m_connectionManager)
        m_connectionManager->disConnectElm();
}

#ifdef Q_OS_ANDROID
bool MainWindow::setScreenOrientation(int orientation)
{
    QAndroidJniObject activity = QtAndroid::androidActivity();

    if(activity.isValid())
    {
        activity.callMethod<void>("setRequestedOrientation", "(I)V", orientation);
        return true;
    }
    return false;
}
#endif

void MainWindow::orientationChanged(Qt::ScreenOrientation orientation)
{
    /*switch (orientation) {
    case Qt::ScreenOrientation::PortraitOrientation:

        break;
    case Qt::ScreenOrientation::LandscapeOrientation:

        break;
    default:
        break;
    }*/
}

void MainWindow::stateChanged(QString state)
{
    ui->textTerminal->append(state);
}

void MainWindow::on_pushConnect_clicked()
{
    if(m_settingsManager)
    {
        saveSettings();
    }

    if(ui->pushConnect->text() == "Connect")
    {        
        QCoreApplication::processEvents();
        ui->textTerminal->clear();
        if(m_connectionManager)
            m_connectionManager->connectElm();
    }
    else
    {
        if(m_connectionManager)
            m_connectionManager->disConnectElm();
    }
}

void MainWindow::on_pushExit_clicked()
{
    QApplication::quit();
}

void MainWindow::on_pushSend_clicked()
{
    QString command = ui->sendEdit->text();
    send(command);
}

void MainWindow::on_pushClear_clicked()
{
    ui->textTerminal->clear();
}

void MainWindow::on_close_dialog_triggered()
{
    m_consoleEnable = true;
}

void MainWindow::on_pushScan_clicked()
{
    m_consoleEnable = false;
    ObdScan *obdScan = new ObdScan(this);
    obdScan->setGeometry(desktopRect);
    obdScan->move(this->x(), this->y());
    connect(obdScan, &ObdScan::on_close_scan, this, &MainWindow::on_close_dialog_triggered);

    obdScan->show();
}

void MainWindow::connected()
{
    ui->pushConnect->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color:#154360; padding: 24px; spacing: 24px;");

    ui->pushSend->setEnabled(true);
    ui->pushReadFault->setEnabled(true);
    ui->pushClearFault->setEnabled(true);
    ui->pushScan->setEnabled(true);
    ui->pushConnect->setText(QString("Disconnect"));

    commandOrder = 0;
    m_initialized = false;

    ui->textTerminal->append("Elm 327 connected");
    send(RESET);
}

void MainWindow::disconnected()
{
    ui->pushSend->setEnabled(false);
    ui->pushReadFault->setEnabled(false);
    ui->pushClearFault->setEnabled(false);
    ui->pushScan->setEnabled(false);
    ui->checkSearchPids->setEnabled(false);
    ui->pushConnect->setText(QString("Connect"));
    commandOrder = 0;
    m_initialized = false;
    ui->textTerminal->append("Elm DisConnected");

}

void MainWindow::analysData(const QString &dataReceived)
{
    unsigned A = 0;
    unsigned B = 0;
    unsigned PID = 0;

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

//        ui->textTerminal->append("Pid: " + QString::number(PID) + "  A: " + QString::number(A)+ "  B: " + QString::number(B));
    }

    //number of dtc & mil
    if(resp.size()>2 && !resp[0].compare("41",Qt::CaseInsensitive) && !resp[1].compare("01",Qt::CaseInsensitive))
    {
        vec.insert(vec.begin(),resp.begin()+2, resp.end());
        std::pair<int,bool> dtcNumber = elm->decodeNumberOfDtc(vec);
        QString milText = dtcNumber.second ? "true" : "false";
        ui->textTerminal->append("Number of Dtcs: " +  QString::number(dtcNumber.first) + ",  Mil on: " + milText);
    }
    //dtc codes
    if(resp.size()>1 && !resp[0].compare("43",Qt::CaseInsensitive))
    {
        //auto resp= elm->prepareResponseToDecode("486B104303000302030314486B10430304000000000D");
        vec.insert(vec.begin(),resp.begin()+1, resp.end());
        std::vector<QString> dtcCodes( elm->decodeDTC(vec));
        if(dtcCodes.size()>0)
        {
            QString dtc_list{"Dtcs: "};
            for(auto &code : dtcCodes)
            {
                dtc_list.append(code + " ");
            }
            ui->textTerminal->append(dtc_list);
        }
        else
            ui->textTerminal->append("Number of Dtcs: 0");
    }
}

void MainWindow::dataReceived(QString dataReceived)
{
//    if(!m_consoleEnable)
//        return;

    dataReceived.remove("\r");
    dataReceived.remove(">");
    dataReceived.remove("?");
    dataReceived.remove(",");

    if(isError(dataReceived.toUpper().toStdString()))
    {
        ui->textTerminal->append("Error : " + dataReceived);
    }
    else if (!dataReceived.isEmpty())
    {
        ui->textTerminal->append("<- " + dataReceived);
    }

    if(!m_initialized && initializeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        m_initialized = true;
        ui->checkSearchPids->setEnabled(true);

        if(m_searchPidsEnable)
        {
            getPids();
        }
    }
    else if(!m_initialized && commandOrder < initializeCommands.size())
    {
        send(initializeCommands[commandOrder]);
        commandOrder++;
    }

    if(m_initialized && !dataReceived.isEmpty())
    {

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
}

void MainWindow::getPids()
{
    runtimeCommands.clear();
    elm->resetPids();
    ui->textTerminal->append("-> Searching available pids.");
    QString supportedPIDs = elm->get_available_pids();
    ui->textTerminal->append("<- Pids:  " + supportedPIDs);

    if(!supportedPIDs.isEmpty())
    {
        if(supportedPIDs.contains(","))
        {
            runtimeCommands.append(supportedPIDs.split(","));
            QString str = runtimeCommands.join("");
            str = runtimeCommands.join(", ");
        }
    }
}

bool MainWindow::isError(std::string msg) {
    std::vector<std::string> errors(ERROR, ERROR + 18);
    for(unsigned int i=0; i < errors.size(); i++) {
        if(msg.find(errors[i]) != std::string::npos)
        return true;
    }
    return false;
}

QString MainWindow::send(const QString &command)
{
    if(m_connectionManager)
    {
        ui->textTerminal->append("-> " + command.trimmed()
                                 .simplified()
                                 .remove(QRegExp("[\\n\\t\\r]"))
                                 .remove(QRegExp("[^a-zA-Z0-9]+")));

        m_connectionManager->send(command);
        QThread::msleep(5);
    }

    return QString();
}

void MainWindow::saveSettings()
{
    QString ip = "192.168.0.10";
    // python3 -m elm -n 35000 -s car
    //QString ip = "0.0.0.0";
    quint16 wifiPort = 35000;
    m_settingsManager->setWifiIp(ip);
    m_settingsManager->setWifiPort(wifiPort);
    m_settingsManager->setSerialPort("/dev/ttys001");
    m_settingsManager->setEngineDisplacement(1600);
    m_settingsManager->saveSettings();
}

void MainWindow::on_pushReadFault_clicked()
{
    ui->textTerminal->append("-> Reading the trouble codes.");
    QThread::msleep(60);
    send(READ_TROUBLE);
}


void MainWindow::on_pushClearFault_clicked()
{
    ui->textTerminal->append("-> Clearing the trouble codes.");
    QThread::msleep(60);
    send(CLEAR_TROUBLE);
}


void MainWindow::on_checkSearchPids_toggled(bool checked)
{
    if(checked)
    {
        m_searchPidsEnable = true;
        getPids();
    }
    else
    {
        m_searchPidsEnable = false;
        runtimeCommands.clear();
    }
}


void MainWindow::on_pushSetProtocol_clicked()
{
    QString command = "ATTP" + ui->protocolEdit->text();
    send(command);
}


void MainWindow::on_pushGetProtocol_clicked()
{
    send(GET_PROTOCOL);
}

