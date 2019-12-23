#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pid.h"
#include "methods.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Elm327 Obd2");

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);

    if(osName() == "android" || osName() == "ios")
        setGeometry(desktopRect);

    if(osName() == "windows")
        ui->textTerminal->setStyleSheet("font: 10pt; color: #00cccc; background-color: #001a1a;");
    else
        ui->textTerminal->setStyleSheet("font: 12pt; color: #00cccc; background-color: #001a1a;");

    ui->pushConnect->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color:#154360; padding: 2px;");
    ui->pushSend->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #154360; padding: 2px;");
    ui->pushClear->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #512E5F; padding: 2px;");
    ui->pushDiagnostic->setStyleSheet("font-size: 42pt; font-weight: bold; color: white; background-color: #0B5345; padding: 6px;");
    ui->pushScan->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #512E5F ; padding: 2px;");
    ui->pushGauge->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #512E5F ; padding: 2px;");
    ui->pushExit->setStyleSheet("font-size: 24pt; font-weight: bold; color: white;background-color: #8F3A3A; padding: 2px;");
    ui->labelIp->setStyleSheet("font-size: 18pt; font-weight: bold; color:#074666; padding: 2px;");
    ui->ipEdit->setStyleSheet("font-size: 18pt; font-weight: bold; color:#074666; padding: 2px;");
    ui->labelPort->setStyleSheet("font-size: 18pt; font-weight: bold; color:#074666; padding: 2px;");
    ui->portEdit->setStyleSheet("font-size: 18pt; font-weight: bold; color:#074666; padding: 2px;");
    ui->textSend->setStyleSheet("font-size: 18pt; font-weight: bold; color:black; background-color: #E7E0CD; padding: 2px;");
    ui->radioBle->setStyleSheet("font-size: 16pt; font-weight: bold; color:darkblue; padding: 2px;");
    ui->radioWifi->setStyleSheet("font-size: 16pt; font-weight: bold; color:darkblue; padding: 2px;");
    ui->comboBleList->setStyleSheet("font-size: 14pt; font-weight: bold; color:black; padding: 2px;");

#ifdef Q_OS_ANDROID
    ui->textSend->setMinimumHeight(100);
#else
    ui->textSend->setMaximumHeight(30);
#endif

    ui->textSend->setText(PIDS_SUPPORTED20);
    ui->pushSend->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);
    ui->textTerminal->append("Plug ELM327 WIFI Scanner into vehicle's OBD2 port.");
    ui->textTerminal->append("Turn ON ignition. (This is one step before engine is powered.)");
    ui->textTerminal->append("On your device : go to Settings > Wi-Fi. ");
    ui->textTerminal->append("Connect to the Wi-Fi signal with name similar to these examples:");
    ui->textTerminal->append("WIFI ELM327, WiFiOBD, OBDDevice, V-Link.");
    ui->textTerminal->append("Press Connect Button");

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                         Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation |
                                         Qt::InvertedPortraitOrientation);

        QObject::connect(screen, &QScreen::orientationChanged, this, &MainWindow::orientationChanged);
    }

    m_settingsManager = SettingsManager::getInstance();
    if(m_settingsManager)
    {
        saveSettings();
    }

    elm = new ELM();
    ui->radioWifi->setChecked(true);
    ui->pushConnect->setFocus();

    m_connectionManager = new ConnectionManager(this);
    if(m_connectionManager)
    {
        connect(m_connectionManager,&ConnectionManager::connected,this, &MainWindow::connected);
        connect(m_connectionManager,&ConnectionManager::disconnected,this,&MainWindow::disconnected);
        connect(m_connectionManager,&ConnectionManager::dataReceived,this,&MainWindow::dataReceived);
        connect(m_connectionManager, &ConnectionManager::addBleDevice, this, &MainWindow::addBleDeviceToList);
        connect(m_connectionManager, &ConnectionManager::stateChanged, this, &MainWindow::stateChanged);
        m_connectionManager->setCType(ConnectionType::Wifi);
    }


#ifdef Q_OS_ANDROID
    //setScreenOrientation(SCREEN_ORIENTATION_PORTRAIT);
    keep_screen_on(true);
#endif

}

MainWindow::~MainWindow()
{
    if(m_connectionManager)
        delete m_connectionManager;
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
    qDebug() << "Orientation:" << orientation;

    switch (orientation) {
    case Qt::ScreenOrientation::PortraitOrientation:

        break;
    case Qt::ScreenOrientation::LandscapeOrientation:

        break;
    default:
        break;
    }
}

void MainWindow::stateChanged(QString &state)
{
    ui->textTerminal->append(state);
}


void MainWindow::errorAccrued(QString & error)
{
    ui->textTerminal->append("Error: " + error );
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
    if(m_connectionManager)
        m_connectionManager->disConnectElm();

    exit(0);
}

void MainWindow::on_pushSend_clicked()
{
    QString command = ui->textSend->toPlainText();
    ui->textTerminal->append("-> " + command.trimmed()
                             .simplified()
                             .remove(QRegExp("[\\n\\t\\r]"))
                             .remove(QRegExp("[^a-zA-Z0-9]+")));


    m_connectionManager->readData(command);
}

