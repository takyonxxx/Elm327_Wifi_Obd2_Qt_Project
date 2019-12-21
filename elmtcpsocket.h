#ifndef ELMTCPSOCKET_H
#define ELMTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTimer>
#include <QThread>

class ElmTcpSocket : public QThread
{
    Q_OBJECT
public:
    explicit ElmTcpSocket(const QString &, const quint16 &, QObject *parent=nullptr);
    ~ElmTcpSocket();
    static ElmTcpSocket* getInstance();
    void run();
    bool send(const QString &);
    bool sendAsync(const QString &);
    QString readData(const QString &command);
    QString checkData();
    void connectTcp();
    void disconnectTcp();
    bool isConnected();

private:
    QTcpSocket *socket;
    QByteArray byteblock{};
    QString ip{};
    quint16 port{};
    QString returnedData{};
    bool m_connected{false};
    bool m_lockDataReady{false};

public slots:
    void connected();
    void disconnected();
    void readyRead();
    void error(QAbstractSocket::SocketError);
signals:
    void dataReceived(QString &);
    void stateChanged(QString &);
    void tcpConnected();
    void tcpDisconnected();

private:
    static ElmTcpSocket* theInstance_;

};


#endif // ELMTCPSOCKET_H
