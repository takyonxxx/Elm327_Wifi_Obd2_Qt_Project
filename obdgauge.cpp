#include "obdgauge.h"
#include "ui_obdgauge.h"
#include "pid.h"

ObdGauge::ObdGauge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ObdGauge)
{
    ui->setupUi(this);
        
    setWindowTitle("Elm327 Obd2");

    ui->pushExit->setStyleSheet("font-size: 12pt; font-weight: bold; color: white;background-color: #8F3A3A;");

    //speed
    mSpeedGauge = new GaugeWidget;
    mSpeedGauge->addBackground(99);
    BackgroundItem *bkgspeed1 = mSpeedGauge->addBackground(92);
    bkgspeed1->clearrColors();
    bkgspeed1->addColor(0.1,Qt::black);
    bkgspeed1->addColor(1.0,Qt::white);

    BackgroundItem *bkgspeed2 = mSpeedGauge->addBackground(88);
    bkgspeed2->clearrColors();
    bkgspeed2->addColor(0.1,Qt::black);
    bkgspeed2->addColor(1.0,Qt::darkGray);

    mSpeedGauge->addArc(55);
    mSpeedGauge->addDegrees(65)->setValueRange(0,240);
    mSpeedGauge->addColorBand(50);

    mSpeedGauge->addValues(76)->setValueRange(0,240);

    mSpeedGauge->addLabel(70)->setText("Km/h");
    LabelItem *labspeed = mSpeedGauge->addLabel(40);
    labspeed->setText("0");
    mSpeedNeedle = mSpeedGauge->addNeedle(60);
    mSpeedNeedle->setLabel(labspeed);
    mSpeedNeedle->setColor(Qt::yellow);
    mSpeedNeedle->setValueRange(0,240);
    mSpeedGauge->addBackground(7);
    mSpeedGauge->addGlass(88);

    //rpm
    mRpmGauge = new GaugeWidget;
    mRpmGauge->addBackground(99);
    BackgroundItem *bkgrpm1 = mRpmGauge->addBackground(92);
    bkgrpm1->clearrColors();
    bkgrpm1->addColor(0.1,Qt::black);
    bkgrpm1->addColor(1.0,Qt::white);

    BackgroundItem *bkgrpm2 = mRpmGauge->addBackground(88);
    bkgrpm2->clearrColors();
    bkgrpm2->addColor(0.1,Qt::black);
    bkgrpm2->addColor(1.0,Qt::darkGray);

    mRpmGauge->addArc(55);
    mRpmGauge->addDegrees(65)->setValueRange(0,80);
    mRpmGauge->addColorBand(50);

    mRpmGauge->addValues(76)->setValueRange(0,80);

    mRpmGauge->addLabel(70)->setText("rpm x 100");
    LabelItem *labrpm = mRpmGauge->addLabel(40);
    labrpm->setText("0");
    mRpmNeedle = mRpmGauge->addNeedle(60);
    mRpmNeedle->setLabel(labrpm);
    mRpmNeedle->setColor(Qt::yellow);
    mRpmNeedle->setValueRange(0,80);
    mRpmGauge->addBackground(7);
    mRpmGauge->addGlass(88);

    ui->verticalLayout->addWidget(mSpeedGauge);
    ui->verticalLayout->addWidget(mRpmGauge);
    ui->verticalLayout->addWidget( ui->pushExit);

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
    delete mSpeedGauge;
    delete ui;
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
            if(mRpmNeedle)
                mRpmNeedle->setCurrentValue(static_cast<float>(value / 100));
            break;
        case 4://PID(04): Engine Load
            // A*100/255
            value = A * 100 / 255;
            break;
        case 13://PID(0D): KM Speed
            // A
            value = A;
            if(mSpeedNeedle)
                mSpeedNeedle->setCurrentValue(static_cast<float>(value));
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
