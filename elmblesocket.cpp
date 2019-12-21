#include "elmblesocket.h"
#include <QDebug>

void *ElmBleSocket::discoveryThread(void * this_ptr)
{
    qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");

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


ElmBleSocket::ElmBleSocket(const QBluetoothAddress &, QObject *parent)
{

}

ElmBleSocket::~ElmBleSocket()
{
    if(m_discoveryThread)
        pthread_cancel(m_discoveryThread);

    delete localDevice;
    delete discoveryAgent;
}

void ElmBleSocket::run()
{
}

void ElmBleSocket::scan()
{
    pthread_create( &m_discoveryThread, nullptr, &ElmBleSocket::discoveryThread, this);
}


void ElmBleSocket::addDevice(const QBluetoothDeviceInfo &info)
{

}

void ElmBleSocket::scanFinished()
{
    qDebug() << "Ble Discovery finished.";
    QString msg{};
    msg.append("Ble Discovery finished.");
    emit stateChanged(msg);
}
