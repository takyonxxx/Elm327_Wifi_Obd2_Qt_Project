#include "obdgauge.h"
#include "ui_obdgauge.h"

ObdGauge::ObdGauge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdGauge)
{
    ui->setupUi(this);

    setWindowTitle("Elm327 Obd2");

    this->centralWidget()->setStyleSheet("background-image: url(:/img/carbon-fiber.png); border: none;");

    pushExit = new QPushButton("Exit");
    pushExit->setStyleSheet("font-size: 24pt; font-weight: bold; color: white; background-color: rgba(0, 0, 255, 128);");

    connect(pushExit, &QPushButton::clicked, this, &ObdGauge::on_pushExit_clicked);

    initGauges();


    foreach (QScreen *screen, QGuiApplication::screens())
    {
        if (screen->orientation() == Qt::LandscapeOrientation)
        {
            ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
            ui->gridLayout_Gauges->addWidget(mCoolentGauge, 0, 1);
            ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 2);
            ui->gridLayout_Gauges->addWidget(pushExit, 1, 0, 1, 3);

            ui->gridLayout_Gauges->setColumnStretch(0, 2);
            ui->gridLayout_Gauges->setColumnStretch(1, 1.5f);
            ui->gridLayout_Gauges->setColumnStretch(2, 2);
        }
        else if (screen->orientation() == Qt::PortraitOrientation)
        {
            ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
            ui->gridLayout_Gauges->addWidget(mCoolentGauge, 1, 0);
            ui->gridLayout_Gauges->addWidget(mRpmGauge, 2, 0);
            ui->gridLayout_Gauges->addWidget(pushExit, 3, 0);
        }

        screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                         Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation |
                                         Qt::InvertedPortraitOrientation);

        QObject::connect(screen, &QScreen::orientationChanged, this, &ObdGauge::orientationChanged);
    }


    if(runtimeCommands.isEmpty())
    {
        runtimeCommands.append(VEHICLE_SPEED);
        runtimeCommands.append(ENGINE_RPM);
        runtimeCommands.append(COOLANT_TEMP);
    }

    if(ConnectionManager::getInstance() && ConnectionManager::getInstance()->isConnected())
    {
        QObject::connect(ConnectionManager::getInstance(),&ConnectionManager::dataReceived,this, &ObdGauge::dataReceived);
        mRunning = true;
        send(VOLTAGE);
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

    auto values = mSpeedGauge->addValues(74);
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

    mRpmGauge->addValues(74)->setValueRange(0,80);

    mRpmGauge->addLabel(70)->setText("X100");
    QcLabelItem *labRpm = mRpmGauge->addLabel(40);
    labRpm->setText("0");
    mRpmNeedle = mRpmGauge->addNeedle(60);
    mRpmNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mRpmNeedle->setLabel(labRpm);
    mRpmNeedle->setColor(Qt::white);
    mRpmNeedle->setValueRange(0,80);
    mRpmGauge->addBackground(7);
    mRpmGauge->addGlass(88);

    //coolent
    mCoolentGauge = new QcGaugeWidget;
    mCoolentGauge->addBackground(99);
    QcBackgroundItem *bkgCoolent1 = mCoolentGauge->addBackground(92);
    bkgCoolent1->clearrColors();
    bkgCoolent1->addColor(0.1,Qt::black);
    bkgCoolent1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkgCoolent2 = mCoolentGauge->addBackground(88);
    bkgCoolent2->clearrColors();
    bkgCoolent2->addColor(0.1,Qt::black);
    bkgCoolent2->addColor(1.0,Qt::darkGray);

    mCoolentGauge->addArc(55);

    QcDegreesItem *deg = mCoolentGauge->addDegrees(65);
    deg->setStep(10);
    deg->setValueRange(40,120);
    auto colorBandCoolent = mCoolentGauge->addColorBand(50);
    colors.clear();

    pair.first = Qt::yellow;
    pair.second = 31;
    colors.append(pair);

    pair.first = Qt::darkGreen;
    pair.second = 69;
    colors.append(pair);

    pair.first = Qt::yellow;
    pair.second = 75;
    colors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    colors.append(pair);
    colorBandCoolent->setColors(colors);

    QcValuesItem *_values = mCoolentGauge->addValues(74);
    _values->setStep(10);
    _values->setValueRange(40,120);

    mCoolentGauge->addLabel(70)->setText("C°");
    QcLabelItem *labCoolent = mCoolentGauge->addLabel(40);
    labCoolent->setColor(Qt::white);
    labCoolent->setText("0");
    mCoolentNeedle = mCoolentGauge->addNeedle(60);
    mCoolentNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mCoolentNeedle->setLabel(labCoolent);
    mCoolentNeedle->setColor(Qt::white);
    mCoolentNeedle->setValueRange(40,120);
    mCoolentGauge->addBackground(7);
    mCoolentGauge->addGlass(88);

    //startSim();
    setCoolent(static_cast<int>(40));

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
}

void ObdGauge::setRpm(int rpm)
{
    mRpmNeedle->setCurrentValue(rpm);
}

void ObdGauge::setCoolent(int degree)
{
    mCoolentNeedle->setCurrentValue(degree);
}

void ObdGauge::timerEvent( QTimerEvent *event )
{
    Q_UNUSED(event)

    auto timeStep = m_time.restart();
    m_realTime = m_realTime + timeStep / 100.0f;
    valueGauge  =  111.0f * std::sin( m_realTime /  5.0f ) +  111.0f;
    setSpeed(static_cast<int>(valueGauge));
    setRpm(static_cast<int>(valueGauge/2.75));
}


QString ObdGauge::send(const QString &command)
{
    if(mRunning && ConnectionManager::getInstance())
    {
        ConnectionManager::getInstance()->send(command);
    }

    return QString();
}

void ObdGauge::analysData(const QString &dataReceived)
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
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            value = ((A * 256) + B) / 4;
            setRpm(static_cast<int>(value / 100));
            break;
        case 13://PID(0D): KM Speed
            // A
            value = A;
            setSpeed(static_cast<int>(value));
            break;
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            setCoolent(value);
            break;
        default:
            //A
            value = A;
            break;
        }
    }
}

void ObdGauge::dataReceived(QString dataReceived)
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

void ObdGauge::orientationChanged(Qt::ScreenOrientation orientation)
{  
    switch (orientation) {
    case Qt::ScreenOrientation::PortraitOrientation:
        ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(mCoolentGauge, 1, 0);
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 2, 0);
        ui->gridLayout_Gauges->addWidget(pushExit, 3, 0);
        break;
    case Qt::ScreenOrientation::LandscapeOrientation:
        ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(mCoolentGauge, 0, 1);
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 2);
        ui->gridLayout_Gauges->addWidget(pushExit, 1, 0, 1, 3);
        break;
    default:
        break;
    }    
}
