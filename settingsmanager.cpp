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
}

void SettingsManager::saveSettings()
{
    QSettings settings(m_sSettingsFile, QSettings::IniFormat);
    settings.setValue("EngineDisplacement", QString::number(EngineDisplacement));
}

unsigned int SettingsManager::getEngineDisplacement() const
{
    return EngineDisplacement;
}

void SettingsManager::setEngineDisplacement(unsigned int value)
{
    EngineDisplacement = value;
}


