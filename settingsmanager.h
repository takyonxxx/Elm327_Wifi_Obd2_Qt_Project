#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QtCore>
#include <QSettings>
#include <QBluetoothAddress>

class SettingsManager
{
public:
    SettingsManager();

    static SettingsManager* getInstance();

    void loadSettings();
    void saveSettings();

    void setEngineDisplacement(unsigned int value);
    unsigned int getEngineDisplacement() const;

    void setWifiIp(const QString &value);
    QString getWifiIp() const;

    void setWifiPort(const quint16 &value);
    quint16 getWifiPort() const;

    void setBleAddress(const QBluetoothAddress &value);
    QBluetoothAddress getBleAddress() const;

    void setSerialPort(const QString &value);
    QString getSerialPort() const;

private:
    static SettingsManager* theInstance_;
    QString m_sSettingsFile{};
    unsigned int EngineDisplacement{1500};
    QString WifiIp{"192.168.0.10"};
    quint16 WifiPort{35000};
    QBluetoothAddress BleAddress{};
    QString SerialPort{};

};

#endif // SETTINGSMANAGER_H
