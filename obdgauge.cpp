#include "obdgauge.h"
#include "ui_obdgauge.h"
#include "pid.h"

ObdGauge::ObdGauge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdGauge)
{
    ui->setupUi(this);

    setWindowTitle("Elm327 Obd2");

    engine = new QQmlApplicationEngine;
    engine->load(QUrl(QLatin1String("qrc:/GaugeScreen.qml")));
    QWindow *qmlWindow = qobject_cast<QWindow*>(engine->rootObjects().at(0));
    QWidget *container = QWidget::createWindowContainer(qmlWindow);

    pushSim = new QPushButton;
    pushExit = new QPushButton;
    pushSim->setText("Start Sim");
    pushExit->setText("Exit");
    pushExit->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #055580;");
    pushSim->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #055580;");

    connect(pushSim, &QPushButton::clicked, this, &ObdGauge::on_pushSim_clicked);
    connect(pushExit, &QPushButton::clicked, this, &ObdGauge::on_pushExit_clicked);

    ui->verticalLayout->addWidget(container);
    //ui->verticalLayout->addWidget(pushSim);
    ui->verticalLayout->addWidget(pushExit);

    m_networkManager = NetworkManager::getInstance();
    commandOrder = 0;

    if(m_networkManager)
    {
        connect(m_networkManager, &NetworkManager::dataReceived, this, &ObdGauge::dataReceived);
        if(m_networkManager->isConnected())
        {
            mRunning = true;
            send(gaugeCommands[commandOrder]);
            commandOrder++;
        }
    }
}

ObdGauge::~ObdGauge()
{
    delete ui;
}

void ObdGauge::startSim()
{
    m_realTime = 0;
    m_timerId  = startTimer(0);
    m_time.start();
}

void ObdGauge::stopSim()
{
    if ( m_timerId ) killTimer( m_timerId );
}

void ObdGauge::setSpeed(int speed)
{
    QObject *rootObject = engine->rootObjects().first();
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setSpeed", Q_ARG(QVariant, speed));
    }
}

void ObdGauge::setRpm(int rpm)
{
    QObject *rootObject = engine->rootObjects().first();
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setRpm", Q_ARG(QVariant, rpm));
    }
}

void ObdGauge::timerEvent( QTimerEvent *event )
{
    Q_UNUSED(event)

    auto timeStep = m_time.restart();
    m_realTime = m_realTime + timeStep / 1000.0f;
    valueGauge  =  121.0f * std::sin( m_realTime /  5.0f ) +  121.0f;
    setSpeed(static_cast<int>(valueGauge));
    setRpm(static_cast<int>(valueGauge/3));
}


void ObdGauge::send(QString &data)
{
    if(!mRunning)return;
    if(!m_networkManager->isConnected())return;
    m_networkManager->send(data);
}

void ObdGauge::analysData(const QString &dataReceived)
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
            setRpm(static_cast<int>(value / 100));
            break;
        case 4://PID(04): Engine Load
            // A*100/255
            value = A * 100 / 255;
            break;
        case 13://PID(0D): KM Speed
            // A
            value = A;
            setSpeed(static_cast<int>(value));
            break;
        default:
            //A
            value = A;
            break;
        }
    }
}

void ObdGauge::dataReceived(QString &dataReceived)
{
    if(!mRunning)return;

    if(commandOrder < gaugeCommands.size())
    {
        send(gaugeCommands[commandOrder]);
        commandOrder++;
    }

    if(gaugeCommands.size() == commandOrder)
    {
        commandOrder = 0;
        send(gaugeCommands[commandOrder]);
    }

    analysData(dataReceived);
}

void ObdGauge::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    mRunning = false;
    emit on_close_gauge();
}

void ObdGauge::on_pushExit_clicked()
{
    mRunning = false;
    close();
}

void ObdGauge::on_pushSim_clicked()
{
    if(pushSim->text() == "Start Sim")
    {
        startSim();
        pushSim->setText("Stop Sim");
    }
    else
    {
        stopSim();
        setSpeed(static_cast<int>(0));
        setRpm(static_cast<int>(0));
        pushSim->setText("Start Sim");
    }
}
