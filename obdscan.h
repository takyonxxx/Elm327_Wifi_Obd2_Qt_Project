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
    bool mRunning{false};    
    int mSpeed{0};
    int mLoad{0};
    int commandOrder{0};
    ELM *elm{};
    QString send(const QString &);
    void analysData(const QString &);
public slots:
    void dataReceived(QString);

private slots:
    void on_pushExit_clicked();
signals:
    void on_close_scan();

protected:
    void closeEvent (QCloseEvent *) override;

private:
    Ui::ObdScan *ui;
};

#endif // OBDSCAN_H
