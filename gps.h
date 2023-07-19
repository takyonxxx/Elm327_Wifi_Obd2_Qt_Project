#ifndef GPS_H
#define GPS_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>

struct Constants {
    // global
    constexpr static const double g = 9.80665; // meters per second^2
    constexpr static const double gasConstant = 8.3144598; // Joules per mole per Kelvin
    constexpr static const double dryAirMolarMass = 0.0289644; // kilograms per mole
    constexpr static const double dryAirSpecificGasConstant = 287.058; // Joule kilogram^-1 Kelvin^-1

    // U.S. Standard Atmosphere model (temperature lapse rates in accordance to International Standard Atmosphere model)
    constexpr static const double layerStart0 = 0; // meters
    constexpr static const double layerStart1 = 11000; // meters
    constexpr static const double layerStart2 = 20000; // meters
    constexpr static const double layerStart3 = 32000; // meters
    constexpr static const double staticPressure0 = 101325.00; // Pascals
    constexpr static const double staticPressure1 = 22632.10; // Pascals
    constexpr static const double staticPressure2 = 5474.89; // Pascals
    constexpr static const double staticPressure3 = 868.02; // Pascals
    constexpr static const double massDensity0 = 1.2250; // kilograms per cubic meter
    constexpr static const double massDensity1 = 0.36391; // kilograms per cubic meter
    constexpr static const double massDensity2 = 0.08803; // kilograms per cubic meter
    constexpr static const double massDensity3 = 0.01322; // kilograms per cubic meter
    constexpr static const double standardTemperature0 = 288.15; // Kelvin
    constexpr static const double standardTemperature1 = 216.65; // Kelvin
    constexpr static const double standardTemperature2 = 216.65; // Kelvin
    constexpr static const double standardTemperature3 = 228.65; // Kelvin
    constexpr static const double standardTemperatureLapseRate0 = -0.0065; // Kelvin per meter
    constexpr static const double standardTemperatureLapseRate1 = 0.0; // Kelvin per meter
    constexpr static const double standardTemperatureLapseRate2 = 0.001; // Kelvin per meter
    constexpr static const double standardTemperatureLapseRate3 = 0.0028; // Kelvin per meter
};


class Gps : public QObject
{
    Q_OBJECT
public:
    explicit Gps(QObject *parent=nullptr);
    ~Gps();

    static bool IsNan( float value )
    {
        return ((*(uint*)&value) & 0x7fffffff) > 0x7f800000;
    }

    // The Barometric formula for pressure
    // returns barometric pressure (in Pascals) at a given altitude (currently valid up to 47km (stratopause))
    // ref: https://en.wikipedia.org/wiki/Barometric_formula

    inline static double barometricPressure(double altitude) {
        if (altitude < Constants::layerStart1) {
            double power = Constants::g * Constants::dryAirMolarMass / (Constants::gasConstant * Constants::standardTemperatureLapseRate0);
            return Constants::staticPressure0 * pow(Constants::standardTemperature0 / (Constants::standardTemperature0 + Constants::standardTemperatureLapseRate0 * (altitude - Constants::layerStart0)), power);
        } else if (altitude < Constants::layerStart2) { // stratosphere
            // this equation is different from the others because the temperature lapse rate is 0
            return Constants::staticPressure1 * exp(-Constants::g * Constants::dryAirMolarMass * (altitude - Constants::layerStart1) / (Constants::gasConstant * Constants::standardTemperature1));
        } else if (altitude < Constants::layerStart3) {
            double power = Constants::g * Constants::dryAirMolarMass / (Constants::gasConstant * Constants::standardTemperatureLapseRate2);
            return Constants::staticPressure2 * pow(Constants::standardTemperature0 / (Constants::standardTemperature2 + Constants::standardTemperatureLapseRate2 * (altitude - Constants::layerStart2)), power);
        } else { // NOTE: there are more layers, but we aren't worrying about them
            double power = Constants::g * Constants::dryAirMolarMass / (Constants::gasConstant * Constants::standardTemperatureLapseRate3);
            return Constants::staticPressure3 * pow(Constants::standardTemperature3 / (Constants::standardTemperature3 + Constants::standardTemperatureLapseRate3 * (altitude - Constants::layerStart3)), power);
        }
    }

    // The Barometric formula for density
    // returns density (in kilograms per cubic meter) at a given altitude (currently valid up to 47km (stratopause))
    // ref: https://en.wikipedia.org/wiki/Barometric_formula
    inline static double barometricDensity(double altitude) {
        if (altitude < Constants::layerStart1) {
            double power = 1 + Constants::g * Constants::dryAirMolarMass / (Constants::gasConstant * Constants::standardTemperatureLapseRate0);
            return Constants::massDensity0 * pow(Constants::standardTemperature0 / (Constants::standardTemperature0 + Constants::standardTemperatureLapseRate0 * (altitude - Constants::layerStart0)), power);
        } else if (altitude < Constants::layerStart2) { // stratosphere
            // this equation is different from the others because the temperature lapse rate is 0
            return Constants::massDensity1 * exp(-Constants::g * Constants::dryAirMolarMass * (altitude - Constants::layerStart1) / (Constants::gasConstant * Constants::standardTemperature1));
        } else if (altitude < Constants::layerStart3) {
            double power = 1 + Constants::g * Constants::dryAirMolarMass / (Constants::gasConstant * Constants::standardTemperatureLapseRate2);
            return Constants::massDensity2 * pow(Constants::standardTemperature0 / (Constants::standardTemperature2 + Constants::standardTemperatureLapseRate2 * (altitude - Constants::layerStart2)), power);
        } else { // NOTE: there are more layers, but we aren't worrying about them
            double power = 1 + Constants::g * Constants::dryAirMolarMass / (Constants::gasConstant * Constants::standardTemperatureLapseRate3);
            return Constants::massDensity3 * pow(Constants::standardTemperature3 / (Constants::standardTemperature3 + Constants::standardTemperatureLapseRate3 * (altitude - Constants::layerStart3)), power);
        }
    }

    QGeoPositionInfo gpsPos() const;

private:
    QGeoPositionInfoSource *m_posSource;
    QGeoPositionInfo m_gpsPos;
    QGeoPositionInfoSource::PositioningMethod m_gpsMode;
    int m_gpsInterval;
public slots:
    void position_changed(QGeoPositionInfo gpsPos);

};

#endif // GPS_H
