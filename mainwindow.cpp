#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pid.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Elm327 Obd2");

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);

    if(osName() != "windows")
        setGeometry(desktopRect);

    m_networkManager = NetworkManager::getInstance();

    if(m_networkManager)
    {
        connect(m_networkManager, &NetworkManager::wifiConnected, this, &MainWindow::connected);
        connect(m_networkManager, &NetworkManager::wifiDisconnected, this, &MainWindow::disconnected);
        connect(m_networkManager, &NetworkManager::dataReceived, this, &MainWindow::dataReceived);
        connect(m_networkManager, &NetworkManager::dataHexReceived, this, &MainWindow::dataHexReceived);
        connect(m_networkManager, &NetworkManager::stateChanged, this, &MainWindow::stateChanged);
        connect(m_networkManager, &NetworkManager::errorAccrued, this, &MainWindow::errorAccrued);
    }

    if(osName() == "windows")
        ui->textTerminal->setStyleSheet("font: 10pt; color: #00cccc; background-color: #001a1a;");
    else
        ui->textTerminal->setStyleSheet("font: 12pt; color: #00cccc; background-color: #001a1a;");

    ui->pushConnect->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color:#074666;");
    ui->pushSend->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushClear->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushDiagnostic->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushScan->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushGauge->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushExit->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;background-color: #8F3A3A;");
    ui->labelIp->setStyleSheet("font-size: 16pt; font-weight: bold; color:#074666");
    ui->labelPort->setStyleSheet("font-size: 16pt; font-weight: bold; color:#074666;");
    ui->checkHex->setStyleSheet("font-size: 16pt; font-weight: bold; color:#074666;");
    ui->textSend->setStyleSheet("font-size: 16pt; font-weight: bold; color:black; background-color: #E7E0CD;");

#ifdef Q_OS_ANDROID
    ui->textSend->setMinimumHeight(100);
#else
    ui->textSend->setMaximumHeight(30);
#endif

    ui->textSend->setText(CHECK_DATA);
    ui->pushSend->setEnabled(false);
    //ui->pushScan->setEnabled(false);
    //ui->pushGauge->setEnabled(false);
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

#ifdef Q_OS_ANDROID
    //setScreenOrientation(SCREEN_ORIENTATION_PORTRAIT);
    keep_screen_on(true);
#endif
}

MainWindow::~MainWindow()
{
    delete m_networkManager;
    delete ui;
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

void MainWindow::send(QString &string)
{
    if(!m_ConsoleEnable)return;
    if(!m_networkManager->isConnected())return;

    m_networkManager->send(string);

    ui->textTerminal->append("-> " + string.trimmed()
                             .simplified()
                             .remove(QRegExp("[\\n\\t\\r]"))
                             .remove(QRegExp("[^a-zA-Z0-9]+")));
}

void MainWindow::stateChanged(QString &state)
{
    ui->textTerminal->append(state);
}

void MainWindow::dataHexReceived(QString &data)
{
    if(m_HexEnabled && !data.isEmpty())
        ui->textTerminal->append("<- " + data);
}

void MainWindow::errorAccrued(QString & error)
{
    ui->textTerminal->append("Error: " + error );
}

void MainWindow::connected()
{
    ui->pushSend->setEnabled(true);
    ui->pushScan->setEnabled(true);
    ui->pushGauge->setEnabled(true);
    ui->pushDiagnostic->setEnabled(true);

    ui->pushConnect->setText(QString("Disconnect"));

    commandOrder = 0;
    m_initialized = false;

    send(RESET); 
}

void MainWindow::disconnected()
{   
    ui->pushSend->setEnabled(false);
    ui->pushScan->setEnabled(false);
    ui->pushGauge->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);

    ui->textTerminal->clear();
    ui->textTerminal->append("Disconnected");
    ui->pushConnect->setText(QString("Connect"));

    commandOrder = 0;
    m_initialized = false;
}

