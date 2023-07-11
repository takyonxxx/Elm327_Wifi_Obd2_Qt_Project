#ifndef OBDGAUGE_H
#define OBDGAUGE_H

#include <QPushButton>
#include <QScreen>
#include "qcgaugewidget.h"
#include "global.h"
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
    int valueGauge{0};
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

    QString send(const QString &);
    void analysData(const QString &);
    void initGauges();
    void startSim();
    void stopSim();
    void setSpeed(int);
    void setRpm(int);
    void setCoolent(int);
    void setBoost(int);

private slots:
    void dataReceived(QString);
    void orientationChanged(Qt::ScreenOrientation );

    void on_pushExit_clicked();

protected:
    void closeEvent (QCloseEvent *) override;
    void timerEvent( QTimerEvent * ) override;

private:
    Ui::ObdGauge *ui;
};

#endif // OBDGAUGE_H
