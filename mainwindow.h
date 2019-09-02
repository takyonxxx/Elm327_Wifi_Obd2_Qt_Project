#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QTimer>
#include "networkmanager.h"
#include "obdscan.h"
#include "obdgauge.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void send(QString &string);
    void analysData(const QString &);

    int foundCount;
    QNetworkConfiguration netcfg;
    QList<QNetworkConfiguration> netcfgList;
    QTimer *refreshTimer;

    NetworkManager *m_networkManager;
    int commandOrder{0};
    bool m_initialized{false};
    bool m_ConsoleEnable{true};
    bool m_HexEnabled{false};
    bool m_clearCodeRequest{false};

private slots:
    void connected();
    void disconnected();
    void refreshObd();
    void dataReceived(QString &);
    void stateChanged(QString &state);
    void dataHexReceived(QString &);
    void errorAccrued(QString &);
    void on_close_dialog_triggered();
    void on_pushConnect_clicked();
    void on_pushExit_clicked();
    void on_pushSend_clicked();
    void on_pushClear_clicked();
    void on_pushDiagnostic_clicked();
    void on_pushScan_clicked();
    void on_checkHex_stateChanged(int arg1);
    void on_pushGauge_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
