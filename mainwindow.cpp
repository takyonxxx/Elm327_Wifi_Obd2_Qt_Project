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
    ui->checkHex->setStyleSheet("font-size: 18pt; font-weight: bold; color:#074666; padding: 2px;");
    ui->textSend->setStyleSheet("font-size: 18pt; font-weight: bold; color:black; background-color: #E7E0CD; padding: 2px;");

#ifdef Q_OS_ANDROID
    ui->textSend->setMinimumHeight(100);
#else
    ui->textSend->setMaximumHeight(30);
#endif

    ui->textSend->setText(PIDS_SUPPORTED20);
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

    runtimeCommands.clear();
    pidsSupportedCommands.clear();

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                         Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation |
                                         Qt::InvertedPortraitOrientation);

        QObject::connect(screen, &QScreen::orientationChanged, this, &MainWindow::orientationChanged);
    }

    ui->pushConnect->setFocus();

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

void MainWindow::appendPidsSupportedCommand(const QString &dataReceived)
{
    if (dataReceived.startsWith("4100")
            || dataReceived.startsWith("4120")
            || dataReceived.startsWith("4140")
            || dataReceived.startsWith("4160")
            || dataReceived.startsWith("4180")
            || dataReceived.startsWith("41A0"))
    {
        QString tmp = dataReceived;
        auto cmd = tmp.remove(0, 4);
        pidsSupportedCommands.append(cmd);
    }
}

void MainWindow::getPidsSupported()
{
    ELM elm{};
    QString supportedPIDs = elm.get_available_pids();
    ui->textTerminal->append("PIDs supported: " + supportedPIDs);
}

void MainWindow::analysData(const QString &dataReceived)
{
    if(dataReceived.isEmpty())return;

    unsigned A = 0;
    unsigned B = 0;
    unsigned PID = 0;
    bool valid;

    QString tmpmsg{};

    if(dataReceived.startsWith(QString("41")))
    {
        tmpmsg = dataReceived.mid(0, dataReceived.length());
        PID = tmpmsg.mid(2,2).toUInt(&valid,16);
        A = tmpmsg.mid(4,2).toUInt(&valid,16);
        B = tmpmsg.mid(6,2).toUInt(&valid,16);

        ui->textTerminal->append("Value of A: " + QString::number(A)+ " B: " + QString::number(B));
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

    /*if(!dataReceived.isEmpty())
        ui->textTerminal->append("<- " + dataReceived);*/

    if(dataReceived.toUpper().contains("SEARCHING"))
        return;

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

    if(m_initialized)
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
