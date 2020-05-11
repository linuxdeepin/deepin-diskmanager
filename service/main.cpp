#include <QCoreApplication>
#include <DLog>
#include <QDBusConnection>
#include <QDBusError>
#include "diskmanagerservice.h"

const QString DiskManagerServiceName = "com.deepin.diskmanager";
const QString DiskManagerPath = "/com/deepin/diskmanager";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-diskmanager-service");

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();


    qDebug() << "write log to" << Dtk::Core::DLogManager::getlogFilePath();
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(DiskManagerServiceName)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    DiskManager::DiskManagerService service;
    qDebug() << "systemBus.registerService success" << Dtk::Core::DLogManager::getlogFilePath();
    if (!systemBus.registerObject(DiskManagerPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots |
                                  QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }
    return a.exec();
}
