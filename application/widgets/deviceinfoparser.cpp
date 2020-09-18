/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     AaronZhang <ya.zhang@archermind.com>
 *
 * Maintainer: AaronZhang <ya.zhang@archermind.com>
 * Maintainer: Yaobin <yao.bin@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "deviceinfoparser.h"
#include <QObject>
#include <sys/utsname.h>
#include <iostream>
#include <QFile>
#include <QProcess>
#include <QRegExp>
#include <DLog>
#include <DApplication>
#include <DSysInfo>
#include "DMessageBox"
#include "Logger.h"

DCORE_USE_NAMESPACE

DeviceInfoParser::DeviceInfoParser(): QObject()
{

}

DeviceInfoParser::~DeviceInfoParser()
{

}


bool DeviceInfoParser::getRootPassword()
{
    bool res = runCmd("id -un");  // file path is fixed. So write cmd direct
    if (res == true && m_standOutput.trimmed() == "root") {
        return true;
    }

#ifdef TEST_DATA_FROM_FILE
    return true;
#endif
    if (false == executeProcess("sudo whoami")) {
        return false;
    }

    return true;
}

bool DeviceInfoParser::executeProcess(const QString &cmd)
{
#ifdef TEST_DATA_FROM_FILE
    return true;
#endif
    if (false == cmd.startsWith("sudo")) {
        return runCmd(cmd);
    }

    runCmd("id -un");
    if (m_standOutput.trimmed() == "root") {
        return runCmd(cmd);
    }

    QString newCmd = "pkexec deepin-diskmanager-authenticateProxy \"";
    newCmd += cmd;
    newCmd += "\"";
    newCmd.remove("sudo");
    return runCmd(newCmd);
}

bool DeviceInfoParser::runCmd(const QString &proxy)
{
    QString key = "devicemanager";
    QString cmd = proxy;
    QProcess process;
    int msecs = 10000;
    if (cmd.startsWith("pkexec deepin-diskmanager-authenticateProxy")) {
        cmd = proxy + QString(" ") + key;
        msecs = -1;
    }

    process.start(cmd);

    bool res = process.waitForFinished(msecs);
    m_standOutput = process.readAllStandardOutput();
    int exitCode = process.exitCode();
    if (cmd.startsWith("pkexec deepin-diskmanager-authenticateProxy") && (exitCode == 127 || exitCode == 126)) {
        dError("Run \'" + cmd + "\' failed: Password Error! " + QString::number(exitCode) + "\n");
        return false;
    }

    if (res == false) {
        dError("Run \'" + cmd + "\' failed\n");
    }

    return res;
}

bool DeviceInfoParser::runCmd(const QStringList &cmdList)
{
    QProcess process;
    process.start("/bin/bash", cmdList);
    bool res = process.waitForFinished(10000);
    m_standOutput = process.readAllStandardOutput();
    return res;
}
