#include "serialmanager.h"
#include <QDebug>

SerialManager* SerialManager::theInstance_ = nullptr;

SerialManager *SerialManager::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new SerialManager();
    }
    return theInstance_;
}

SerialManager::SerialManager()
{
    serial = new QSerialPort();
}

SerialManager::~SerialManager()
{
    delete serial;
}

void SerialManager::connect(const QString &port)
{
   if(serial->isOpen())
        serial->close();

    serial->setPortName(port);

    QObject::connect(serial,SIGNAL(readyRead()),this, SLOT(readyRead()));

    if(!serial->open(QIODevice::ReadWrite))
    {
        m_connected = true;
        qDebug() << "Serial started " << serial->isOpen();
    }
    else
    {
        m_connected = false;
        qDebug() << "Fail : " + serial->errorString();
    }
}

void SerialManager::disconnect()
{
    if(serial->isOpen())
    {
        serial->close();
        m_connected = false;
    }
}

bool SerialManager::isConnected()
{
    return m_connected;
}

bool SerialManager::send(const QString &string)
{
    QByteArray dataToSend;
    dataToSend.append(string);

    if(serial->isOpen())
    {
        serial->write(dataToSend);
        qDebug() << "send";
    }
    else
        return false;
    return true;
}

void SerialManager::readyRead()
{
    QByteArray lTmpBA;
    lTmpBA = serial->readAll();

    qDebug() << "Received data: " << lTmpBA;
}
