#include <QCoreApplication>

#include "server.h"
#include "common/SettingsManagement.h"
#include "common/Logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SettingsManagement::setDefaultSettigs();
    auto settings = SettingsManagement::loadSettings();

    Server server(settings.first, std::move(std::make_shared<Logger>("server.log", settings.second)));
    return a.exec();
}
