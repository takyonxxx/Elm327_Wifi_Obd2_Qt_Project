#include "obdgauge.h"
#include "ui_obdgauge.h"
#include "pid.h"

ObdGauge::ObdGauge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdGauge)
{
    ui->setupUi(this);

    setWindowTitle("Elm327 Obd2");

    mEngineDisplacement = 1500;

    //this->centralWidget()->setStyleSheet("background-image: url(:/img/carbon-fiber.png); border: none;");
    this->centralWidget()->setStyleSheet("background-color:#17202A ; border: none;");

    pushReset = new QPushButton;
    pushExit = new QPushButton;
    pushReset->setText("Reset");
    pushExit->setText("Exit");
    pushExit->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #212F3C; padding: 6px; spacing: 6px;");
    pushReset->setStyleSheet("font-size: 22pt; font-weight: bold; color: white;background-color: #212F3C; padding: 6px; spacing: 6px;");

    connect(pushReset, &QPushButton::clicked, this, &ObdGauge::on_pushReset_clicked);
    connect(pushExit, &QPushButton::clicked, this, &ObdGauge::on_pushExit_clicked);

    initGauges();

    lbl_fuel = new QLabel(this);
    lbl_fuel->setStyleSheet("font: 32pt 'Trebuchet MS'; font-weight: bold; color: #CACFD2  ; background-color: #212F3C ;  padding: 6px; spacing: 6px;");
    lbl_fuel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lbl_fuel->setText("Avg fuel cons:\n-- L / 100km");
    lbl_fuel->setAlignment(Qt::AlignCenter | Qt::AlignCenter);

    lbl_temp = new QLabel(this);
    lbl_temp->setStyleSheet("font: 32pt 'Trebuchet MS'; font-weight: bold; color: #CACFD2  ; background-color: #212F3C ;  padding: 6px; spacing: 6px;");
    lbl_temp->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lbl_temp->setText("Coolant temp:\n-- C°");
    lbl_temp->setAlignment(Qt::AlignCenter | Qt::AlignCenter);

    if(osName() == "windows")
    {
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(lbl_fuel, 1, 0);
        ui->gridLayout_Gauges->addWidget(lbl_temp, 2, 0);
        ui->gridLayout_Gauges->addWidget(pushReset, 3, 0);
        ui->gridLayout_Gauges->addWidget(pushExit, 4, 0);
    }
    else
    {
        auto gauge_size = int(lbl_fuel->height() + lbl_temp->height());
        mRpmGauge->setMinimumHeight(this->height() - 2*gauge_size);

        foreach (QScreen *screen, QGuiApplication::screens())
        {
            if(screen->orientation() == Qt::LandscapeOrientation)
            {
                ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 0, 2, 1);
                ui->gridLayout_Gauges->addWidget(lbl_fuel,0, 1);
                ui->gridLayout_Gauges->addWidget(lbl_temp,1, 1);
                ui->gridLayout_Gauges->addWidget(pushReset, 2, 0);
                ui->gridLayout_Gauges->addWidget(pushExit, 2, 1);
            }
            else if(screen->orientation() == Qt::PortraitOrientation)
            {
                ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 0);
                ui->gridLayout_Gauges->addWidget(lbl_fuel, 1, 0);
                ui->gridLayout_Gauges->addWidget(lbl_temp, 2, 0);
                ui->gridLayout_Gauges->addWidget(pushReset, 3, 0);
                ui->gridLayout_Gauges->addWidget(pushExit, 4, 0);
            }

            screen->setOrientationUpdateMask(Qt::LandscapeOrientation |
                                             Qt::PortraitOrientation |
                                             Qt::InvertedLandscapeOrientation |
                                             Qt::InvertedPortraitOrientation);

            QObject::connect(screen, &QScreen::orientationChanged, this, &ObdGauge::orientationChanged);
        }
    }

    mRpmGauge->update();

    runtimeCommands.clear();
    runtimeCommands.append(VEHICLE_SPEED);
    runtimeCommands.append(ENGINE_RPM);
    runtimeCommands.append(ENGINE_LOAD);
    runtimeCommands.append(COOLANT_TEMP);
    runtimeCommands.append(MAF_AIR_FLOW);

    if(ConnectionManager::getInstance() && ConnectionManager::getInstance()->isConnected())
    {
        connect(ConnectionManager::getInstance(),&ConnectionManager::dataReceived,this, &ObdGauge::dataReceived);
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
    /*mSpeedGauge = new QcGaugeWidget;
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

    mSpeedGauge->addLabel(60)->setText("Km/h");
    QcLabelItem *labSpeed = mSpeedGauge->addLabel(40);
    labSpeed->setText("0");
    mSpeedNeedle = mSpeedGauge->addNeedle(60);
    mSpeedNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mSpeedNeedle->setLabel(labSpeed);
    mSpeedNeedle->setColor(Qt::white);
    mSpeedNeedle->setValueRange(0,220);
    mSpeedGauge->addBackground(7);
    mSpeedGauge->addGlass(88);*/

    QList<QPair<QColor, float> > colors;
    QPair<QColor,float> pair;

    //rpm
    mRpmGauge = new QcGaugeWidget;
    mRpmGauge->addBackground(99);
    QcBackgroundItem *bkgRpm1 = mRpmGauge->addBackground(92);
    bkgRpm1->clearrColors();
    bkgRpm1->addColor(0.1,Qt::black);
    bkgRpm1->addColor(1.0,Qt::gray);

    QcBackgroundItem *bkgRpm2 = mRpmGauge->addBackground(88);
    bkgRpm2->clearrColors();
    bkgRpm2->addColor(0.1,Qt::black);
    bkgRpm2->addColor(1.0,Qt::blue);

    mRpmGauge->addArc(55);
    mRpmGauge->addDegrees(65)->setValueRange(0,70);
    auto colorBandRpm = mRpmGauge->addColorBand(50);
    colors.clear();

    pair.first = Qt::darkGreen;
    pair.second = 33;
    colors.append(pair);

    pair.first = Qt::yellow;
    pair.second = 68;
    colors.append(pair);

    pair.first = Qt::red;
    pair.second = 100;
    colors.append(pair);
    colorBandRpm->setColors(colors);

    mRpmGauge->addValues(74)->setValueRange(0,70);

    mRpmGauge->addLabel(60)->setText("X100");
    QcLabelItem *labRpm = mRpmGauge->addLabel(40);
    labRpm->setText("0");
    mRpmNeedle = mRpmGauge->addNeedle(80);
    mRpmNeedle->setNeedle(QcNeedleItem::DiamonNeedle);
    mRpmNeedle->setLabel(labRpm);
    mRpmNeedle->setColor(Qt::white);
    mRpmNeedle->setValueRange(0,70);
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
        case 4://PID(04): Engine Load
            // A*100/255
            mLoad = A * 100 / 255;
            break;
        case 5://PID(05): Coolant Temperature
            // A-40
            value = A - 40;
            lbl_temp->setText("Coolant temp:\n" + QString::number(value, 'f', 0) + " C°");
            break;
        case 12: //PID(0C): RPM
            //((A*256)+B)/4
            value = ((A * 256) + B) / 4;
            setRpm(static_cast<int>(value / 100));
            break;
        case 13://PID(0D): KM Speed
            // A
            mSpeed = A;
            break;
        case 16://PID(10): MAF air flow rate grams/sec
            // ((256*A)+B) / 100  [g/s]
            mMAF = ((256 * A) + B) / 100;
            break;
        case 94://PID(5E) Fuel rate
            // ((A*256)+B) / 20
        {
            value = ((A*256)+B) / 20;
            //lbl_fuel->setText("Avg fuel cons:\n12.3 L / 100");
            /*mAvarageFuelConsumption.append(value);
            lbl_fuel->setText(QString::number(calculateAverage(mAvarageFuelConsumption), 'f', 1) + " l / h");*/
        }
        default:
            //A
            value = A;
            break;
        }

        if(PID == 4 || PID == 12 || PID == 13) // LOAD, RPM, SPEED
        {
            auto AL = mMAF * mLoad;  // Airflow * Load
            auto coeff = (mEngineDisplacement / 1000.0) / 714.0; // Fuel flow coefficient
            auto FuelFlowLH = AL * coeff + 1.0;   // Fuel flow L/h

            if(FuelFlowLH > 99)
                FuelFlowLH = 99;

            if(mLoad == 0)
                FuelFlowLH = 0;

            mAvarageFuelConsumption.append(FuelFlowLH);
            lbl_fuel->setText("Avg fuel cons:\n" + QString::number(calculateAverage(mAvarageFuelConsumption), 'f', 1) + "  L / h");

            if(mSpeed > 0)
            {
                auto mFuelLPer100 = FuelFlowLH * 100 / mSpeed;   // FuelConsumption in l per 100km
                if(mFuelLPer100 > 99)
                    mFuelLPer100 = 99;

                mAvarageFuelConsumption100.append(mFuelLPer100);
                lbl_fuel->setText("Avg fuel cons:\n" + QString::number(calculateAverage(mAvarageFuelConsumption100), 'f', 1) + "  L / 100km");
            }
        }
    }
}

