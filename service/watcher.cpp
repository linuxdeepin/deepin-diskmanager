// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "watcher.h"
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QRegularExpression>
#include <QCoreApplication>


namespace DiskManager {

/**
 * @brief 检测前端进程 deepin-diskmanager 是否在运行
 *
 * 仅启动 ps 一个子进程并在代码内完成过滤，避免使用 shell 解释器，也不再串联 awk/grep 子进程。
 * 等价于管道 `ps -eo cmd | grep -w deepin-diskmanager$`。
 *
 */
bool Watcher::isFrontEndRunning(QString &error)
{
    QProcess proc;
    proc.start("ps", QStringList() << "-eo" << "cmd");
    proc.waitForFinished(-1);
    error = proc.readAllStandardError();

    static const QRegularExpression rxMatch("(?:^|[^\\w])deepin-diskmanager$");
    const QStringList lines = QString::fromLocal8Bit(proc.readAllStandardOutput()).split('\n');
    for (const QString &line : lines) {
        const QString exe = line.trimmed().section(' ', 0, 0, QString::SectionSkipEmpty);
        if (rxMatch.match(exe).hasMatch()) {
            return true;
        }
    }

    return false;
}

void Watcher::exit()
{
    qDebug() << "Watcher wait for exit now!";
    stoped = true;
    this->wait();
}

void Watcher::run()
{
    bool isrun = false;
    QString error;

    while (!stoped) {
        QThread::msleep(500);  //0.5 second
        if (isFrontEndRunning(error)) {
            //这里表示前端在运行当中
            isrun = true;
        } else {
            //这里表示，前端启动过，但是现在已经关闭了
            if (isrun) {
                qDebug() << "Need to quit now";
                _exit(0);
            }
        }
    }
}

} //endof namespace
