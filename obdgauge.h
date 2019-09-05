#ifndef OBDGAUGE_H
#define OBDGAUGE_H

#include <QMainWindow>
#include <QPushButton>
#include "qcgaugewidget.h"
#include "networkmanager.h"

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
    NetworkManager *m_networkManager;
    int commandOrder{0};

    int m_timerId{};
    float m_realTime{};
    QTime m_time{};
    int valueGauge{0};
    bool mRunning{false};
    QPushButton *pushSim;
    QPushButton *pushExit;

    QcGaugeWidget * mSpeedGauge{};
    QcNeedleItem *mSpeedNeedle{};

    QcGaugeWidget * mRpmGauge{};
    QcNeedleItem *mRpmNeedle{};

    void send(QString &);
    void analysData(const QString &);
    void initGauges();
    void startSim();
    void stopSim();
    void setSpeed(int);
    void setRpm(int);

private slots:
    void dataReceived(QString &);
    void on_pushExit_clicked();
    void on_pushSim_clicked();
    void orientationChanged(Qt::ScreenOrientation );

signals:
    void on_close_gauge();

protected:
    void closeEvent (QCloseEvent *) override;
    void timerEvent( QTimerEvent * ) override;

private:
    Ui::ObdGauge *ui;
};

#endif // OBDGAUGE_H
