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

    void setEngineDisplacement(unsigned int value);
    void setIp(const QString &value);
    void setPort(const quint16 &value);

    unsigned int getEngineDisplacement() const;
    QString getIp() const;
    quint16 getPort() const;

private:
    static SettingsManager* theInstance_;
    QString m_sSettingsFile{};
    unsigned int EngineDisplacement;
    QString Ip{};
    quint16 Port{};

};

#endif // SETTINGSMANAGER_H
