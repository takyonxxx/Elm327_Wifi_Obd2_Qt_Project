#include "connectionmanager.h"

ConnectionManager* ConnectionManager::theInstance_ = nullptr;

ConnectionManager *ConnectionManager::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new ConnectionManager();
    }
    return theInstance_;
}

ConnectionManager::ConnectionManager(QObject *parent)
{
    mElmTcpSocket = new ElmTcpSocket(this);
    if(mElmTcpSocket)
    {
        connect(mElmTcpSocket,&ElmTcpSocket::tcpConnected,this, &ConnectionManager::conConnected);
        connect(mElmTcpSocket,&ElmTcpSocket::tcpDisconnected,this,&ConnectionManager::conDisconnected);
        connect(mElmTcpSocket,&ElmTcpSocket::dataReceived,this,&ConnectionManager::conDataReceived);
        connect(mElmTcpSocket, &ElmTcpSocket::stateChanged, this, &ConnectionManager::conStateChanged);
    }

    mElmBleSocket = new ElmBleSocket(this);
    if(mElmBleSocket)
    {
        connect(mElmBleSocket,&ElmBleSocket::bleConnected,this, &ConnectionManager::conConnected);
        connect(mElmBleSocket,&ElmBleSocket::bleDisconnected,this,&ConnectionManager::conDisconnected);
        connect(mElmBleSocket,&ElmBleSocket::dataReceived,this,&ConnectionManager::conDataReceived);
        connect(mElmBleSocket, &ElmBleSocket::addBleDevice, this, &ConnectionManager::conAddBleDevice);
        connect(mElmBleSocket, &ElmBleSocket::stateChanged, this, &ConnectionManager::conStateChanged);
    }

}

bool ConnectionManager::send(const QString &command)
{
    if(cType == ConnectionType::Wifi)
    {
        if(mElmTcpSocket)
        {
            return  mElmTcpSocket->send(command);
        }
    }
    else if(cType == ConnectionType::BlueTooth)
    {
        if(mElmBleSocket)
        {
            return mElmBleSocket->send(command);
        }
    }

    return false;
}

QString ConnectionManager::readData(const QString &command)
{
    if(cType == ConnectionType::Wifi)
    {
        if(mElmTcpSocket)
        {
            return  mElmTcpSocket->readData(command);
        }
    }
    else if(cType == ConnectionType::BlueTooth)
    {
        if(mElmBleSocket)
        {
            return mElmBleSocket->readData(command);
        }
    }

    return QString();
}

void ConnectionManager::disConnectElm()
{
    if(mElmTcpSocket && mElmTcpSocket->isConnected())
    {
        mElmTcpSocket->disconnectTcp();
    }

    if(mElmBleSocket && mElmBleSocket->isConnected())
    {
        mElmBleSocket->stopScan();
        mElmBleSocket->disconnectBle();
    }
}

void ConnectionManager::connectElm()
{
    if(cType == ConnectionType::None)
        return;

    disConnectElm();

    m_settingsManager = SettingsManager::getInstance();

    if(cType == ConnectionType::Wifi)
    {
        if(mElmTcpSocket)
        {
            QString ip = m_settingsManager->getWifiIp();
            quint16 port = m_settingsManager->getWifiPort();
            mElmTcpSocket->connectTcp(ip, port);
        }
    }
    else if(cType == ConnectionType::BlueTooth)
    {
        if(mElmBleSocket)
        {
            auto bleAddress = m_settingsManager->getBleAddress();
            mElmBleSocket->connectBle(bleAddress);
        }
    }
}

void ConnectionManager::setCType(const ConnectionType &value)
{
    cType = value;

    if(cType == ConnectionType::BlueTooth)
    {
        startScanBle();
    }
}

void ConnectionManager::startScanBle()
{
    if(mElmBleSocket)
    {
        mElmBleSocket->startScan();
    }
}

void ConnectionManager::stopScanBle()
{
    if(mElmBleSocket)
    {
        mElmBleSocket->stopScan();
    }
}

ConnectionType ConnectionManager::getCType() const
{
    return cType;
}

bool ConnectionManager::isConnected() const
{
    return m_connected;
}

void ConnectionManager::conConnected()
{
    m_connected = true;
    emit connected();
}

void ConnectionManager::conDisconnected()
{
    m_connected = false;
    emit disconnected();
}

void ConnectionManager::conDataReceived(QString data)
{
    emit dataReceived(data);
}

void ConnectionManager::conStateChanged(QString state)
{
    emit stateChanged(state);
}

void ConnectionManager::conAddBleDevice(const QBluetoothAddress& address, const QString& name)
{
    if(cType == ConnectionType::BlueTooth)
        emit addBleDevice(address, name);
}
