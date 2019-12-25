#include "elmserialport.h"
#include <QDebug>

ElmSerialPort::ElmSerialPort(QObject *parent)
{
    QString sPort("/dev/pts/8");
    openSerialPort(sPort);
}

ElmSerialPort::~ElmSerialPort()
{
    mStop = true;
    delete serial;
}

bool ElmSerialPort::openSerialPort(const QString &sPort)
{
    serial = new Serial((char*)sPort.toStdString().c_str(), 38400, PARITY_8N1, 0);
    int result = serial->initPort();
    QThread::usleep(10000);
    return result;
}

void ElmSerialPort::closeSerialPort()
{
    if(serial->isConnected())
        serial->closePort();

    delete serial;
}

bool ElmSerialPort::isConnected() const
{
    if(!serial)
        return false;

    return serial->isConnected();
}

bool ElmSerialPort::send(const QString &command)
{
    if(serial->isConnected())
    {
        return serial->sendData((char*)command.toStdString().c_str());
    }
    else
        return false;
}

QString ElmSerialPort::readData(const QString &command)
{
    QString strData{};

    if(serial->isConnected() && send(command))
    {
        char* line = new char[32];
        *line = '\0';

        serial->getData(line);
        strData = QString(line);
        strData.remove("\r");
        strData.remove(">");
        strData = strData.trimmed()
                .simplified()
                .remove(QRegExp("[\\n\\t\\r]"))
                .remove(QRegExp("[^a-zA-Z0-9]+"));

        // Some of these look like errors that ought to be handled..
        strData.replace("?","");
        strData.replace(",","");
        qDebug() << strData;
    }

    return strData;
}

void ElmSerialPort::connected()
{

}

void ElmSerialPort::disconnected()
{

}

void ElmSerialPort::readyRead()
{

}

void ElmSerialPort::socketError()
{
}
