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

private slots:
    void openSerialPort(const QString &);
    void closeSerialPort();
    bool send(const QString &);
    QString readData(const QString &);
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
