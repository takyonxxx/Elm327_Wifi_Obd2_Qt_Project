#include "bluetoothmanager.h"

#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

BluetoothManager* BluetoothManager::theInstance_ = nullptr;

BluetoothManager *BluetoothManager::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new BluetoothManager();
    }
    return theInstance_;
}

void *BluetoothManager::discoveryThread(void * this_ptr)
{
    qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");

    BluetoothManager* obj_ptr = static_cast<BluetoothManager*>(this_ptr);
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

        QObject::connect(obj_ptr->discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, obj_ptr, &BluetoothManager::addDevice);
        QObject::connect(obj_ptr->discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, obj_ptr, &BluetoothManager::scanFinished);

        if(obj_ptr->discoveryAgent)
        {
            QString msg{};

            msg.append("Scanning bluetooth devices");
            emit obj_ptr->stateChanged(msg);

            obj_ptr->discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
        }
    }

}

void BluetoothManager::scan()
{
    pthread_create( &scanThread, nullptr, &BluetoothManager::discoveryThread, this);
}

BluetoothManager::BluetoothManager():localDevice(new QBluetoothLocalDevice)
{

}

BluetoothManager::~BluetoothManager()
{
    if(scanThread)
        pthread_cancel(scanThread);

    delete localDevice;
    delete discoveryAgent;
    delete socket;
}

void BluetoothManager::connectBle(const QBluetoothAddress &address)
{
    QString msg{};
    msg.append("Connecting to bluetooth : " + QString(address.toString()));
    emit stateChanged(msg);

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    QObject::connect(socket, &QBluetoothSocket::connected, this, &BluetoothManager::connected);
    QObject::connect(socket,  &QBluetoothSocket::disconnected, this, &BluetoothManager::disconnected);
    QObject::connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));
    QObject:: connect(socket,  &QBluetoothSocket::readyRead, this, &BluetoothManager::readyRead);
    socket->connectToService(address, QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")), QIODevice::ReadWrite);
    socket->open(QIODevice::ReadWrite);
}

void BluetoothManager::disconnectBle()
{
    if(socket->isOpen())
    {
        if(scanThread)
            pthread_cancel(scanThread);

        socket->disconnectFromService();
        socket->deleteLater();
    }
}

bool BluetoothManager::isConnected()
{
    return m_connected;
}

void BluetoothManager::connected()
{
    m_connected = true;

    QString msg{};

    msg.append("Ble Socket connected\n");
    msg.append( socket->peerName()+ " : ");
    msg.append( socket->peerAddress().toString());
    emit stateChanged(msg);

    emit bleConnected();
}

void BluetoothManager::disconnected()
{    
    socket->deleteLater();
    m_connected = false;
    emit bleDisconnected();
    QString msg{};
    msg.append("Ble disconnected.");
    emit stateChanged(msg);
}


void BluetoothManager::socketError(QBluetoothSocket::SocketError error)
{
    qDebug() << "Socket error: " << socket->errorString();

    QString msg{};

    msg.append("Socket error : " + socket->errorString());
    emit stateChanged(msg);
}

void BluetoothManager::addDevice(const QBluetoothDeviceInfo &info)
{
    if(info.name().contains("OBD") || info.name().contains("ECU") || info.name().contains("ELM") || info.name().contains("SCAN"))
        emit addDeviceToList(info.address(), info.name());
}

void BluetoothManager::scanFinished()
{
    qDebug() << "Ble Discovery finished.";
    QString msg{};
    msg.append("Ble Discovery finished.");
    emit stateChanged(msg);
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

void BluetoothManager::readyRead()
{
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
}

bool BluetoothManager::send(const QString &string)
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
        return true;
    }
    else
        return false;
}
