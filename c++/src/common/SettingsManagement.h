#ifndef SETTINGS_MANAGEMENT_H
#define SETTINGS_MANAGEMENT_H

#include <QSettings>
#include <QFile>
#include <utility>


namespace SettingsManagement
{
    inline std::pair<int, int> loadSettings(QString path = "config.ini")
    {
        QSettings settings(path, QSettings::IniFormat);
        std::pair<int, int> pair;

        settings.beginGroup("SERVER");
        pair.first = settings.value("port", 33333).toInt();
        settings.endGroup();

        settings.beginGroup("LOGGER");
        pair.second = settings.value("level", 3).toInt();
        settings.endGroup();

        return pair;
    }

    inline void setDefaultSettigs(QString path = "config.ini")
    {
        QSettings settings(path, QSettings::IniFormat);

        settings.beginGroup("SERVER");
        settings.setValue("port", 33333);
        settings.endGroup();

        settings.beginGroup("LOGGER");
        settings.setValue("level", 3);
        settings.endGroup();
    }
};

#endif //SETTINGS_MANAGEMENT_H