#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QIODevice>
#include <QMessageBox>

class SerialManager : public QObject
{
    Q_OBJECT

public:
    SerialManager();
    ~SerialManager();

    static SerialManager* getInstance();
    void connect(const QString &);
    void disconnect();
    bool isConnected();
    bool send(const QString &);

private slots:
    //void connected();
    //void disconnected();
    void readyRead();

private:
    QSerialPort*serial;
    bool m_connected{false};
    static SerialManager* theInstance_;
};

#endif // SERIALMANAGER_H
