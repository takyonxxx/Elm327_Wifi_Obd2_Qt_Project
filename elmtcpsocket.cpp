#include "elmtcpsocket.h"
#include <QDebug>

ElmTcpSocket::ElmTcpSocket(QObject *parent)
{

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

void ElmTcpSocket::connectTcp(const QString &ip, const quint16 &port)
{
    QString msg{};
    msg.append("Connecting to Wifi " + ip + " : " + QString::number(port));
    emit stateChanged(msg);

    this->socket = new QTcpSocket(this);
    if(socket)
    {
        connect(socket,&QTcpSocket::connected,this, &ElmTcpSocket::connected);
        connect(socket,&QTcpSocket::disconnected,this,&ElmTcpSocket::disconnected);
        connect(socket,&QTcpSocket::stateChanged,this,&ElmTcpSocket::stateChange);
        //connect(socket,&QTcpSocket::readyRead,this,&ElmTcpSocket::readyRead);
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(socketError(QAbstractSocket::SocketError)));
        socket->connectToHost(ip, port);
        socket->waitForConnected(3000);
    }
}

void ElmTcpSocket::disconnectTcp()
{
    if(socket)
    {
        socket->close();
        socket->deleteLater();
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
    emit tcpDisconnected();
}

QString ElmTcpSocket::checkData()
{
    QString strData{};
    QCoreApplication::processEvents(QEventLoop::AllEvents);

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
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if (socket->waitForReadyRead())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
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

QString ElmTcpSocket::statetoString(QAbstractSocket::SocketState socketState)
{
    QString statestring;
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState : statestring="The socket is not connected";
        break;
    case QAbstractSocket::HostLookupState : statestring="The socket is performing a host name lookup";
        break;
    case QAbstractSocket::ConnectingState : statestring="The socket has started establishing a connection";
        break;
    case QAbstractSocket::ConnectedState : statestring="Connection is established";
        break;
    case QAbstractSocket::BoundState : statestring="The socket is bound to an address and port";
        break;
    case QAbstractSocket::ClosingState : statestring="The socket is about to close";
        break;
    case QAbstractSocket::ListeningState : statestring="Listening state";
        break;
    default: statestring="Unknown state";
        break;
    }
    return statestring;
}

void ElmTcpSocket::stateChange(QAbstractSocket::SocketState socketState)
{
    QString state(statetoString(socketState).toStdString().c_str());
    emit stateChanged(state);
}

void ElmTcpSocket::socketError(QAbstractSocket::SocketError)
{
    auto errorString = socket->errorString();
    emit stateChanged(errorString);
}
