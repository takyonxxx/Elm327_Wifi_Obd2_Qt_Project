#ifndef OBDSCAN_H
#define OBDSCAN_H

#include <QMainWindow>
#include "networkmanager.h"
#include "bluetoothmanager.h"
#include <QVector>

namespace Ui {
class ObdScan;
}

class ObdScan : public QMainWindow
{
    Q_OBJECT

public:
    explicit ObdScan(QWidget *parent = nullptr);
    ~ObdScan();

private:
    NetworkManager *m_networkManager;
    BluetoothManager *m_bluetoothManager;
    int commandOrder{0};
    bool mRunning{false};
    bool getFuelPid{false};
    int mSpeed{0};
    int mRpm{0};
    int mLoad{0};
    int mMAF{0};
    int mTPos{0};
    qreal mFuelConsumption{0.0};    
    QVector<qreal> mAvarageFuelConsumption{};

    void send(const QString &);
    void analysData(const QString &);    
    qreal calculateAverage(QVector<qreal> &listavg) ;

private slots:
    void on_pushExit_clicked();    
    void dataReceived(QString &);
    void on_pushClear_clicked();
    void on_comboEngineDisplacement_currentIndexChanged(const QString &arg1);

signals:
    void on_close_scan();

protected:
    void closeEvent (QCloseEvent *) override;


private:
    Ui::ObdScan *ui;
};

#endif // OBDSCAN_H
