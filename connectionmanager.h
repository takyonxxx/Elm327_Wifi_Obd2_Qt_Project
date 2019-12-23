#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include "elmtcpsocket.h"
#include "elmblesocket.h"
#include "settingsmanager.h"

enum ConnectionType {BlueTooth, Wifi, Serial, None};

class ConnectionManager : public QObject
{
      Q_OBJECT

public:
    explicit ConnectionManager(QObject *parent = nullptr);
    static ConnectionManager* getInstance();

    void connectElm();
    void disConnectElm();

    bool send(const QString &);
    QString readData(const QString &command);
    void setCType(const ConnectionType &value);

private:
    ConnectionType cType{None};
    SettingsManager *m_settingsManager{};
    ElmTcpSocket *mElmTcpSocket{};
    ElmBleSocket *mElmBleSocket{};

signals:
    void dataReceived(QString &);
    void stateChanged(QString &);
    void connected();
    void disconnected();
    void addBleDevice(const QBluetoothAddress&, const QString&);

public slots:
    void conConnected();
    void conDisconnected();
    void conDataReceived(QString &);
    void conStateChanged(QString &);
    void conAddBleDevice(const QBluetoothAddress&, const QString&);

private:
     static ConnectionManager* theInstance_;

};

#endif // CONNECTIONMANAGER_H
