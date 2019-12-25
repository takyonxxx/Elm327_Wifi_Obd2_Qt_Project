#ifndef SERIAL_H
#define	SERIAL_H
#include <QObject>
#include <QDebug>

#define     PARITY_7E1      1
#define     PARITY_8N1      2

class Serial : public QObject
{
    Q_OBJECT

public:
    explicit Serial(char* dev, int baud, int parity, int blocking, QObject *parent=nullptr);
    ~Serial();

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
public:  
    int initPort();
    void flushPort();
    int getData(char* );
    int sendData(char* );
    void closePort();
    int getBaud() const;
    void setBaud(int );
    int getBlocking() const;
    void setBlocking(int );
    char* getDev() const;
    void setDev(char* );
    int getParity() const;
    void setParity(int );
    bool isConnected() const;
};

#endif	/* SERIAL_H */

