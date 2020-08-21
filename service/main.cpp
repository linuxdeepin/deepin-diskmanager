#include <QCoreApplication>
#include <DLog>
#include <QDBusConnection>
#include <QDBusError>
#include "diskmanagerservice.h"
#include "log.h"

const QString DiskManagerServiceName = "com.deepin.diskmanager";
const QString DiskManagerPath = "/com/deepin/diskmanager";

int main(int argc, char *argv[])
{
    //set env otherwise utils excutecmd  excute command failed
    QString PATH = qgetenv("PATH");

    qDebug() << "main start" << PATH;
    if (PATH.isEmpty()) {
        PATH = "/usr/bin";
    }
    PATH += ":/usr/sbin";
    PATH += ":/sbin";
    qputenv("PATH", PATH.toLatin1());

    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-diskmanager-service");

    QDir dirCheck;
    QString Log_path = QString("%1/%2/%3/Log/")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());
    qDebug()<<Log_path;
    setLogDir(Log_path);
    if (!dirCheck.exists(Log_path))
    {
        dirCheck.mkpath(Log_path);
    }
    //检查日志是否过期
    CheckLogTime();
    //磁盘剩余空间小于阈值，清除早期日志
    CheckFreeDisk();
    //创建新日志
    CreateNewLog();
    qInstallMessageHandler(customLogMessageHandler);
    qDebug()<<Log_path;//QStandardPaths::displayName(QStandardPaths::ConfigLocation);
//    Dtk::Core::DLogManager::registerConsoleAppender();
//    Dtk::Core::DLogManager::registerFileAppender();

//    qDebug() << "write log to" << Dtk::Core::DLogManager::getlogFilePath();
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(DiskManagerServiceName)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    DiskManager::DiskManagerService service;
    qDebug() << "systemBus.registerService success" /*<< Dtk::Core::DLogManager::getlogFilePath()*/;
    if (!systemBus.registerObject(DiskManagerPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }
    return a.exec();
}
