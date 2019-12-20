#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QtNetwork>

class NetworkManager : public QThread
{
    Q_OBJECT

public:
    NetworkManager();
    ~NetworkManager() override;

    static NetworkManager* getInstance();

public:
     void run() override;

public: signals:
    void dataReceived(QString &);
    void dataBytesReceived(QString &);
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
    bool send(const QString &);
    void connectWifi(const QString &, int );
    void disconnectWifi();
    bool isConnected();
    QString checkData();
    QString readData(const QString &);

    void setStop(bool stop);

protected:
    bool m_headerRead{false};
    unsigned int m_size_of_data_to_read;

private:
    QString statetoString(QAbstractSocket::SocketState);
    static NetworkManager* theInstance_;
    QTcpSocket *socket{nullptr};
    QByteArray byteblock{};
    bool m_connected{false};
    bool m_stop{false};
};

#endif // NETWORKMANAGER_H