void MainWindow::on_pushClear_clicked()
{
    ui->textTerminal->clear();
}

void MainWindow::on_pushDiagnostic_clicked()
{
    ui->textTerminal->clear();
    //0x03 : show stored diagnostic trouble code.
    //0x04 : clear diagnostic trouble code.
    m_clearCodeRequest = true;
    ui->textTerminal->append("The trouble codes requested.");
    QString text(REQUEST_TROUBLE);
    send(text);
}

void MainWindow::on_close_dialog_triggered()
{
    m_ConsoleEnable = true;
}

void MainWindow::on_pushScan_clicked()
{
    m_ConsoleEnable = false;
    ObdScan *obdScan = new ObdScan;
    obdScan->setGeometry(this->rect());
    obdScan->move(this->x(), this->y());
    QObject::connect(obdScan, &ObdScan::on_close_scan, this, &MainWindow::on_close_dialog_triggered);

    obdScan->show();
    m_ConsoleEnable = false;
}


void MainWindow::on_pushGauge_clicked()
{
    ObdGauge *obdGauge = new ObdGauge;
    obdGauge->setGeometry(this->rect());
    obdGauge->move(this->x(), this->y());
    QObject::connect(obdGauge, &ObdGauge::on_close_gauge, this, &MainWindow::on_close_dialog_triggered);

    obdGauge->show();
    m_ConsoleEnable = false;
}

void MainWindow::connected()
{
    ui->pushSend->setEnabled(true);
    ui->pushDiagnostic->setEnabled(true);

    ui->pushConnect->setText(QString("Disconnect"));

    commandOrder = 0;
    m_initialized = false;
    send(RESET);
}

void MainWindow::disconnected()
{
    ui->pushSend->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);

    ui->textTerminal->clear();
    ui->pushConnect->setText(QString("Connect"));

    commandOrder = 0;
    m_initialized = false;
}

void MainWindow::analysData(const QString &dataReceived)
{

    unsigned A = 0;
    unsigned B = 0;
    unsigned PID = 0;

    if(dataReceived.toUpper().startsWith("UNABLETOCONNECT"))
        return;

    std::vector<QString> vec;
    auto resp= elm->prepareResponseToDecode(dataReceived);

    if(resp.size()>0 && !resp[0].compare("41",Qt::CaseInsensitive))
    {
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

        ui->textTerminal->append("Pid: " + QString::number(PID) + "  A: " + QString::number(A)+ "  B: " + QString::number(B));
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
        vec.insert(vec.begin(),resp.begin()+1, resp.end());
        std::vector<QString> dtcCodes( elm->decodeDTC(vec));
        if(dtcCodes.size()>0)
        {
            for(auto &code : dtcCodes)
            {
                ui->textTerminal->append(code);
            }
        }
    }

    if(m_clearCodeRequest)
    {
        ui->textTerminal->append("Clearing the trouble codes.");
        QString text(CLEAR_TROUBLE);
        send(text);
        m_clearCodeRequest = false;
    }
}

void MainWindow::dataReceived(QString &dataReceived)
{
    if(!m_ConsoleEnable)return;
    if(dataReceived.isEmpty())return;

    ui->textTerminal->append("<- " + dataReceived);

    if(!m_initialized && initializeCommands.size() == commandOrder)
    {
        m_initialized = true;
        commandOrder = 0;
    }

    if(!m_initialized && commandOrder < initializeCommands.size())
    {
        send(initializeCommands[commandOrder]);
        commandOrder++;
    }

    if(m_initialized && !dataReceived.isEmpty())
    {
        analysData(dataReceived);
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

void MainWindow::on_radioBle_clicked(bool checked)
{
    if(checked && m_connectionManager)
    {
        ui->textTerminal->clear();
        ui->textTerminal->append("Ready for elm327 bluetooth devices..");

        m_connectionManager->setCType(ConnectionType::BlueTooth);
    }
}

void MainWindow::on_radioWifi_clicked(bool checked)
{
    if(checked && m_connectionManager)
    {
        ui->textTerminal->clear();
        ui->textTerminal->append("Ready for elm327 wifi devices..");
        ui->comboBleList->clear();

        m_connectionManager->setCType(ConnectionType::Wifi);
    }
}

void MainWindow::saveSettings()
{
    QString ip = ui->ipEdit->text();
    quint16 port = ui->portEdit->text().toUShort();
    m_settingsManager->setIp(ip);
    m_settingsManager->setPort(port);

    auto text = ui->comboBleList->currentText();
    if(text.isEmpty())
        return;

    auto strAddress = text.split(" ").at(0);
    m_settingsManager->setBleAddress(QBluetoothAddress(strAddress));

    m_settingsManager->saveSettings();
}

void MainWindow::addBleDeviceToList(const QBluetoothAddress & bleAddress, const QString & bleName)
{
    QString item = bleAddress.toString() + QString(" ") + bleName;
    ui->comboBleList->addItem(item);
}

