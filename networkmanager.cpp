#include "networkmanager.h"
#include <QDebug>

NetworkManager* NetworkManager::theInstance_ = nullptr;

NetworkManager *NetworkManager::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new NetworkManager();
    }
    return theInstance_;
}

NetworkManager::NetworkManager()
{
    socket = new QTcpSocket(this);

    if(socket)
    {
        connect(socket,&QTcpSocket::connected,this, &NetworkManager::connected);
        connect(socket,&QTcpSocket::disconnected,this,&NetworkManager::disconnected);
        connect(socket,&QTcpSocket::readyRead,this,&NetworkManager::readyRead);
        connect(socket,&QTcpSocket::stateChanged,this,&NetworkManager::stateChange);
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(error(QAbstractSocket::SocketError)));
    }
}

NetworkManager::~NetworkManager()
{
    socket->disconnectFromHost();
    socket->waitForDisconnected();
    delete socket;
}

void NetworkManager::connected()
{    
    m_connected = true;
    emit wifiConnected();
}

void NetworkManager::disconnected()
{
    m_connected = false;
    QString msg{};
    msg.append("Wifi disconnected.");
    emit stateChanged(msg);
    emit wifiDisconnected();
}

QString NetworkManager::statetoString(QAbstractSocket::SocketState socketState)
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

void NetworkManager::stateChange(QAbstractSocket::SocketState socketState)
{
    QString state(statetoString(socketState).toStdString().c_str());
    emit stateChanged(state);
}

QString NetworkManager::readData(QString &command)
{
    customRead = true;
    QString strData{};
    if(send(command))
    {
        while (socket->bytesAvailable() > 0)
        {
            if (!socket->waitForReadyRead())
            {
                qDebug() << "waitForReadyRead() timed out";
                return strData;
            }

            auto received = socket->readAll();
            auto strData = QString::fromStdString(received.toStdString());
        }
    }
    customRead = false;
    return strData;
}


bool NetworkManager::send(const QString &string)
{
    if(socket->isOpen())
    {
        QByteArray dataToSend = string.toUtf8();

        if (string.isEmpty())
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

void NetworkManager::readyRead()
{   
    if(customRead)
        return;

    while (!socket->atEnd()) {
        QByteArray data = socket->read(socket->bytesAvailable());
        byteblock += data;
    }

    auto strData = QString::fromStdString(byteblock.toStdString());
    if(strData.contains("\r"))
    {
        strData.remove("\r");
        strData.remove(">");
        strData.remove("atrv").remove("ATRV");
        if(!strData.isEmpty())
        {
            strData = strData.trimmed()
                    .simplified()
                    .remove(QRegExp("[\\n\\t\\r]"))
                    .remove(QRegExp("[^a-zA-Z0-9]+"));
            // Some of these look like errors that ought to be handled..
            strData.replace("STOPPED","");
            strData.replace("SEARCHING","");
            strData.replace("NODATA","");
            strData.replace("OK","");
            strData.replace("?","");
            strData.replace(",","");

            if(!strData.isEmpty())
            {
                emit dataReceived(strData);
            }
            byteblock.clear();
        }
    }
    //emit dataBytesReceived(strData);
}

void NetworkManager::error(QAbstractSocket::SocketError serr)
{
    auto errorString = socket->errorString();
    emit errorAccrued(errorString);
}

void NetworkManager::connectWifi(const QString &ip, int port)
{
    if(!socket)return;

    QString msg{};
    msg.append("Connecting to Wifi " + ip + " : " + port);
    emit stateChanged(msg);

    socket->connectToHost(ip, port);
    socket->waitForConnected(3000);
}

void NetworkManager::disconnectWifi()
{
    if(!socket)return;

    socket->close();
}

bool NetworkManager::isConnected()
{
    return m_connected;
}