qreal ObdGauge::calculateAverage(QVector<qreal> &listavg)
{
    qreal sum = 0.0;
    for (auto &val : listavg) {
        sum += val;
    }
    return sum / listavg.size();
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

void ObdGauge::on_pushReset_clicked()
{
    lbl_fuel->setText("Avg fuel cons:\n-- L / 100km");
    lbl_temp->setText("Coolant temp:\n-- C°");
    lbl_temp->setAlignment(Qt::AlignCenter | Qt::AlignCenter);
    mAvarageFuelConsumption.clear();
    mAvarageFuelConsumption100.clear();
}

void ObdGauge::orientationChanged(Qt::ScreenOrientation orientation)
{  
    switch (orientation) {
    case Qt::ScreenOrientation::PortraitOrientation:
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 0);
        ui->gridLayout_Gauges->addWidget(lbl_fuel, 1, 0);
        ui->gridLayout_Gauges->addWidget(lbl_temp, 2, 0);
        ui->gridLayout_Gauges->addWidget(pushReset, 3, 0);
        ui->gridLayout_Gauges->addWidget(pushExit, 4, 0);
        break;
    case Qt::ScreenOrientation::LandscapeOrientation:
        ui->gridLayout_Gauges->addWidget(mRpmGauge, 0, 0, 2, 1);
        ui->gridLayout_Gauges->addWidget(lbl_fuel,0, 1);
        ui->gridLayout_Gauges->addWidget(lbl_temp,1, 1);
        ui->gridLayout_Gauges->addWidget(pushReset, 2, 0);
        ui->gridLayout_Gauges->addWidget(pushExit, 2, 1);
        break;
    default:
        break;
    }

}
