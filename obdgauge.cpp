#include "obdgauge.h"
#include "ui_obdgauge.h"

ObdGauge::ObdGauge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdGauge)
{
    ui->setupUi(this);

    setWindowTitle("Elm327 Obd2");

    this->centralWidget()->setStyleSheet("background-image: url(:/img/carbon-fiber.png); border: none;");
//    labelCommand = new QLabel(this);
//    labelCommand->setStyleSheet("font-size: 32pt; font-weight: bold; color: yellow; background-color: #154360 ; padding: 6px; spacing: 6px;");
//    labelCommand->setAlignment(Qt::AlignCenter);

    initGauges();

    foreach (QScreen *screen, QGuiApplication::screens())
    {
        if (screen->orientation() == Qt::LandscapeOrientation)
        {
            ui->gridLayout_Gauges->addWidget(mBoostGauge, 0, 0);
            //ui->gridLayout_Gauges->addWidget(labelCommand, 0, 1);
            ui->gridLayout_Gauges->addWidget(mCoolentGauge, 0, 2);

            //labelCommand->setFixedWidth(100);

//            ui->gridLayout_Gauges->setColumnStretch(0, 1);
//            ui->gridLayout_Gauges->setColumnStretch(1, 0);
//            ui->gridLayout_Gauges->setColumnStretch(2, 1);
        }
        else if (screen->orientation() == Qt::PortraitOrientation)
        {
            ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
            ui->gridLayout_Gauges->addWidget(mBoostGauge, 1, 0);
            //ui->gridLayout_Gauges->addWidget(labelCommand, 2, 0);
        }

        screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                         Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation |
                                         Qt::InvertedPortraitOrientation);

        QObject::connect(screen, &QScreen::orientationChanged, this, &ObdGauge::orientationChanged);
    }

    runtimeCommands.clear();

    if(runtimeCommands.isEmpty())
    {
        runtimeCommands.append(COOLANT_TEMP);
        runtimeCommands.append(MAN_ABSOLUTE_PRESSURE);
        //runtimeCommands.append(BAROMETRIC_PRESSURE);
    }

    m_gps = new Gps(this);

    startQueue();

//    if(ConnectionManager::getInstance() && ConnectionManager::getInstance()->isConnected())
//    {
//        QObject::connect(ConnectionManager::getInstance(),&ConnectionManager::dataReceived,this, &ObdGauge::dataReceived);
//        mRunning = true;
//        send(VOLTAGE);
//    }
}

ObdGauge::~ObdGauge()
{
    stopQueue();
    delete ui;
}

void ObdGauge::startQueue()
{
    m_realTime = 0;
    m_timerId  = startTimer(interval);
    m_time.start();
}

void ObdGauge::stopQueue()
{
    if ( m_timerId ) killTimer( m_timerId );
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
    auto speed_degrees = mSpeedGauge->addDegrees(65);
    speed_degrees->setStep(20);
    speed_degrees->setValueRange(0,220);

    auto speed_ColorBand = mSpeedGauge->addColorBand(50);
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
    speed_ColorBand->setColors(colors);

    auto speed_values = mSpeedGauge->addValues(74);
    speed_values->setStep(20);
    speed_values->setValueRange(0,220);

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
    auto rpm_ColorBand= mRpmGauge->addColorBand(50);
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
    rpm_ColorBand->setColors(colors);

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
    auto coolent_ColorBandCoolent = mCoolentGauge->addColorBand(50);
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
    coolent_ColorBandCoolent->setColors(colors);

    QcValuesItem *coolent_values = mCoolentGauge->addValues(74);
    coolent_values->setStep(10);
    coolent_values->setValueRange(40,120);

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


    //turbo boost
    mBoostGauge = new QcGaugeWidget;
    mBoostGauge->addBackground(99);
    QcBackgroundItem *bkgBoost1 = mBoostGauge->addBackground(92);
    bkgBoost1->clearrColors();
    bkgBoost1->addColor(0.1,Qt::black);
    bkgBoost1->addColor(1.0,Qt::white);

    QcBackgroundItem *bkgBoost2 = mBoostGauge->addBackground(88);
    bkgBoost2->clearrColors();
    bkgBoost2->addColor(0.1,Qt::black);
    bkgBoost2->addColor(1.0,Qt::darkGray);

    mBoostGauge->addArc(55);

    QcDegreesItem *boost_deg = mBoostGauge->addDegrees(65);
    boost_deg->setStep(5);
    boost_deg->setValueRange(-10,30);
    auto boost_ColorBand = mBoostGauge->addColorBand(50);
    colors.clear();

    pair.first = Qt::darkGreen;
    pair.second = 50;
    colors.append(pair);

    pair.first = Qt::yellow;
    pair.second = 66.5;
    colors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    colors.append(pair);
    boost_ColorBand->setColors(colors);

    QcValuesItem *boost_values = mBoostGauge->addValues(74);
    boost_values->setStep(5);
    boost_values->setValueRange(-10,30);

    mBoostGauge->addLabel(70)->setText("PSI");
    QcLabelItem *labBoost = mBoostGauge->addLabel(40);
    labBoost->setColor(Qt::white);
    labBoost->setText("0");
    mBoostNeedle = mBoostGauge->addNeedle(60);
    mBoostNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mBoostNeedle->setLabel(labBoost);
    mBoostNeedle->setColor(Qt::white);
    mBoostNeedle->setValueRange(-10,30);
    mBoostGauge->addBackground(7);
    mBoostGauge->addGlass(88);

    /*engine = new QQmlApplicationEngine;
    engine->load(QUrl(QLatin1String("qrc:/GaugeScreen.qml")));
    QWindow *qmlWindow = qobject_cast<QWindow*>(engine->rootObjects().at(0));
    QWidget *container = QWidget::createWindowContainer(qmlWindow);
    ui->verticalLayout->addWidget(container);*/
}

