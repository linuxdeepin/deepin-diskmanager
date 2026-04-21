// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "diskmanagerservice.h"
#include "log.h"

#include <QCoreApplication>
#include <DLog>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusServiceWatcher>
#include <QThread>

const QString DiskManagerServiceName = "com.deepin.diskmanager";
const QString DiskManagerPath = "/com/deepin/diskmanager";

int main(int argc, char *argv[])
{
    //set env otherwise utils excutecmd  excute command failed
    QString PATH = qgetenv("PATH");

    qDebug() << "Application starting with PATH:" << PATH;
    if (PATH.isEmpty()) {
        qWarning() << "PATH environment variable is empty";
        PATH = "/usr/bin";
    }
    PATH += ":/usr/sbin";
    PATH += ":/sbin";
    qputenv("PATH", PATH.toLatin1());

    QString frontEndDBusName;
    if (argc < 2) {
        qCritical() << "Invalid arguments count:" << argc;
        return 1;
    } else {
        qDebug() << "Arguments count:" << argc;
        frontEndDBusName = QString(argv[1]);
        if (frontEndDBusName.isEmpty()) {
            qCritical() << "Invalid front-end DBus name";
            return 1;
        }
    }

    QCoreApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-diskmanager-service");

    QDir dirCheck;
    QString LogPath = QString("%1/%2/%3/Log/")
        .arg("/var/log")
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());
    setLogDir(LogPath);
    if (!dirCheck.exists(LogPath))
    {
        qDebug() << "Log path does not exist, creating:" << LogPath;
        dirCheck.mkpath(LogPath);
    }
    //检查日志是否过期
    CheckLogTime();
    //磁盘剩余空间小于阈值，清除早期日志
    CheckFreeDisk();
    //创建新日志
    CreateNewLog();
    qInstallMessageHandler(customLogMessageHandler);
   Dtk::Core::DLogManager::registerConsoleAppender();
   // Dtk::Core::DLogManager::registerFileAppender();

//    qDebug() << "write log to" << Dtk::Core::DLogManager::getlogFilePath();
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(DiskManagerServiceName)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    DiskManager::DiskManagerService service(frontEndDBusName);
    qDebug() << "systemBus.registerService success" /*<< Dtk::Core::DLogManager::getlogFilePath()*/;
    if (!systemBus.registerObject(DiskManagerPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }

    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(
        frontEndDBusName,
        QDBusConnection::systemBus(),
        QDBusServiceWatcher::WatchForUnregistration,
        &a
    );
    QObject::connect(watcher, &QDBusServiceWatcher::serviceUnregistered, [](const QString &name) {
        qWarning() << "Front-end service unregistered:" << name;
        QCoreApplication::exit(0);
    });

    return a.exec();
}
