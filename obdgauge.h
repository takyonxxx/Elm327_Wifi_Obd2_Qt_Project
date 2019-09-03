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

    GaugeWidget * mRpmGauge;
    NeedleItem *mRpmNeedle;
    bool mRunning{false};

    void send(QString &);
    void analysData(const QString &);

private slots:
    void dataReceived(QString &);

    void on_pushExit_clicked();

signals:
    void on_close_gauge();

protected:
    void closeEvent (QCloseEvent *) override;

private:
    Ui::ObdGauge *ui;
};

#endif // OBDGAUGE_H
