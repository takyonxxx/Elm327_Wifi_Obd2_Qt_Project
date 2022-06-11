#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pid.h"

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

    ui->textTerminal->setStyleSheet("font: 12pt; color: #00cccc; background-color: #001a1a;");

    ui->pushConnect->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color:#154360; padding: 24px; spacing: 24px;");
    ui->pushSend->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #512E5F; padding: 24px; spacing: 24px;");
    ui->pushClear->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #512E5F; padding: 24px; spacing: 24px");
    ui->pushDiagnostic->setStyleSheet("font-size: 22pt; font-weight: bold; color: white; background-color: #0B5345; padding: 24px; spacing: 24px");
    ui->pushScan->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #512E5F ; padding: 24px; spacing: 24px");
    ui->pushGauge->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #512E5F ; padding: 24px; spacing: 24px");
    ui->pushExit->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #512E5F; padding: 24px; spacing: 24px");

    ui->sendEdit->setStyleSheet("font-size: 22pt; font-weight: bold; color:white; padding: 24px; spacing: 24px");

    ui->sendEdit->setText("0100");
    ui->pushSend->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);
    ui->pushScan->setEnabled(true);
    ui->pushGauge->setEnabled(true);
    /*auto diag_size = int(ui->pushGauge->height() + ui->pushScan->height());
    ui->pushDiagnostic->setMinimumHeight(2*diag_size);*/

    m_settingsManager = SettingsManager::getInstance();
    if(m_settingsManager)
    {
        saveSettings();
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
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                         Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation |
                                         Qt::InvertedPortraitOrientation);

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
    ui->textTerminal->append("Resolution : " + QString::number(desktopRect.width()) + "x" +QString::number(desktopRect.height()));
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
        ui->textTerminal->clear();      
        QCoreApplication::processEvents();

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

void MainWindow::on_pushDiagnostic_clicked()
{        
    ui->textTerminal->append("-> Reading the trouble codes.");
    m_requestClearDtc = true;
    send(READ_TROUBLE);
}

void MainWindow::on_close_dialog_triggered()
{
    m_consoleEnable = true;
}

void MainWindow::on_pushScan_clicked()
{
    /*if(runtimeCommands.size() == 0)
        return;*/

    m_consoleEnable = false;
    ObdScan *obdScan = new ObdScan(this);
    obdScan->setGeometry(desktopRect);
    obdScan->move(this->x(), this->y());
    connect(obdScan, &ObdScan::on_close_scan, this, &MainWindow::on_close_dialog_triggered);

    obdScan->show();
}


void MainWindow::on_pushGauge_clicked()
{   
    ObdGauge *obdGauge = new ObdGauge(this);
    obdGauge->setGeometry(desktopRect);
    obdGauge->move(this->x(), this->y());
    connect(obdGauge, &ObdGauge::on_close_gauge, this, &MainWindow::on_close_dialog_triggered);

    obdGauge->show();
    m_consoleEnable = false;
}

void MainWindow::connected()
{
    ui->pushConnect->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color:#154360; padding: 24px; spacing: 24px;");

    ui->pushSend->setEnabled(true);
    ui->pushDiagnostic->setEnabled(true);
    ui->pushConnect->setText(QString("Disconnect"));

    commandOrder = 0;
    m_initialized = false;

    ui->textTerminal->append("Elm 327 connected");
    //send(END_LINE);
    send(RESET);
}

void MainWindow::disconnected()
{
    ui->pushConnect->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color:#154360; padding: 24px; spacing: 24px;");

    ui->pushSend->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);
    ui->pushConnect->setText(QString("Connect"));

    commandOrder = 0;
    m_initialized = false;

    ui->textTerminal->append("Elm DisConnected");
    ui->pushConnect->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color:#154360; padding: 24px; spacing: 24px;");

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

        //ui->textTerminal->append("Pid: " + QString::number(PID) + "  A: " + QString::number(A)+ "  B: " + QString::number(B));
    }

    //number of dtc & mil
    if(resp.size()>2 && !resp[0].compare("41",Qt::CaseInsensitive) && !resp[1].compare("01",Qt::CaseInsensitive))
    {
        vec.insert(vec.begin(),resp.begin()+2, resp.end());
        std::pair<int,bool> dtcNumber = elm->decodeNumberOfDtc(vec);
        QString milText = dtcNumber.second ? "true" : "false";
        ui->textTerminal->append("Number of Dtcs: " +  QString::number(dtcNumber.first) + " Mil on: " + milText);
    }
    //dtc codes
    if(resp.size()>2 && !resp[0].compare("43",Qt::CaseInsensitive))
    {
        /*
        43 12 29 03 80 00 00
        12 29 03 80=>
        P1229
        P0	380
        */

        vec.insert(vec.begin(),resp.begin()+1, resp.begin()+7);
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

        if(m_requestClearDtc)
        {
             if(dtcCodes.size()>0)
                send(CLEAR_TROUBLE);
             else
                ui->textTerminal->append("There is no dtc.");

             m_requestClearDtc = false;
        }
    }
}

void MainWindow::dataReceived(QString dataReceived)
{
    if(!m_consoleEnable)
        return;

    if(dataReceived.isEmpty())
        return;

    if(!m_initialized && initializeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        m_initialized = true;

        /*elm->resetPids();
        ui->textTerminal->append("-> Searching available pids.");
        QString supportedPIDs = elm->get_available_pids();

        if(!supportedPIDs.isEmpty())
        {
            runtimeCommands.clear();
            runtimeCommands.append(VOLTAGE);
            if(supportedPIDs.contains(","))
                runtimeCommands.append(supportedPIDs.split(","));

            QString str = runtimeCommands.join("");
            str = runtimeCommands.join(", ");
            ui->textTerminal->append("<- Pids:  " + str);
        }*/
    }
    else if(!m_initialized && commandOrder < initializeCommands.size())
    {
        send(initializeCommands[commandOrder]);
        commandOrder++;
    }

    if(m_initialized && !dataReceived.isEmpty())
    {
        ui->textTerminal->append("<- " + dataReceived);

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
    }

    return QString();
}

void MainWindow::saveSettings()
{
    QString ip = "192.168.0.10";
    quint16 wifiPort = 35000;
    m_settingsManager->setWifiIp(ip);
    m_settingsManager->setWifiPort(wifiPort);
    m_settingsManager->setSerialPort("/dev/pts/8");  
    m_settingsManager->setEngineDisplacement(650);
    m_settingsManager->saveSettings();
}
