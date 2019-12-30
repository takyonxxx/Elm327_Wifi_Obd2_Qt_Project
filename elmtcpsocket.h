#ifndef ELMTCPSOCKET_H
#define ELMTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QThread>

class ElmTcpSocket : public QThread
{
    Q_OBJECT
public:
    explicit ElmTcpSocket(QObject *parent=nullptr);
    ~ElmTcpSocket();
    void run();
    bool send(const QString &);
    bool sendAsync(const QString &);
    QString readData(const QString &);
    QString checkData();
    void connectTcp(const QString &, const quint16 &);
    void disconnectTcp();
    bool isConnected();

private:
    QTcpSocket *socket;
    QByteArray byteblock{};
    QString returnedData{};
    bool m_connected{false};
    bool m_lockDataReady{false};
    QString statetoString(QAbstractSocket::SocketState);

public slots:
    void connected();
    void disconnected();
    void readyRead();
    void stateChange(QAbstractSocket::SocketState);
    void socketError(QAbstractSocket::SocketError);
signals:
    void dataReceived(QString);
    void stateChanged(QString);
    void tcpConnected();
    void tcpDisconnected();

};


#endif // ELMTCPSOCKET_H
