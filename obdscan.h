#ifndef OBDSCAN_H
#define OBDSCAN_H

#include "global.h"
#include "elm.h"
#include "settingsmanager.h"

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

    int m_timerId{};
    float m_realTime{};
    QTime m_time{};

    bool mRunning{false};
    int commandOrder{0};

    ELM *elm{};

    QString send(const QString &);
    QString getData(const QString &);
    bool isError(std::string);

    void analysData(const QString &);
    void startQueue();
    void stopQueue();

public slots:
    void dataReceived(QString);

private slots:
    void on_pushExit_clicked();

protected:
    void closeEvent (QCloseEvent *) override;
    void timerEvent( QTimerEvent * ) override;

private:
    Ui::ObdScan *ui;
};

#endif // OBDSCAN_H
