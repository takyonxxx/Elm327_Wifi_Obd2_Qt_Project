#ifndef OBDGAUGE_H
#define OBDGAUGE_H

#include <QMainWindow>
#include <QPushButton>
#include <QScreen>
#include <QLabel>
#include "global.h"

#include "qcgaugewidget.h"
#include "elm.h"

namespace Ui {
class ObdGauge;
}

class ObdGauge : public QMainWindow
{
    Q_OBJECT

public:
    explicit ObdGauge(QWidget *parent = nullptr);
    ~ObdGauge();

private:
    int commandOrder{0};

    int m_timerId{};
    float m_realTime{};
    QTime m_time{};

    QLabel *labelCommand;

    int valueGauge{0};
    int map{0};
    int barometric_pressure{0};
    bool mRunning{false};

    QcGaugeWidget * mSpeedGauge{};
    QcNeedleItem *mSpeedNeedle{};

    QcGaugeWidget * mRpmGauge{};
    QcNeedleItem *mRpmNeedle{};

    QcGaugeWidget * mCoolentGauge{};
    QcNeedleItem *mCoolentNeedle{};

    QcGaugeWidget * mBoostGauge{};
    QcNeedleItem *mBoostNeedle{};

    ELM *elm{};

    void startQueue();
    void stopQueue();

    QString send(const QString &);
    QString getData(const QString &);
    bool isError(std::string);

    void analysData(const QString &);
    void initGauges();
    void setSpeed(int);
    void setRpm(int);
    void setCoolent(float);
    void setBoost(float);

private slots:
    void dataReceived(QString);
    void orientationChanged(Qt::ScreenOrientation );

protected:
    void closeEvent (QCloseEvent *) override;
    void timerEvent( QTimerEvent * ) override;

private:
    Ui::ObdGauge *ui;
};

#endif // OBDGAUGE_H
