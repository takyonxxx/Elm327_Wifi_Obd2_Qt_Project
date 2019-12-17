#include "bluetoothmanager.h"

#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <QTime>
#include <QCoreApplication>
#include <QDebug>

BluetoothManager* BluetoothManager::theInstance_ = nullptr;

BluetoothManager *BluetoothManager::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new BluetoothManager();
    }
    return theInstance_;
}

void BluetoothManager::scan()
{
    if(discoveryAgent)
    {
        QString msg{};
        msg.append("Scaning Bluetooth devices..");
        emit stateChanged(msg);
        discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    }
}

BluetoothManager::BluetoothManager():localDevice(new QBluetoothLocalDevice)
{
    if (localDevice->isValid())
    {
        qDebug() << "Bluetooth is available on this device";

        //Turn BT on
        localDevice->powerOn();

        // Make it visible to others
        localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
        discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
        discoveryAgent->setLowEnergyDiscoveryTimeout(5000);

        QObject::connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothManager::addDevice);
        QObject::connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothManager::scanFinished);
    }
}

BluetoothManager::~BluetoothManager()
{
    delete localDevice;
    delete discoveryAgent;
    delete socket;
}

void BluetoothManager::connectBle(const QBluetoothAddress &address)
{
    if(socket)
        return;

    QString msg{};
    msg.append("Connecting to : " + QString(address.toString()));
    emit stateChanged(msg);
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    QObject::connect(socket, &QBluetoothSocket::connected, this, &BluetoothManager::connected);
    QObject::connect(socket,  &QBluetoothSocket::disconnected, this, &BluetoothManager::disconnected);
    QObject::connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));
    QObject:: connect(socket,  &QBluetoothSocket::readyRead, this, &BluetoothManager::readyRead);
    socket->connectToService(address, QBluetoothUuid(QString("00001101-0000-1000-8000-00805F9B34FB")), QIODevice::ReadWrite);
    socket->open(QIODevice::ReadWrite);
    socket->openMode();
}

void BluetoothManager::disconnectBle()
{
    if(socket->isOpen())
    {
        socket->disconnectFromService();
        socket->deleteLater();
        QString msg{};
        msg.append("Ble disconnected.");
        emit stateChanged(msg);
    }
}

bool BluetoothManager::isConnected()
{
    return m_connected;
}

void BluetoothManager::connected()
{
    m_connected = true;
    emit bleConnected();

    QString msg{};

    msg.append("Ble Socket connected\n");
    msg.append( "Local:\n");
    msg.append( socket->localName() + "\n");
    msg.append( socket->localAddress().toString()+ "\n");
    msg.append( socket->localPort() + "\n");
    msg.append( "Peer:\n");
    msg.append( socket->peerName()+ "\n");
    msg.append( socket->peerAddress().toString()+ "\n");
    msg.append( socket->peerPort() + "\n");
    emit stateChanged(msg);
}

void BluetoothManager::disconnected()
{
    emit bleDisconnected();
    socket->deleteLater();
    m_connected = false;
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
    //if(info.name().contains("OBD") || info.name().contains("ECU") || info.name().contains("ELM") || info.name().contains("SCAN"))
    if(!info.name().isEmpty())
        emit addDeviceToList(info.address(), info.name());
}

void BluetoothManager::scanFinished()
{
    qDebug() << "Ble Discovery finished.";
    QString msg{};
    msg.append("Ble Discovery finished.");
    emit stateChanged(msg);
}

void BluetoothManager::readyRead()
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
    s_received.replace("SEARCHING","");
    s_received.replace("NO DATA","");
    s_received.replace("NODATA","");
    s_received.replace("OK","");
    s_received.replace("?","");
    s_received.replace(",","");

    emit dataReceived(s_received);
}

bool BluetoothManager::send(const QString &string)
{
    if(socket->isOpen())
    {
        QByteArray dataToSend = string.toUtf8();

        if (string.isEmpty())
        {
            // If toWrite is empty then just send a CR char.
            dataToSend += ('\r\r');
        }
        else
        {
            // Check for CR at end.
            if (dataToSend[dataToSend.size()] != '\r')
                dataToSend += '\r\r';
        }

        socket->write(dataToSend);
        return true;
    }
    else
        return false;
}
