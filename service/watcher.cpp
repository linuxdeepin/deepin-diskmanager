/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file watcher.cpp
 *
 * @brief 检测前端进程是否被用户强制退出，如果前端被杀，则后台进程需要自动退出
 *
 * @date 2021-11-19 11:38
 *
 * Author: liweigang  <liweigang@uniontech.com>
 *
 * Maintainer: liweigang  <liweigang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "watcher.h"
#include <sys/types.h>
#include <signal.h>
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QProcess>
#include <QString>
#include <QCoreApplication>


namespace DiskManager {

/**
 * @brief 执行外部命令
 * @param strCmd:外部命令字符串
 * @param outPut:命令控制台输出
 * @param error:错误信息
 * @return exitcode:退出码
 */
void Watcher::executCmd(const QString &strCmd, QString &outPut, QString &error)
{
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << strCmd);
    proc.waitForFinished(-1);
    outPut = proc.readAllStandardOutput();
}

void Watcher::run()
{
    bool isrun = false;
    QString cmd, outPut, error;
    //先判断后台服务进程是否存在,如果存在可能是强制退出导致,应先退出后台程序再重新启动磁盘管理器
    cmd = QString("ps -eo pid,cmd |awk '{print $2}' |grep -w deepin-diskmanager$");

    while (1) {
        QThread::msleep(500);  //0.5 second
        executCmd(cmd, outPut, error);
        int ret = 0;
        ret = outPut.length();
        if (ret) {
            //这里表示前端在运行当中
            qDebug() << "Set to true!!!!!";
            isrun = true;
        } else {
            //这里表示，前端启动过，但是现在已经关闭了
            if (isrun) {
                qDebug() << "Need to quit now";
                exit(0);
            }
        }
        qDebug() << "Sleep !!!  == " << ret << " " << outPut << " " << outPut.length() << " "<< error;
    }
}

} //endof namespace
