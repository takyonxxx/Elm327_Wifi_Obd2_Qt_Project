#ifndef ELMSERIALPORT_H
#define ELMSERIALPORT_H

#include <QObject>
#include <QThread>
#include <iostream>
#include "serial.h"

using namespace std;

class ElmSerialPort
{
public:
    explicit ElmSerialPort(QObject *parent=nullptr);
    ~ElmSerialPort();
    Serial* serial{};

    bool openSerialPort(const QString &);
    void closeSerialPort();
    bool send(const QString &);
    QString readData(const QString &);
    bool isConnected() const;

private slots:
    void socketError();
    void connected();
    void disconnected();
    void readyRead();
    bool mStop{false};
signals:
    void dataReceived(QString &);
    void stateChanged(QString &);
    void serialConnected();
    void serialDisconnected();
};

#endif // ELMSERIALPORT_H
