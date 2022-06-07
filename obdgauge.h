#ifndef OBDGAUGE_H
#define OBDGAUGE_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "qcgaugewidget.h"
#include "elm.h"
#include "connectionmanager.h"

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
    QRect rect{};
    int commandOrder{0};
    QStringList runtimeCommands{};

    int m_timerId{};
    float m_realTime{};
    QTime m_time{};
    int valueGauge{0};
    bool mRunning{false};
    QPushButton *pushReset;
    QPushButton *pushExit;

    QcGaugeWidget * mSpeedGauge{};
    QcNeedleItem * mSpeedNeedle{};

    QcGaugeWidget * mRpmGauge{};
    QcNeedleItem * mRpmNeedle{};

    QLabel *lbl_fuel{};
    QLabel *lbl_temp{};
    QLabel *lbl_voltage{};

    int mMAF{0};
    int mLoad{0};
    int mEngineDisplacement{0};
    int mSpeed{0};
    QVector<qreal> mAvarageFuelConsumption{};
    QVector<qreal> mAvarageFuelConsumption100{};

    ELM *elm{};

    QString send(const QString &);
    void analysData(const QString &);
    void initGauges();
    void startSim();
    void stopSim();
    void setSpeed(int);
    void setRpm(int);
    qreal calculateAverage(QVector<qreal> &listavg);

private slots:
    void dataReceived(QString);
    void on_pushExit_clicked();
    void on_pushReset_clicked();
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
