#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QtCore>
#include <QSettings>

class SettingsManager
{
public:
    SettingsManager();

    static SettingsManager* getInstance();

    void loadSettings();
    void saveSettings();
    unsigned int getEngineDisplacement() const;
    void setEngineDisplacement(unsigned int value);

private:
    static SettingsManager* theInstance_;
    QString m_sSettingsFile{};
    unsigned int EngineDisplacement;

};

#endif // SETTINGSMANAGER_H
