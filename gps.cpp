#include "gps.h"
#include "QDebug"

Gps::Gps(QObject *parent) :
    QObject(parent)
{
    m_gpsMode = QGeoPositionInfoSource::AllPositioningMethods;
    m_gpsInterval = 1000;

    m_posSource = QGeoPositionInfoSource::createDefaultSource(this);

    if (m_posSource)
    {
        m_posSource->setPreferredPositioningMethods(m_gpsMode);
        m_posSource->setUpdateInterval(m_gpsInterval);

        connect(m_posSource,&QGeoPositionInfoSource::positionUpdated,this, &Gps::position_changed);
        m_posSource->startUpdates();
        qDebug() << "Gps started...";
    }
    else
        qDebug() << "No Gps found...";
}

Gps::~Gps()
{
    m_posSource->stopUpdates();
    delete m_posSource;
}

QGeoPositionInfo Gps::gpsPos() const
{
    return m_gpsPos;
}

void Gps::position_changed(QGeoPositionInfo gpsPos)
{
    m_gpsPos = gpsPos;
}
