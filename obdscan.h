#ifndef OBDSCAN_H
#define OBDSCAN_H

#include <QMainWindow>
#include <QVector>
#include "elm.h"

namespace Ui {
class ObdScan;
}

class ObdScan : public QMainWindow
{
    Q_OBJECT

public:
    explicit ObdScan(QWidget *parent = nullptr);
    ~ObdScan() override;

private:
    QMutex m_mutex{};
    int commandOrder{0};
    bool mRunning{false};
    bool getFuelPid{false};
    int mSpeed{0};
    int mRpm{0};
    int mLoad{0};
    int mMAF{0};
    int mTPos{0};    
    QVector<qreal> mAvarageFuelConsumption{};
    QVector<qreal> mAvarageFuelConsumption100{};
    ELM *elm{};
    NetworkManager *manager{};
    static void *scanThread(void * this_ptr);
    pthread_t m_scanThread{};

    QString send(const QString &);
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
