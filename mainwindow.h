#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QTimer>
#include <QScreen>
#include "connectionmanager.h"
#include "settingsmanager.h"
#include "obdscan.h"
#include "obdgauge.h"
#include "elm.h"

#define SCREEN_ORIENTATION_LANDSCAPE 0
#define SCREEN_ORIENTATION_PORTRAIT 1

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif


#if defined (Q_OS_ANDROID)
#include <QtAndroid>
const QVector<QString> permissions({"android.permission.BLUETOOTH",
                                    "android.permission.BLUETOOTH_ADMIN",
                                    "android.permission.INTERNET",
                                    "android.permission.WRITE_EXTERNAL_STORAGE",
                                    "android.permission.READ_EXTERNAL_STORAGE"});
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


#ifdef Q_OS_ANDROID
    void keep_screen_on(bool on)
    {
        QtAndroid::runOnAndroidThread([on]{
            QAndroidJniObject activity = QtAndroid::androidActivity();
            if (activity.isValid()) {
                QAndroidJniObject window =
                        activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

                if (window.isValid()) {
                    const int FLAG_KEEP_SCREEN_ON = 128;
                    if (on) {
                        window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                    } else {
                        window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                    }
                }
            }
            QAndroidJniEnvironment env;
            if (env->ExceptionCheck()) {
                env->ExceptionClear();
            }
        });
    }    
#endif

#ifdef Q_OS_ANDROID
    bool setScreenOrientation(int orientation);
#endif

private:

    void connectElm();
    void disConnectElm();
    QString send(const QString &);
    void analysData(const QString &);
    void saveSettings();
    bool isError(std::string msg);
    void getPids();

    QRect desktopRect{};
    ConnectionManager *m_connectionManager{};
    SettingsManager *m_settingsManager{};
    ELM *elm{};

    int commandOrder{0};
    bool m_connected{false};
    bool m_initialized{false};
    bool m_searchPidsEnable{false};
    std::vector<uint32_t> cmds{};

private slots:
    void connected();
    void disconnected();
    void dataReceived(QString );
    void stateChanged(QString);
    void on_pushConnect_clicked();
    void on_pushExit_clicked();
    void on_pushSend_clicked();
    void on_pushClear_clicked();
    void on_pushScan_clicked();
    void orientationChanged(Qt::ScreenOrientation orientation);

    void on_pushReadFault_clicked();
    void on_pushClearFault_clicked();
    void on_checkSearchPids_toggled(bool checked);

    void on_pushSetProtocol_clicked();

    void on_pushGetProtocol_clicked();

    void on_pushGauge_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
