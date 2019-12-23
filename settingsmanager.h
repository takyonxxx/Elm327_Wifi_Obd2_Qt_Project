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

    void setIp(const QString &value);
    void setPort(const quint16 &value);

    void setEngineDisplacement(unsigned int value);
    unsigned int getEngineDisplacement() const;

    QString getIp() const;
    quint16 getPort() const;

    void setBleAddress(const QBluetoothAddress &value);
    QBluetoothAddress getBleAddress() const;

private:
    static SettingsManager* theInstance_;
    QString m_sSettingsFile{};
    unsigned int EngineDisplacement;
    QString Ip{};
    quint16 Port{};
    QBluetoothAddress BleAddress{};

};

#endif // SETTINGSMANAGER_H