void MainWindow::on_pushConnect_clicked()
{
    if(ui->pushConnect->text() == "Connect")
    {
        ui->textTerminal->clear();
        QString ip = ui->ipEdit->text();
        int port = ui->portEdit->text().toInt();
        m_networkManager->connectWifi(ip, port);
    }
    else
    {
        m_networkManager->disconnectWifi();
    }
}

void MainWindow::on_pushExit_clicked()
{
    if(m_networkManager->isConnected())
        m_networkManager->disconnectWifi();

    exit(0);
}

void MainWindow::on_pushSend_clicked()
{
    QString text = ui->textSend->toPlainText();
    send(text);
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
    qDebug() << "on_close_dialog_triggered";
}


void MainWindow::on_pushScan_clicked()
{
    ObdScan *obdScan = new ObdScan;
    obdScan->setGeometry(this->rect());
    obdScan->move(this->x(), this->y());
    QObject::connect(obdScan, &ObdScan::on_close_scan, this, &MainWindow::on_close_dialog_triggered);

    obdScan->show();
    m_ConsoleEnable = false;
}

void MainWindow::on_checkHex_stateChanged(int arg1)
{
    m_HexEnabled = arg1;
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

void MainWindow::analysData(const QString &dataReceived)
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
            value = A * 100 / 255;
            break;
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            break;
        case 10://PID(0A): Fuel Pressure
            // A * 3
            value = A * 3;
            break;
        case 11://PID(0B) Manifold Absolute Pressure
            // A
            value = A;
            break;
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            value = ((A * 256) + B) / 4;
            break;
        case 13://PID(0D): KM
            // A
            value = A;
            break;
        case 15://PID(0F): Intake Temperature
            // A - 40
            value = A - 40;
            break;
        case 16://PID(10): Maf
            // ((256*A)+B) / 100  [g/s]
            value = ((256 * A) + B) / 100;
            break;
        case 17://PID(11)
            //A*100/255
            value = A * 100 / 255;
            break;
        case 34://PID(22) The fuel guide rail is relative to the manifold vacuum pressureFuel
            // ((A*256)+B)*0.079
            value = ((A * 256) + B) * 0.079;
            break;
        case 35://PID(23) Fuel guide pressure
            // ((A*256)+B) * 10
            value = ((A*256)+B) * 10;
            break;
        case 49://PID(31)
            //(256*A)+B km
            value = (A * 256) + B;
            break;
        case 70://PID(46) Ambient Air Temperature
            // A-40 [DegC]
            value = A - 40;
            break;
        case 92://PID(05): Oil Temperature
            // A-40
            value = A - 40;
            break;
        default:
            //A
            value = A;
            break;
        }

        ui->textTerminal->append("Value: " + QString::number(value, 'f', 1).trimmed());
    }
    else
    {
        if (dataReceived.contains(QRegExp("\\s*[0-9]{1,2}([.][0-9]{1,2})?V\\s*")))
        {
            ui->textTerminal->append("Volt: " + dataReceived.mid(0,2) + "." + dataReceived.mid(2,1) + " V");
        }
    }
}

void MainWindow::dataReceived(QString &dataReceived)
{
    if(!m_ConsoleEnable)return;

    if(!m_HexEnabled)
    {
        if(dataReceived.isEmpty())
            ui->textTerminal->append("<- null");
        else
            ui->textTerminal->append("<- " + dataReceived);
    }

    if(dataReceived.toUpper().contains("SEARCHING"))return;

    if(!m_initialized && initializeCommands.size() == commandOrder)
    {
        m_initialized = true;
        commandOrder = 0;
        ui->textTerminal->append("<- initalized");
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

    if(m_clearCodeRequest)
    {
        ui->textTerminal->append("Clearing the trouble codes.");
        QString text(CLEAR_TROUBLE);
        send(text);
        m_clearCodeRequest = false;
    }
}
