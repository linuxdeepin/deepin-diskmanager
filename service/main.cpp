// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "diskmanagerservice.h"
#include "log.h"
#include "logger.h"

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
    // 动态日志规则监听必须放在最前：qunsetenv("QT_LOGGING_RULES") 要求在任何日志打印之前
    MLogger logger;

    //set env otherwise utils excutecmd  excute command failed
    QString PATH = qgetenv("PATH");

    qCDebug(serviceLog) << "Application starting with PATH:" << PATH;
    if (PATH.isEmpty()) {
        qCWarning(serviceLog) << "PATH environment variable is empty";
        PATH = "/usr/bin";
    }
    PATH += ":/usr/sbin";
    PATH += ":/sbin";
    qputenv("PATH", PATH.toLatin1());

    // 避免在调用 Dtk::Core::DLogManager::registerConsoleAppender() 时 dtkcore 启动会话总线
    qunsetenv("DISPLAY");

    QString frontEndDBusName;
    if (argc < 2) {
        qCCritical(serviceLog) << "Invalid arguments count:" << argc;
        return 1;
    } else {
        qCDebug(serviceLog) << "Arguments count:" << argc;
        frontEndDBusName = QString(argv[1]);
        if (frontEndDBusName.isEmpty()) {
            qCCritical(serviceLog) << "Invalid front-end DBus name";
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
        qCDebug(serviceLog) << "Log path does not exist, creating:" << LogPath;
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

//    qCDebug(serviceLog) << "write log to" << Dtk::Core::DLogManager::getlogFilePath();
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(DiskManagerServiceName)) {
        qCCritical(serviceLog) << "registerService failed:" << systemBus.lastError();
        exit(0x0001);
    }
    DiskManager::DiskManagerService service(frontEndDBusName);
    qCDebug(serviceLog) << "systemBus.registerService success" /*<< Dtk::Core::DLogManager::getlogFilePath()*/;
    if (!systemBus.registerObject(DiskManagerPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCCritical(serviceLog) << "registerObject failed:" << systemBus.lastError();
        exit(0x0002);
    }

    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(
        frontEndDBusName,
        QDBusConnection::systemBus(),
        QDBusServiceWatcher::WatchForUnregistration,
        &a
    );
    QObject::connect(watcher, &QDBusServiceWatcher::serviceUnregistered, [](const QString &name) {
        qCWarning(serviceLog) << "Front-end service unregistered:" << name;
        QCoreApplication::exit(0);
    });

    return a.exec();
}
