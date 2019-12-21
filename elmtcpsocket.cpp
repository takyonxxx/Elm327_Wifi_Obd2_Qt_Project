#include "elmtcpsocket.h"
#include <QDebug>

ElmTcpSocket* ElmTcpSocket::theInstance_ = nullptr;

ElmTcpSocket *ElmTcpSocket::getInstance()
{
    return theInstance_;
}

ElmTcpSocket::ElmTcpSocket(const QString &ip, const quint16 &port, QObject *parent)
{
    theInstance_ = this;
    this->ip = ip;
    this->port = port;
    this->socket = new QTcpSocket(this);
    if(socket)
    {
        connect(socket,&QTcpSocket::connected,this, &ElmTcpSocket::connected);
        connect(socket,&QTcpSocket::disconnected,this,&ElmTcpSocket::disconnected);
        //connect(socket,&QTcpSocket::readyRead,this,&ElmTcpSocket::readyRead);
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(error(QAbstractSocket::SocketError)));
    }
}

ElmTcpSocket::~ElmTcpSocket()
{
    if(socket)
        delete socket;
}

void ElmTcpSocket::run()
{
    exec();
}

void ElmTcpSocket::connectTcp()
{
    if(socket)
    {
        QString msg{};
        msg.append("Connecting to Wifi " + ip + " : " + QString::number(port));
        emit stateChanged(msg);

        socket->connectToHost(ip, port);
        socket->waitForConnected(3000);
    }
}

void ElmTcpSocket::disconnectTcp()
{
    if(socket)
    {
        socket->close();
        QString msg{};
        msg.append("DisConnected Wifi " + ip + " : " + QString::number(port));
        emit stateChanged(msg);
    }
}

bool ElmTcpSocket::isConnected()
{
    return m_connected;
}


bool ElmTcpSocket::send(const QString &command)
{
    if(socket->isOpen())
    {
        connect(socket,&QTcpSocket::readyRead,this,&ElmTcpSocket::readyRead);

        QByteArray dataToSend = command.toUtf8();

        if (command.isEmpty())
        {
            // If toWrite is empty then just send a CR char.
            dataToSend += ('\r');
        }
        else
        {
            // Check for CR at end.
            if (dataToSend[dataToSend.size()] != '\r')
                dataToSend += '\r';
        }

        socket->write(dataToSend);
        return socket->waitForBytesWritten();
    }
    else
        return false;
}

bool ElmTcpSocket::sendAsync(const QString &command)
{
    if(socket->isOpen())
    {
        QByteArray dataToSend = command.toUtf8();

        if (command.isEmpty())
        {
            // If toWrite is empty then just send a CR char.
            dataToSend += ('\r');
        }
        else
        {
            // Check for CR at end.
            if (dataToSend[dataToSend.size()] != '\r')
                dataToSend += '\r';
        }

        socket->write(dataToSend);
        return socket->waitForBytesWritten();
    }
    else
        return false;
}

void ElmTcpSocket::readyRead()
{
    QString strData{};
    QByteArray data = socket->readAll();
    byteblock += data;

    strData = QString::fromStdString(byteblock.toStdString());

    if(strData.contains("\r"))
    {
        if(!strData.isEmpty())
        {
            byteblock.clear();
            strData.remove("\r");
            strData.remove(">");

            strData = strData.trimmed()
                    .simplified()
                    .remove(QRegExp("[\\n\\t\\r]"))
                    .remove(QRegExp("[^a-zA-Z0-9]+"));

            // Some of these look like errors that ought to be handled..
            strData.replace("?","");
            strData.replace(",","");
            if(!strData.isEmpty())
            {
                disconnect(socket,&QTcpSocket::readyRead,this,&ElmTcpSocket::readyRead);
                emit dataReceived(strData);
            }
        }
    }
}

void ElmTcpSocket::connected()
{
    m_connected = true;
    emit tcpConnected();
}

void ElmTcpSocket::disconnected()
{
    m_connected = false;
    QString msg{};
    msg.append("Wifi disconnected.");
    emit stateChanged(msg);
    emit tcpDisconnected();
}

QString ElmTcpSocket::checkData()
{
    QString strData{};

    if (socket->waitForReadyRead())
    {
        QByteArray data = socket->readAll();
        byteblock += data;

        strData = QString::fromStdString(byteblock.toStdString());
        if(strData.contains("\r"))
        {
            byteblock.clear();
            strData.remove("\r");
            strData.remove(">");

            strData = strData.trimmed()
                    .simplified()
                    .remove(QRegExp("[\\n\\t\\r]"))
                    .remove(QRegExp("[^a-zA-Z0-9]+"));

            // Some of these look like errors that ought to be handled..
            strData.replace("?","");
            strData.replace(",","");
            emit dataReceived(strData);
            return strData;
        }
    }
    return strData;
}

QString ElmTcpSocket::readData(const QString &command)
{
    QString strData{};

    if(sendAsync(command))
    {
        QCoreApplication::processEvents();
        if (socket->waitForReadyRead())
        {
            QByteArray data = socket->readAll();
            byteblock += data;

            strData = QString::fromStdString(byteblock.toStdString());
            if(strData.contains("\r"))
            {
                byteblock.clear();
                strData.remove("\r");
                strData.remove(">");

                strData = strData.trimmed()
                        .simplified()
                        .remove(QRegExp("[\\n\\t\\r]"))
                        .remove(QRegExp("[^a-zA-Z0-9]+"));

                // Some of these look like errors that ought to be handled..
                strData.replace("?","");
                strData.replace(",","");
                if(!strData.isEmpty())
                {
                    if(strData.contains("SEARCHING"))
                    {
                        QCoreApplication::processEvents();
                        return  checkData();
                    }
                }

                disconnect(socket,&QTcpSocket::readyRead,this,&ElmTcpSocket::readyRead);
                emit dataReceived(strData);
                return strData;
            }
        }
    }
    return strData;
}

void ElmTcpSocket::error(QAbstractSocket::SocketError)
{

}
