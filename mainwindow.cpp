#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pid.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Elm 327 Obd");

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    //setGeometry(desktopRect);

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

    ui->textTerminal->setStyleSheet("font: 12pt; color: #00cccc; background-color: #001a1a;");
    ui->pushConnect->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color:#074666;");
    ui->pushSend->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushClear->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushDiagnostic->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushScan->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushGauge->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #074666;");
    ui->pushExit->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #8F3A3A;");
    ui->labelIp->setStyleSheet("font-size: 12pt; font-weight: bold; color:#074666");
    ui->labelPort->setStyleSheet("font-size: 12pt; font-weight: bold; color:#074666;");
    ui->checkHex->setStyleSheet("font-size: 12pt; font-weight: bold; color:#074666;");
    ui->textSend->setText("ATI");
    ui->pushSend->setEnabled(false);
    ui->pushScan->setEnabled(false);
    ui->pushGauge->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);

    refreshTimer = new QTimer();
    refreshTimer->setInterval(1000);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::refreshObd);
}

MainWindow::~MainWindow()
{
    delete refreshTimer;
    delete m_networkManager;
    delete ui;
}

void MainWindow::refreshObd()
{
    send(ENGINE_RPM);
}

/*void MainWindow::findActiveWirelesses()
{

    QNetworkConfigurationManager ncm;
    netcfgList = ncm.allConfigurations();
    for (auto &x : netcfgList)
    {
        if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
        {
            if(!x.name().isEmpty())
            {
                if (x.name() == "Wi-Fi") {
                    netcfg = x;
                    auto session = new QNetworkSession(netcfg, this);
                    session->open();

                    if (session->isOpen())
                        qDebug() << "Wi-Fi Open Success on " + session->configuration().name() + ".\n";
                    else
                        qDebug() << "Wi-Fi Open Failure on " + session->configuration().name() + ".\n";
                }
            }
        }
    }
}*/

void MainWindow::send(QString &string)
{
    if(!m_networkManager->isConnected())return;

    m_networkManager->send(string);

    if(m_ConsoleEnable)
        ui->textTerminal->append("-> " + string.trimmed()
                                 .simplified()
                                 .remove(QRegExp("[\\n\\t\\r]"))
                                 .remove(QRegExp("[^a-zA-Z0-9]+")));
}

void MainWindow::dataReceived(QString &data)
{
    if(!m_ConsoleEnable)return;

    if(!m_HexEnabled && !data.isEmpty())
        ui->textTerminal->append("<- " + data);

    if(!m_initialized && commandOrder < initializeCommands.size())
    {
        send(initializeCommands[commandOrder]);
        commandOrder++;
    }

    if(!m_initialized && initializeCommands.size() == commandOrder)
    {
        m_initialized = true;
        commandOrder = 0;
    }

    if(m_initialized && !data.isEmpty())
    {
        analysData(data);
    }

    if(m_clearCodeRequest)
    {
        QString text("0104");
        send(text);
        m_clearCodeRequest = false;
    }
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

    ui->textTerminal->append("Connected to ELM327");
    ui->pushConnect->setText(QString("Disconnect"));

    send(initializeCommands[commandOrder]);
    commandOrder++;

    refreshTimer->start();
}

void MainWindow::disconnected()
{
    refreshTimer->stop();

    ui->pushSend->setEnabled(false);
    ui->pushScan->setEnabled(false);
    ui->pushGauge->setEnabled(false);
    ui->pushDiagnostic->setEnabled(false);

    ui->textTerminal->append("Disconnected from ELM327");
    ui->pushConnect->setText(QString("Connect"));
    commandOrder = 0;    
}

void MainWindow::on_pushConnect_clicked()
{
    if(ui->pushConnect->text() == "Connect")
    {
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
    QString text("0103");
    send(text);
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

void MainWindow::on_close_dialog_triggered()
{
    m_ConsoleEnable = true;
    qDebug() << "on_close_dialog_triggered";
}


void MainWindow::on_pushScan_clicked()
{
    ObdScan *obdScan = new ObdScan;
    obdScan->setWindowTitle("Obd Scan");
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
    obdGauge->setWindowTitle("Obd Scan");
    obdGauge->setGeometry(this->rect());
    obdGauge->move(this->x(), this->y());
    QObject::connect(obdGauge, &ObdGauge::on_close_gauge, this, &MainWindow::on_close_dialog_triggered);

    obdGauge->show();
    m_ConsoleEnable = false;
}
