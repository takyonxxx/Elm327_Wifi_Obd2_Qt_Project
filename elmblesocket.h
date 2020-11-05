#ifndef ELMBLESOCKET_H
#define ELMBLESOCKET_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QDataStream>
#include <qbluetoothlocaldevice.h>
#include <QBluetoothSocket>
#include <QCoreApplication>
#include <iostream>
#include <QFuture>
#include <qtconcurrentrun.h>
#include <QThread>
#include <QBluetoothDeviceDiscoveryAgent>

QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QBluetoothDeviceInfo)

QT_USE_NAMESPACE

class ElmBleSocket : public QThread
{
    Q_OBJECT
public:
    explicit ElmBleSocket(QObject *parent=nullptr);
    ~ElmBleSocket();
    void run();
    void startScan();
    void stopScan();
    bool sendAsync(const QString &command);
    bool send(const QString &);
    QString readData(const QString &command);
    void connectBle(const QBluetoothAddress &);
    void disconnectBle();
    bool isConnected();

public slots:
    void addDevice(const QBluetoothDeviceInfo &);
    void bleStateChanged(QBluetoothSocket::SocketState);

private slots:
    void scanFinished();
    void connected();
    void disconnected();    
    void readyRead();
    void socketError(QBluetoothSocket::SocketError);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error);
    QString statetoString(QBluetoothSocket::SocketState);


signals:
    void dataReceived(QString);
    void stateChanged(QString);
    void bleConnected();
    void bleDisconnected();
    void addBleDevice(const QBluetoothAddress&, const QString&);


private:
    QMutex m_mutex{};
    QBluetoothSocket* socket{};
    QBluetoothLocalDevice *localDevice{};
    QBluetoothDeviceDiscoveryAgent *discoveryAgent{};
    QByteArray byteblock{};
    bool m_connected{false};

    void scanBle();
    void handleDiscoveryTimeout();
    QTimer m_scanTimer{};
};


#endif // ELMBLESOCKET_H
