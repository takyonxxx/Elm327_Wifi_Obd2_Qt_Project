#include "elmblesocket.h"
#include <QDebug>

void *ElmBleSocket::bleDiscoveryThread(void * this_ptr)
{
    qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");
    qRegisterMetaType<QString>("QString&");

    ElmBleSocket* obj_ptr = static_cast<ElmBleSocket*>(this_ptr);
    QMutexLocker locker(&obj_ptr->m_mutex);

    if (obj_ptr->localDevice->isValid())
    {
        //Turn BT on
        obj_ptr->localDevice->powerOn();

        // Make it visible to others
        obj_ptr->localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        obj_ptr->discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
        obj_ptr->discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
        obj_ptr->discoveryAgent->setLowEnergyDiscoveryTimeout(0);

        QObject::connect(obj_ptr->discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, obj_ptr, &ElmBleSocket::addDevice);
        QObject::connect(obj_ptr->discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, obj_ptr, &ElmBleSocket::scanFinished);

        if(obj_ptr->discoveryAgent)
        {
            QString msg{};

            msg.append("Scanning bluetooth devices");
            emit obj_ptr->stateChanged(msg);

            obj_ptr->discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
        }
    }
}


ElmBleSocket::ElmBleSocket(QObject *parent):localDevice(new QBluetoothLocalDevice)
{

}

ElmBleSocket::~ElmBleSocket()
{
    if(m_discoveryThread)
        pthread_cancel(m_discoveryThread);

    delete localDevice;
    delete discoveryAgent;
    delete socket;
}

void ElmBleSocket::run()
{
}

void ElmBleSocket::scan()
{
    pthread_create( &m_discoveryThread, nullptr, &ElmBleSocket::bleDiscoveryThread, this);
}

void ElmBleSocket::connectBle(const QBluetoothAddress & address)
{
    QString msg{};
    msg.append("Connecting to bluetooth : " + QString(address.toString()));
    emit stateChanged(msg);

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connect(socket, &QBluetoothSocket::connected, this, &ElmBleSocket::connected);
    connect(socket,  &QBluetoothSocket::disconnected, this, &ElmBleSocket::disconnected);
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));
    connect(socket,&QBluetoothSocket::stateChanged,this,&ElmBleSocket::stateChange);
    connect(socket,  &QBluetoothSocket::readyRead, this, &ElmBleSocket::readyRead);
    socket->connectToService(address, QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")), QIODevice::ReadWrite);
    socket->open(QIODevice::ReadWrite);
}

void ElmBleSocket::disconnectBle()
{
    if(socket->isOpen())
    {
        if(m_discoveryThread)
            pthread_cancel(m_discoveryThread);

        socket->disconnectFromService();
        socket->deleteLater();
        QString msg{};
        msg.append("DisConnected Ble");
        emit stateChanged(msg);
    }
}

bool ElmBleSocket::isConnected()
{
    return m_connected;
}


void ElmBleSocket::connected()
{
    m_connected = true;

    QString msg{};

    msg.append("Ble Socket connected\n");
    msg.append( socket->peerName()+ " : ");
    msg.append( socket->peerAddress().toString());
    emit stateChanged(msg);

    emit bleConnected();
}

void ElmBleSocket::disconnected()
{
    socket->deleteLater();
    m_connected = false;
    emit bleDisconnected();
    QString msg{};
    msg.append("Ble disconnected.");
    emit stateChanged(msg);
}


void ElmBleSocket::socketError(QBluetoothSocket::SocketError error)
{
    auto errorString = socket->errorString();
    emit stateChanged(errorString);
}

void ElmBleSocket::addDevice(const QBluetoothDeviceInfo &info)
{
      emit addBleDevice(info.address(), info.name());
}

void ElmBleSocket::scanFinished()
{
    QString msg{};
    msg.append("Ble Discovery finished.");
    emit stateChanged(msg);
}

bool ElmBleSocket::send(const QString &string)
{
    if(socket->isOpen())
    {
        connect(socket,&QBluetoothSocket::readyRead,this,&ElmBleSocket::readyRead);

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
        return socket->waitForBytesWritten(5000);
    }
    else
        return false;
}

bool ElmBleSocket::sendAsync(const QString &command)
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
        return socket->waitForBytesWritten(5000);
    }
    else
        return false;
}


QString ElmBleSocket::checkData()
{
    QString strData{};

    if (socket->waitForReadyRead(5000))
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

QString ElmBleSocket::readData(const QString &command)
{
    QString strData{};

    if(sendAsync(command))
    {
        QCoreApplication::processEvents();
        if (socket->waitForReadyRead(5000))
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

                emit dataReceived(strData);
                return strData;
            }
        }
    }
    return strData;
}

void ElmBleSocket::readyRead()
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
                disconnect(socket,&QBluetoothSocket::readyRead,this,&ElmBleSocket::readyRead);
                emit dataReceived(strData);
            }
        }
    }
}

QString ElmBleSocket::statetoString(QBluetoothSocket::SocketState socketState)
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

void ElmBleSocket::stateChange(QBluetoothSocket::SocketState socketState)
{
    QString state(statetoString(socketState).toStdString().c_str());
    emit stateChanged(state);
}
