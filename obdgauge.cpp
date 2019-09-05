#include "obdgauge.h"
#include "ui_obdgauge.h"
#include "pid.h"

ObdGauge::ObdGauge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdGauge)
{
    ui->setupUi(this);

    setWindowTitle("Elm327 Obd2");

    this->centralWidget()->setStyleSheet("background-image: url(:/img/carbon-fiber.jpg); border: none;");

    pushSim = new QPushButton;
    pushExit = new QPushButton;
    pushSim->setText("Start Sim");
    pushExit->setText("Exit");
    pushExit->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #055580;");
    pushSim->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #055580;");

    connect(pushSim, &QPushButton::clicked, this, &ObdGauge::on_pushSim_clicked);
    connect(pushExit, &QPushButton::clicked, this, &ObdGauge::on_pushExit_clicked);
    pushExit->setMaximumHeight(100);

    initGauges();

    //ui->verticalLayout->addWidget(pushSim);

    m_networkManager = NetworkManager::getInstance();
    commandOrder = 0;

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        if(screen->orientation() == Qt::LandscapeOrientation)
        {
            ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
            ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 1);
            ui->gridLayout_Gauges->addWidget(pushExit, 1, 0, 1, 2);
        }
        else if(screen->orientation() == Qt::PortraitOrientation)
        {
            ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
            ui->gridLayout_Gauges->addWidget(mRpmGauge, 1, 0);
            ui->gridLayout_Gauges->addWidget(pushExit, 2, 0);
        }

        screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                         Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation |
                                         Qt::InvertedPortraitOrientation);

        QObject::connect(screen, &QScreen::orientationChanged, this, &ObdGauge::orientationChanged);
    }

    if(m_networkManager)
    {
        connect(m_networkManager, &NetworkManager::dataReceived, this, &ObdGauge::dataReceived);
        if(m_networkManager->isConnected())
        {
            mRunning = true;
            send(CHECK_DATA);
        }
    }
}

ObdGauge::~ObdGauge()
{
    delete ui;
}

void ObdGauge::initGauges()
{
    //speed
    mSpeedGauge = new QcGaugeWidget;
    mSpeedGauge->addBackground(99);
    QcBackgroundItem *bkgSpeed1 = mSpeedGauge->addBackground(92);
    bkgSpeed1->clearrColors();
    bkgSpeed1->addColor(0.1,Qt::black);
    bkgSpeed1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkgSpeed2 = mSpeedGauge->addBackground(88);
    bkgSpeed2->clearrColors();
    bkgSpeed2->addColor(0.1,Qt::black);
    bkgSpeed2->addColor(1.0,Qt::darkGray);

    mSpeedGauge->addArc(55);
    auto degrees = mSpeedGauge->addDegrees(65);
    degrees->setStep(20);
    degrees->setValueRange(0,220);
    auto colorBandSpeed = mSpeedGauge->addColorBand(50);

    QList<QPair<QColor, float> > colors;

    QColor tmpColor;
    tmpColor.setAlphaF(0.1);
    QPair<QColor,float> pair;

    pair.first = Qt::darkGreen;
    pair.second = 37;
    colors.append(pair);

    pair.first = Qt::yellow;
    pair.second = 55;
    colors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    colors.append(pair);

    colorBandSpeed->setColors(colors);

    auto values = mSpeedGauge->addValues(76);
    values->setStep(20);
    values->setValueRange(0,220);

    mSpeedGauge->addLabel(70)->setText("Km/h");
    QcLabelItem *labSpeed = mSpeedGauge->addLabel(40);
    labSpeed->setText("0");
    mSpeedNeedle = mSpeedGauge->addNeedle(60);
    mSpeedNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mSpeedNeedle->setLabel(labSpeed);
    mSpeedNeedle->setColor(Qt::white);
    mSpeedNeedle->setValueRange(0,220);
    mSpeedGauge->addBackground(7);
    mSpeedGauge->addGlass(88);

    //rpm
    mRpmGauge = new QcGaugeWidget;
    mRpmGauge->addBackground(99);
    QcBackgroundItem *bkgRpm1 = mRpmGauge->addBackground(92);
    bkgRpm1->clearrColors();
    bkgRpm1->addColor(0.1,Qt::black);
    bkgRpm1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkgRpm2 = mRpmGauge->addBackground(88);
    bkgRpm2->clearrColors();
    bkgRpm2->addColor(0.1,Qt::black);
    bkgRpm2->addColor(1.0,Qt::darkGray);

    mRpmGauge->addArc(55);
    mRpmGauge->addDegrees(65)->setValueRange(0,80);
    auto colorBandRpm = mRpmGauge->addColorBand(50);
    colors.clear();

    pair.first = Qt::darkGreen;
    pair.second = 44;
    colors.append(pair);

    pair.first = Qt::yellow;
    pair.second = 75;
    colors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    colors.append(pair);

    colorBandRpm->setColors(colors);

    mRpmGauge->addValues(76)->setValueRange(0,80);

    mRpmGauge->addLabel(70)->setText("RpmX100");
    QcLabelItem *labRpm = mRpmGauge->addLabel(40);
    labRpm->setText("0");
    mRpmNeedle = mRpmGauge->addNeedle(60);
    mRpmNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mRpmNeedle->setLabel(labRpm);
    mRpmNeedle->setColor(Qt::white);
    mRpmNeedle->setValueRange(0,80);
    mRpmGauge->addBackground(7);
    mRpmGauge->addGlass(88);

    /*engine = new QQmlApplicationEngine;
    engine->load(QUrl(QLatin1String("qrc:/GaugeScreen.qml")));
    QWindow *qmlWindow = qobject_cast<QWindow*>(engine->rootObjects().at(0));
    QWidget *container = QWidget::createWindowContainer(qmlWindow);
    ui->verticalLayout->addWidget(container);*/
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
    mSpeedNeedle->setCurrentValue(speed);
    /*QObject *rootObject = engine->rootObjects().first();
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setSpeed", Q_ARG(QVariant, speed));
    }*/
}

void ObdGauge::setRpm(int rpm)
{
    mRpmNeedle->setCurrentValue(rpm);
    /*QObject *rootObject = engine->rootObjects().first();
    if(rootObject != nullptr)
    {
        QMetaObject::invokeMethod(rootObject, "setRpm", Q_ARG(QVariant, rpm));
    }*/
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

void ObdGauge::orientationChanged(Qt::ScreenOrientation orientation)
{
    qDebug() << "Orientation:" << orientation;

    switch (orientation) {
    case Qt::ScreenOrientation::PortraitOrientation:
        ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 1, 0);
        ui->gridLayout_Gauges->addWidget(pushExit, 2, 0);
        break;
    case Qt::ScreenOrientation::LandscapeOrientation:
        ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 1);
        ui->gridLayout_Gauges->addWidget(pushExit, 1, 0, 1, 2);
        break;
    default:
        break;
    }
}
