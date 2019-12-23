#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QTimer>
#include "connectionmanager.h"
#include "settingsmanager.h"
#include "obdscan.h"
#include "obdgauge.h"

#define SCREEN_ORIENTATION_LANDSCAPE 0
#define SCREEN_ORIENTATION_PORTRAIT 1

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif
#include "elm.h"

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

#ifdef Q_OS_ANDROID
    bool setScreenOrientation(int orientation);
#endif

private:
    void connectElm();
    void disConnectElm();
    QString send(const QString &);
    void analysData(const QString &);
    void saveSettings();

    ConnectionManager *m_connectionManager{};
    SettingsManager *m_settingsManager{};
    ELM *elm{};

    int commandOrder{0};
    bool m_initialized{false};
    bool m_ConsoleEnable{true};
    bool m_clearCodeRequest{false};
    std::vector<uint32_t> cmds{};

private slots:
    void connected();
    void disconnected();
    void dataReceived(QString &);
    void stateChanged(QString &state);
    void errorAccrued(QString &);    
    void on_pushConnect_clicked();
    void on_pushExit_clicked();
    void on_pushSend_clicked();
    void on_pushClear_clicked();
    void on_pushDiagnostic_clicked();
    void on_pushScan_clicked();
    void on_pushGauge_clicked();
    void orientationChanged(Qt::ScreenOrientation orientation);
    void on_radioBle_clicked(bool checked);
    void on_radioWifi_clicked(bool checked);
    void on_close_dialog_triggered();
    void addBleDeviceToList(const QBluetoothAddress&, const QString&);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
