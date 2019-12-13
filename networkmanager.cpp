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
        connect(socket,&QTcpSocket::readyRead,this,&NetworkManager::readed);
        connect(socket,&QTcpSocket::stateChanged,this,&NetworkManager::stateChange);
        connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(error(QAbstractSocket::SocketError)));
    }
}

NetworkManager::~NetworkManager()
{
    delete socket;
}

void NetworkManager::send(QString &string)
{
    if(socket->isOpen())
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        out << string.trimmed();
        out << "\r";
        socket->write(block);
    }
}

void NetworkManager::connected()
{    
    m_connected = true;
    emit wifiConnected();
}

void NetworkManager::disconnected()
{
    m_connected = false;
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

QString NetworkManager::readData()
{
    auto received = socket->readAll();
    auto strData = QString::fromStdString(received.toStdString());
    return strData;
}

void NetworkManager::readed()
{
    auto received = socket->readAll();
    auto strData = QString::fromStdString(received.toStdString());

    QString s_received = strData
            .trimmed()
            .simplified()
            .remove(QRegExp("[\\n\\t\\r]"))
            .remove(QRegExp("[^a-zA-Z0-9]+"));

    s_received.replace(">","");

    // Some of these look like errors that ought to be handled..
    s_received.replace("STOPPED","");
    //s_received.replace("SEARCHING","");
    //s_received.replace("NO DATA","");
    s_received.replace("?","");
    s_received.replace(",","");

    emit dataReceived(s_received);

    QString s_hex_received = received.toHex();
    emit dataHexReceived(s_hex_received);
}

void NetworkManager::error(QAbstractSocket::SocketError serr)
{
    auto errorString = socket->errorString();
    emit errorAccrued(errorString);
}

void NetworkManager::connectWifi(const QString &ip, int port)
{
    if(!socket)return;

    socket->connectToHost(ip, port);
    socket->waitForConnected(1000);
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
