#ifndef ELMSERIALPORT_H
#define ELMSERIALPORT_H

#include <QObject>

#define     PARITY_7E1      1
#define     PARITY_8N1      2


class ElmSerialPort : public QObject
{
    Q_OBJECT

public:
    explicit ElmSerialPort(QObject *parent=nullptr);
    ~ElmSerialPort();

    bool connectSerial(const QString &);
    bool doConnect(char* dev, int baud, int parity, int blocking);
    void disconnectSerial();
    bool send(const QString &);
    QString readData(const QString &);
    bool isConnected() const;

signals:
    void dataReceived(QString &);
    void stateChanged(QString &);
    void serialConnected();
    void serialDisConnected();

private:
    char* _dev;
    char* _buffer;
    char* _temp;
    int _baud;
    int _dataBits;
    int _bufferSize;
    int _parity;
    int _bufferIndex;
    int _blocking;
    int _fd;
    bool _connected{false};
    unsigned int elm327_timeout_seconds = 1;

private:
    int initPort();
    void flushPort();

    int sendData(char* );
    QString getData();

    int getBaud() const;
    void setBaud(int );
    int getBlocking() const;
    void setBlocking(int );
    char* getDev() const;
    void setDev(char* );
    int getParity() const;
    void setParity(int );

};

#endif // ELMSERIALPORT_H

