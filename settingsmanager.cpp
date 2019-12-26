#include "settingsmanager.h"

SettingsManager* SettingsManager::theInstance_ = nullptr;

SettingsManager *SettingsManager::getInstance()
{
    if (theInstance_ == nullptr)
    {
        theInstance_ = new SettingsManager();
    }
    return theInstance_;
}

SettingsManager::SettingsManager()
{
    m_sSettingsFile = QCoreApplication::applicationDirPath() + "/settings.ini";
    if (QFile(m_sSettingsFile).exists())
        loadSettings();
}

void SettingsManager::loadSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    EngineDisplacement = settings.value("EngineDisplacement", "").toString().toUInt();
    WifiIp = settings.value("WifiIp", "").toString();
    WifiPort = settings.value("WifiPort", "").toString().toUShort();
    BleAddress = QBluetoothAddress(settings.value("BleAddress", "").toString());
    SerialPort = settings.value("SerialPort", "").toString();
}

void SettingsManager::saveSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    settings.setValue("EngineDisplacement", QString::number(EngineDisplacement));
    settings.setValue("WifiIp", WifiIp);
    settings.setValue("WifiPort", QString::number(WifiPort));
    settings.setValue("BleAddress", BleAddress.toString());
    settings.setValue("SerialPort", SerialPort);
}

unsigned int SettingsManager::getEngineDisplacement() const
{
    return EngineDisplacement;
}

void SettingsManager::setEngineDisplacement(unsigned int value)
{
    EngineDisplacement = value;
}

void SettingsManager::setWifiIp(const QString &value)
{
    WifiIp = value;
}

void SettingsManager::setWifiPort(const quint16 &value)
{
    WifiPort = value;
}

QString SettingsManager::getWifiIp() const
{
    return WifiIp;
}

quint16 SettingsManager::getWifiPort() const
{
    return WifiPort;
}

void SettingsManager::setBleAddress(const QBluetoothAddress &value)
{
    BleAddress = value;
}

QBluetoothAddress SettingsManager::getBleAddress() const
{
    return BleAddress;
}

QString SettingsManager::getSerialPort() const
{
    return SerialPort;
}

void SettingsManager::setSerialPort(const QString &value)
{
    SerialPort = value;
}


