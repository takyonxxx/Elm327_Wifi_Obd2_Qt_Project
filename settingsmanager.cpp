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
    else
        EngineDisplacement = 1500;
}

void SettingsManager::loadSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    EngineDisplacement = settings.value("EngineDisplacement", "").toString().toUInt();
    Ip = settings.value("Ip", "").toString();
    Port = settings.value("Port", "").toString().toUShort();
}

void SettingsManager::saveSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    settings.setValue("EngineDisplacement", QString::number(EngineDisplacement));
    settings.setValue("Ip", Ip);
    settings.setValue("Port", QString::number(Port));
}

unsigned int SettingsManager::getEngineDisplacement() const
{
    return EngineDisplacement;
}

void SettingsManager::setEngineDisplacement(unsigned int value)
{
    EngineDisplacement = value;
}

void SettingsManager::setIp(const QString &value)
{
    Ip = value;
}

void SettingsManager::setPort(const quint16 &value)
{
    Port = value;
}

QString SettingsManager::getIp() const
{
    return Ip;
}

quint16 SettingsManager::getPort() const
{
    return Port;
}


