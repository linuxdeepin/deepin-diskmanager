// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "diskmanagerservice.h"
#include "log.h"
#include "watcher.h"

#include <QCoreApplication>
#include <DLog>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QThread>
#include <unistd.h>

const QString DiskManagerServiceName = "com.deepin.diskmanager";
const QString DiskManagerPath = "/com/deepin/diskmanager";

/**
 * @brief 尝试停止已存在的 D-Bus 服务
 * @param systemBus D-Bus 连接
 * @return true 表示成功停止并可重新注册，false 表示失败
 *
 * 通过 D-Bus 调用 Quit() 方法优雅退出，未使用 PID kill，
 * 以避免 TOCTOU 竞态条件和 PID 复用带来的安全风险。
 * Quit() 调用后轮询等待 D-Bus 服务名称释放，确认服务已退出。
 */
static bool stopExistingService(QDBusConnection &systemBus)
{
    QDBusInterface existingService(DiskManagerServiceName, DiskManagerPath, "", systemBus);
    if (!existingService.isValid()) {
        qWarning() << "Cannot access existing service via D-Bus interface,"
                   << "service may have already exited, retry registration";
        return true;
    }

    qDebug() << "Calling Quit() on existing service...";
    QDBusPendingCall pendingCall = existingService.asyncCall("Quit");
    pendingCall.waitForFinished();
    if (pendingCall.isError()) {
        qWarning() << "Quit() call failed:" << pendingCall.error().message();
        return false;
    }

    // 轮询等待 D-Bus 服务名称释放，最多等待 3 秒
    qDebug() << "Quit() called successfully, waiting for service to exit...";
    const int maxWaitMs = 3000;
    const int pollIntervalMs = 200;
    for (int elapsed = 0; elapsed < maxWaitMs; elapsed += pollIntervalMs) {
        usleep(pollIntervalMs * 1000);
        // 通过尝试注册来判断名称是否已释放（注册后立即释放）
        if (systemBus.registerService(DiskManagerServiceName)) {
            systemBus.unregisterService(DiskManagerServiceName);
            qDebug() << "Service name released after" << (elapsed + pollIntervalMs) << "ms";
            return true;
        }
    }

    qWarning() << "Timed out waiting for existing service to exit";
    return false;
}

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

    QString frontEndDBusName;
    if (argc >= 2) {
        frontEndDBusName = QString(argv[1]);
        qDebug() << "Received frontend DBus name:" << frontEndDBusName;
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
        dirCheck.mkpath(LogPath);
    }
    //检查日志是否过期
    CheckLogTime();
    //磁盘剩余空间小于阈值，清除早期日志
    CheckFreeDisk();
    //创建新日志
    CreateNewLog();
    qInstallMessageHandler(customLogMessageHandler);
//    Dtk::Core::DLogManager::registerConsoleAppender();
//    Dtk::Core::DLogManager::registerFileAppender();

//    qDebug() << "write log to" << Dtk::Core::DLogManager::getlogFilePath();
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.registerService(DiskManagerServiceName)) {
        qCritical() << "registerService failed:" << systemBus.lastError();
        // 服务名注册失败，通常是因为已有实例持有该名称，尝试停止它
        if (stopExistingService(systemBus)) {
            qDebug() << "Retrying registerService after stopping existing service...";
            // 停止旧服务后重试注册
            if (!systemBus.registerService(DiskManagerServiceName)) {
                qCritical() << "registerService failed even after stopping existing service";
                exit(0x0001);
            }
        } else {
            qCritical() << "Failed to stop existing service";
            exit(0x0001);
        }
    }
    DiskManager::DiskManagerService service(frontEndDBusName);
    qDebug() << "systemBus.registerService success" /*<< Dtk::Core::DLogManager::getlogFilePath()*/;
    if (!systemBus.registerObject(DiskManagerPath,
                                  &service,
                                  QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
        qCritical() << "registerObject failed:" << systemBus.lastError();
            exit(0x0002);
    }

    /*
     * 启动一个线程监测前端是否运行：
     *    1 如果前端没有启动过，则后台保持运行
     *    2 如果前端启动过，又退出了，则后台退出。这可能是因为用户从dock栏强杀了磁盘管理器。
     */
    DiskManager::Watcher m_watcher;
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, &m_watcher, &DiskManager::Watcher::exit);
    m_watcher.start();

    return a.exec();
}
