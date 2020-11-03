#ifndef OBDSCAN_H
#define OBDSCAN_H

#include <QMainWindow>
#include <QVector>
#include "connectionmanager.h"
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
    bool mRunning{false};    
    int mSpeed{0};
    int mMAF{0};
    int mLoad{0};
    int commandOrder{0};
    unsigned int mEngineDisplacement{0};
    QStringList runtimeCommands{};
    QVector<qreal> mAvarageFuelConsumption{};
    QVector<qreal> mAvarageFuelConsumption100{};
    ELM *elm{};
    QString send(const QString &);
    void analysData(const QString &);    
    qreal calculateAverage(QVector<qreal> &listavg) ;

public slots:
    void dataReceived(QString);

private slots:
    void on_pushExit_clicked();
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
