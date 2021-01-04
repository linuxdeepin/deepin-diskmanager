/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file thread.cpp
 *
 * @brief 坏道检测修复线程类
 *
 * @date 2021-01-04 13:58
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
#include "thread.h"
#include "commondef.h"

#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QThread>

namespace DiskManager {

workthread::workthread(QObject *parent)
{
    Q_UNUSED(parent);
    m_stopFlag = 0;
}

void workthread::setStopFlag(int flag)
{
    m_stopFlag = flag;
}

void workthread::setConutInfo(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize)
{
    m_devicePath = devicePath;
    m_blockStart = blockStart;
    m_blockEnd = blockEnd;
    m_checkConut = checkConut;
    m_checkSize = checkSize;
}

void workthread::setTimeInfo(const QString &devicePath, int blockStart, int blockEnd, QString checkTime, int checkSize)
{
    m_devicePath = devicePath;
    m_blockStart = blockStart;
    m_blockEnd = blockEnd;
    m_checkTime = checkTime;
    m_checkSize = checkSize;
}

void workthread::runCount()
{
    Sector i = m_blockStart;
    Sector j = m_blockStart+1;
    QProcess proc;
    while(j <= m_blockEnd+1 && m_stopFlag != 2)
    {
        QString cmd = QString("badblocks -sv -c %1 -b %2 %3 %4 %5").arg(m_checkConut).arg(m_checkSize).arg(m_devicePath).arg(j).arg(i);

        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();

        cmd = proc.readAllStandardError();

        if (cmd.indexOf("(0/0/0 errors)") != -1) {

            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "good";
            QString cylinderErrorInfo = "";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {

            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Read Error";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        }

        i++;
        j++;
    }

    if (m_stopFlag != 2) {
        emit checkBadBlocksFinished();
    }

    emit checkBadBlocksDeviceStatusFinished();
}

void workthread::runTime()
{

    Sector i = m_blockStart;
    Sector j = m_blockStart+1;
    QProcess proc;
    while(j <= m_blockEnd+1 && m_stopFlag != 2)
    {
        QString cmd = QString("badblocks -sv -b %1 %2 %3 %4").arg(m_checkSize).arg(m_devicePath).arg(j).arg(i);

        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();

        cmd = proc.readAllStandardError();

        if(ctime.msecsTo(ctime1) > m_checkTime.toInt()) {
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Device Timeout";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {
            if (cmd.indexOf("(0/0/0 errors)") != -1) {
                QString cylinderNumber = QString("%1").arg(i);
                QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
                QString cylinderStatus = "good";
                QString cylinderErrorInfo = "";

                emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
            } else {
                QString cylinderNumber = QString("%1").arg(i);
                QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
                QString cylinderStatus = "bad";
                QString cylinderErrorInfo = "IO Read Error";

                emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
            }
        }
        i++;
        j++;
    }

    if (m_stopFlag != 2) {
        emit checkBadBlocksFinished();
    }

    emit checkBadBlocksDeviceStatusFinished();
}

fixthread::fixthread(QObject *parent)
{
    Q_UNUSED(parent);
    m_stopFlag = 0;
}

void fixthread::setStopFlag(int flag)
{
    m_stopFlag = flag;
}

void fixthread::runFix()
{
    qDebug() << m_list << endl;
    int i = 0;
    QProcess proc;
    while(i < m_list.size() && m_stopFlag != 2) {
        Sector j = m_list.at(i).toInt();
        Sector k = m_list.at(i).toInt() + 1;
        QString cmd = QString("badblocks -sv -b %1 -w %2 %3 %4").arg(m_checkSize).arg(m_devicePath).arg(k).arg(j);

        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();

        cmd = proc.readAllStandardError();

        if (cmd.indexOf("(0/0/0 errors)") != -1) {

            QString cylinderNumber = QString("%1").arg(j);
            QString cylinderStatus = "good";
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));

            emit fixBadBlocksInfo(cylinderNumber, cylinderStatus, cylinderTimeConsuming);
        } else {

            QString cylinderNumber = QString("%1").arg(j);
            QString cylinderStatus = "bad";
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));

            emit fixBadBlocksInfo(cylinderNumber, cylinderStatus, cylinderTimeConsuming);
        }
        i++;
    }

    if (m_stopFlag != 2) {
        emit fixBadBlocksFinished();
    }

    emit checkBadBlocksDeviceStatusFinished();
}

void fixthread::setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize)
{
    m_devicePath = devicePath;
    m_list = list;
    m_checkSize = checkSize;
}
}
