#include "gps.h"

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
    }
}

Gps::~Gps()
{
    m_posSource->stopUpdates();
    delete m_posSource;
}

double Gps::altitude() const
{
    return m_altitude;
}

void Gps::setAltitude(double newAltitude)
{
    m_altitude = newAltitude;
}

void Gps::position_changed(QGeoPositionInfo gpsPos)
{
    m_gpsPos = gpsPos;

    if(m_gpsPos.isValid())
    {
        auto m_coord = gpsPos.coordinate();

//        auto groundspeed = 3.6 * m_gpsPos.attribute(QGeoPositionInfo::GroundSpeed);
//        if(IsNan((float)groundspeed)) groundspeed = 0;

//        auto heading = m_gpsPos.attribute(QGeoPositionInfo::Direction);
//        if(IsNan((float)heading))  heading = 0;

        auto altitude = m_coord.altitude();
        if(IsNan((float)altitude))  altitude = 0;

        setAltitude(altitude);
    }
}
