#ifndef OBDGAUGE_H
#define OBDGAUGE_H

#include <QMainWindow>
#include "networkmanager.h"
#include "gaugewidget.h"

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
    GaugeWidget * mSpeedGauge;
    NeedleItem *mSpeedNeedle;

    int m_timerId{};
    float m_realTime{};
    QTime m_time{};
    int valueGauge{0};

    GaugeWidget * mRpmGauge;
    NeedleItem *mRpmNeedle;
    bool mRunning{false};

    void send(QString &);
    void analysData(const QString &);
    void startSim();
    void stopSim();

private slots:
    void dataReceived(QString &);

    void on_pushExit_clicked();

    void on_pushSim_clicked();

signals:
    void on_close_gauge();

protected:
    void closeEvent (QCloseEvent *) override;
    void timerEvent( QTimerEvent * ) override;

private:
    Ui::ObdGauge *ui;
};

#endif // OBDGAUGE_H
