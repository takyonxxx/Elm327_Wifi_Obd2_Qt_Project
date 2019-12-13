#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager();
    ~NetworkManager();

    static NetworkManager* getInstance();

public: signals:
    void dataReceived(QString &);
    void dataHexReceived(QString &);
    void stateChanged(QString &);
    void errorAccrued(QString &);
    void wifiConnected();
    void wifiDisconnected();

private slots:
    void connected();
    void disconnected();
    void readyRead();
    void error(QAbstractSocket::SocketError);
    void stateChange(QAbstractSocket::SocketState);

public:
    bool send(QString &string);
    void connectWifi(const QString &, int );
    void disconnectWifi();
    bool isConnected();
    QString readData(QString &);

private:
    QString statetoString(QAbstractSocket::SocketState);
    static NetworkManager* theInstance_;
    QTcpSocket *socket{nullptr};
    bool m_connected{false};
    bool customRead {false};
};

#endif // NETWORKMANAGER_H
