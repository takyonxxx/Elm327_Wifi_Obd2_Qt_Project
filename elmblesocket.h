#ifndef ELMBLESOCKET_H
#define ELMBLESOCKET_H

#include <QObject>
#include <QMutex>
#include <QDataStream>
#include <qbluetoothlocaldevice.h>
#include <QBluetoothSocket>
#include <iostream>
#include <thread>
#include <QThread>
#include <QBluetoothDeviceDiscoveryAgent>

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)

QT_USE_NAMESPACE

class ElmBleSocket : public QThread
{
    Q_OBJECT
public:
    explicit ElmBleSocket(const QBluetoothAddress &, QObject *parent=nullptr);
    ~ElmBleSocket();
    void run();
    void scan();

    QBluetoothDeviceDiscoveryAgent *discoveryAgent{};

public slots:
    void addDevice(const QBluetoothDeviceInfo &info);
    void scanFinished();

public: signals:
    void stateChanged(QString &);

private:
    QMutex m_mutex{};
    QBluetoothLocalDevice *localDevice{};
    bool m_connected{false};
    pthread_t m_discoveryThread{};
    static void *discoveryThread(void * this_ptr);
};


#endif // ELMBLESOCKET_H