void ObdGauge::setSpeed(int speed)
{
    mSpeedNeedle->setCurrentValue(speed);
}

void ObdGauge::setRpm(int rpm)
{
    mRpmNeedle->setCurrentValue(rpm);
}

void ObdGauge::setCoolent(float degree)
{
    mCoolentNeedle->setCurrentValue(degree);
}

void ObdGauge::setBoost(float atm)
{
    mBoostNeedle->setCurrentValue(atm);
}

void ObdGauge::timerEvent( QTimerEvent *event )
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
        //ui->labelCommand->setText(runtimeCommands[commandOrder]);
        commandOrder++;
    }

//    auto timeStep = m_time.restart();
//    m_realTime = m_realTime + timeStep / 100.0f;
//    valueGauge  =  111.0f * std::sin( m_realTime /  5.0f ) +  111.0f;
//    setSpeed(static_cast<int>(valueGauge));
//    setRpm(static_cast<int>(valueGauge/2.75));
}


QString ObdGauge::send(const QString &command)
{
    if(mRunning && ConnectionManager::getInstance())
    {
        ConnectionManager::getInstance()->send(command);
    }

    return QString();
}

bool ObdGauge::isError(std::string msg) {
    std::vector<std::string> errors(ERROR, ERROR + 18);
    for(unsigned int i=0; i < errors.size(); i++) {
        if(msg.find(errors[i]) != std::string::npos)
            return true;
    }
    return false;
}

QString ObdGauge::getData(const QString &command)
{
    auto dataReceived = ConnectionManager::getInstance()->readData(command);

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
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            setCoolent(value);
            break;        
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
        case 51://PID(33) Absolute Barometric Pressure
            //A kPa
            value = A;
            barometric_pressure = value * 0.1450377377; //kPa to psi
            break;
        case 11://PID(0B): Manifold Absolute Pressure
            // A
            value = A;
            if (barometric_pressure == 0.0)
            {
                auto alt = m_gps->altitude();
                barometric_pressure = Gps::barometricPressure(alt) * 0.000145037738; //pascals to psi
            }

            qDebug() << barometric_pressure;

            setBoost(value * 0.1450377377 - barometric_pressure);
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
//        labelCommand->setText(runtimeCommands.join(", ") + "\n" + runtimeCommands[commandOrder]);
    }

    if(commandOrder < runtimeCommands.size())
    {
        send(runtimeCommands[commandOrder]);
        labelCommand->setText(runtimeCommands[commandOrder]);
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

void ObdGauge::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    mRunning = false;
    stopQueue();
}


void ObdGauge::orientationChanged(Qt::ScreenOrientation orientation)
{  
    switch (orientation) {
    case Qt::ScreenOrientation::PortraitOrientation:
        ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(mCoolentGauge, 1, 0);
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 2, 0);
        break;
    case Qt::ScreenOrientation::LandscapeOrientation:
        ui->gridLayout_Gauges->addWidget(mSpeedGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(mCoolentGauge, 0, 1);
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 2);
        break;
    default:
        break;
    }    
}

