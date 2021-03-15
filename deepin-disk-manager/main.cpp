/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file main.cpp
 *
 * @brief 磁盘管理器启动程序
 *
 * @date 2021-03-08 16:24
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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QThread>



/**
 * @brief 执行外部命令
 * @param strCmd:外部命令字符串
 * @param outPut:命令控制台输出
 * @param error:错误信息
 * @return exitcode:退出码
 */
int executCmd(const QString &strCmd, QString &outPut, QString &error)
{
    QProcess proc;
    proc.start(strCmd);
    proc.waitForFinished(-1);
    outPut = proc.readAllStandardOutput();
    error = proc.readAllStandardError();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    proc.close();
    return exitcode;

}

/**
 * @brief 磁盘管理器启动程序
 * @return exitcode:退出码
 */
int main()
{
    QProcess proc;
    QString cmd, outPut, error;
    //先判断后台服务进程是否存在,如果存在可能是强制退出导致,应先退出后台程序再重新启动磁盘管理器
    cmd = QString("pidof deepin-diskmanager-service");

    if (!executCmd(cmd, outPut, error)) {
        proc.startDetached("/usr/bin/dbus-send --system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.Quit");
    }

    //判断前端页面是否已经启动,如果启动,直接再次启动页面程序,然后退出此程序
    cmd = QString("pidof deepin-diskmanager");

    if (!executCmd(cmd, outPut, error)) {
        proc.startDetached("deepin-diskmanager");
        return 0;
    }

    proc.startDetached("/usr/bin/deepin-diskmanager-authenticateProxy");

    //正常启动程序后,循环查询后台服务是否已经启动,如果后台服务启动说明鉴权成功,启动前端界面
    while (1) {
        cmd = QString("pidof deepin-diskmanager-service");

        if (!executCmd(cmd, outPut, error)) {
            proc.startDetached("deepin-diskmanager");
            break;
        }
        QThread::msleep(300);
    }

    proc.close();
    return 0;
}
