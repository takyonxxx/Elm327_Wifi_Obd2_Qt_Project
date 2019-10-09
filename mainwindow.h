#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QTimer>
#include "networkmanager.h"
#include "obdscan.h"
#include "obdgauge.h"

#define SCREEN_ORIENTATION_LANDSCAPE 0
#define SCREEN_ORIENTATION_PORTRAIT 1

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
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

    bool requestFineLocationPermission()
    {
        QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");
        if (request == QtAndroid::PermissionResult::Denied){
            QtAndroid::requestPermissionsSync(QStringList() <<  "android.permission.ACCESS_FINE_LOCATION");
            request = QtAndroid::checkPermission("android.permission.ACCESS_FINE_LOCATION");
            if (request == QtAndroid::PermissionResult::Denied)
            {
                qDebug() << "FineLocation Permission denied";
                return false;
            }
        }
        qDebug() << "FineLocation Permissions granted!";
        return true;
    }

    bool requestStorageWritePermission()
    {
        QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        if(request == QtAndroid::PermissionResult::Denied) {
            QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" );
            request = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
            if(request == QtAndroid::PermissionResult::Denied)
            {
                qDebug() << "StorageWrite Permission denied";
                return false;
            }
        }
        qDebug() << "StorageWrite Permissions granted!";
        return true;
    }
#endif

private:
    void send(QString &string);
    void analysData(const QString &);
    void setPidsSupported(const QString &dataReceived);
#ifdef Q_OS_ANDROID
    bool setScreenOrientation(int orientation);
#endif
    NetworkManager *m_networkManager;
    int commandOrder{0};
    bool m_initialized{false};
    bool m_ConsoleEnable{true};
    bool m_HexEnabled{false};
    bool m_clearCodeRequest{false};

private slots:
    void connected();
    void disconnected();
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
    void orientationChanged(Qt::ScreenOrientation orientation);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
