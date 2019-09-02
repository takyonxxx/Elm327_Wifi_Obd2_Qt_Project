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
    void readed();
    void error(QAbstractSocket::SocketError);
    void stateChange(QAbstractSocket::SocketState);


public:
    void send(QString &string);
    void connectWifi(const QString &, int );
    void disconnectWifi();
    bool isConnected();

private:
    QString statetoString(QAbstractSocket::SocketState);

    static NetworkManager* theInstance_;
    QTcpSocket *socket{nullptr};
    bool m_connected;
};

#endif // NETWORKMANAGER_H
