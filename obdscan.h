#ifndef OBDSCAN_H
#define OBDSCAN_H

#include <QMainWindow>
#include "networkmanager.h"

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
    int commandOrder{0};

    void send(QString &);
    void analysData(const QString &);

private slots:
    void on_pushExit_clicked();    
    void dataReceived(QString &);

signals:
    void on_close_scan();

protected:
    void closeEvent (QCloseEvent *) override;


private:
    Ui::ObdScan *ui;
};

#endif // OBDSCAN_H
