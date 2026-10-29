// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "thread.h"
#include "commondef.h"
#include "blockspecial.h"
#include "procpartitionsinfo.h"
#include "fsinfo.h"
#include "mountinfo.h"
#include "partedcore.h"
#include "luksoperator/luksoperator.h"

#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QThread>
#include <unistd.h>

namespace DiskManager {

#define DEFAULT_BLOCK_SIZE 4096

WorkThread::WorkThread(QObject *parent)
{
    Q_UNUSED(parent);
    m_stopFlag = 0;
    m_blockStart = 0;
    m_blockEnd = 0;
    m_checkConut = 0;
    m_checkSize = 0;
    qCDebug(serviceLog) << "WorkThread created";
}

void WorkThread::setStopFlag(int flag)
{
    qCDebug(serviceLog) << "Setting stop flag to:" << flag;
    m_stopFlag = flag;
}

void WorkThread::setCountInfo(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize)
{
    qCDebug(serviceLog) << "Setting count info for device:" << devicePath << "start:" << blockStart << "end:" << blockEnd << "count:" << checkConut << "size:" << checkSize;
    m_devicePath = devicePath;
    m_blockStart = blockStart;
    m_blockEnd = blockEnd;
    m_checkConut = checkConut;
    m_checkSize = checkSize;
}

void WorkThread::setTimeInfo(const QString &devicePath, int blockStart, int blockEnd, QString checkTime, int checkSize)
{
    qCDebug(serviceLog) << "Setting time info for device:" << devicePath << "start:" << blockStart << "end:" << blockEnd << "time:" << checkTime << "size:" << checkSize;
    m_devicePath = devicePath;
    m_blockStart = blockStart;
    m_blockEnd = blockEnd;
    m_checkTime = checkTime;
    m_checkSize = checkSize;
}

void WorkThread::runCount()
{
    qCDebug(serviceLog) << "Check range:" << m_blockStart << "-" << m_blockEnd << "with count:" << m_checkConut;
    Sector i = m_blockStart;
    Sector j = m_blockStart + 1;
    while (j <= m_blockEnd + 1 && m_stopFlag != 2) {
        // qCDebug(serviceLog) << "Checking block:" << i;
        QString cmd = QString("badblocks -sv -c %1 -b %2 %3 %4 %5").arg(m_checkConut).arg(DEFAULT_BLOCK_SIZE).arg(m_devicePath).arg(j).arg(i);

        QDateTime ctime = QDateTime::currentDateTime();
        QString stdErr, error;
        int exitcode = Utils::executCmd(cmd, nullptr, &stdErr, error);
        if (exitcode != 0) {
            qCDebug(serviceLog) << "Failed to execute badblocks command, error:" << error;
            return;
        }
        QDateTime ctime1 = QDateTime::currentDateTime();

        if (stdErr.indexOf("0/0/0") != -1) {
            // qCDebug(serviceLog) << "Block" << i << "is good";
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "good";
            QString cylinderErrorInfo = "";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {
            // qCDebug(serviceLog) << "Block" << i << "is bad";
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
        qCDebug(serviceLog) << "Bad blocks count check completed normally";
        emit checkBadBlocksFinished(); //检测完成正常退出,发送给页面的正常结束信号
    } else {
        qCDebug(serviceLog) << "Bad blocks count check was stopped by user";
    }
}

void WorkThread::runTime()
{
    qCDebug(serviceLog) << "Starting bad blocks check by time. Range:" << m_blockStart << "-" << m_blockEnd << "Time limit:" << m_checkTime;
    Sector i = m_blockStart;
    Sector j = m_blockStart + 1;
    while (j <= m_blockEnd + 1 && m_stopFlag != 2) {
        // qCDebug(serviceLog) << "Checking block:" << i;
        QString cmd = QString("badblocks -sv -b %1 %2 %3 %4").arg(DEFAULT_BLOCK_SIZE).arg(m_devicePath).arg(j).arg(i);

        QDateTime ctime = QDateTime::currentDateTime();
        QString stdErr, error;
        int exitcode = Utils::executCmd(cmd, nullptr, &stdErr, error);
        if (exitcode != 0) {
            qCDebug(serviceLog) << "Failed to execute badblocks command, error:" << error;
            return;
        }
        QDateTime ctime1 = QDateTime::currentDateTime();

        if (ctime.msecsTo(ctime1) > m_checkTime.toInt()) {
            // qCDebug(serviceLog) << "Block" << i << "is bad (timeout)";
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Device Timeout";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {
            if (stdErr.indexOf("0/0/0") != -1) {
                // qCDebug(serviceLog) << "Block" << i << "is good";
                QString cylinderNumber = QString("%1").arg(i);
                QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
                QString cylinderStatus = "good";
                QString cylinderErrorInfo = "";

                emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
            } else {
                // qCDebug(serviceLog) << "Block" << i << "is bad (read error)";
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
        qCDebug(serviceLog) << "Bad blocks time check finished normally";
        emit checkBadBlocksFinished();
    } else {
        qCDebug(serviceLog) << "Bad blocks time check was stopped by user";
    }
}

FixThread::FixThread(QObject *parent)
{
    Q_UNUSED(parent);
    m_stopFlag = 0;
    m_checkSize = 0;
    qCDebug(serviceLog) << "FixThread created";
}

void FixThread::setStopFlag(int flag)
{
    qCDebug(serviceLog) << "Setting fix thread stop flag to:" << flag;
    m_stopFlag = flag;
}

void FixThread::runFix()
{
    qCDebug(serviceLog) << "Bad blocks count to fix:" << m_list.size();
    int i = 0;
    while (i < m_list.size() && m_stopFlag != 2) {
        // qCDebug(serviceLog) << "Fixing block:" << m_list.at(i);
        Sector j = m_list.at(i).toInt();
        Sector k = m_list.at(i).toInt() + 1;
        QString cmd = QString("badblocks -sv -b %1 -w %2 %3 %4").arg(m_checkSize).arg(m_devicePath).arg(k).arg(j);

        QDateTime ctime = QDateTime::currentDateTime();
        QString stdErr, error;
        int exitcode = Utils::executCmd(cmd, nullptr, &stdErr, error);
        if (exitcode != 0) {
            qCDebug(serviceLog) << "Failed to execute badblocks command, error:" << error;
            return;
        }
        QDateTime ctime1 = QDateTime::currentDateTime();

        if (stdErr.indexOf("0/0/0") != -1) {
            // qCDebug(serviceLog) << "Fixed block" << j << "successfully";
            QString cylinderNumber = QString("%1").arg(j);
            QString cylinderStatus = "good";
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));

            emit fixBadBlocksInfo(cylinderNumber, cylinderStatus, cylinderTimeConsuming);
        } else {
            // qCDebug(serviceLog) << "Failed to fix block" << j;
            QString cylinderNumber = QString("%1").arg(j);
            QString cylinderStatus = "bad";
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));

            emit fixBadBlocksInfo(cylinderNumber, cylinderStatus, cylinderTimeConsuming);
        }
        i++;
    }

    if (m_stopFlag != 2) {
        qCDebug(serviceLog) << "Bad blocks fix completed normally";
        emit fixBadBlocksFinished();
    } else {
        qCDebug(serviceLog) << "Bad blocks fix was stopped by user";
    }
}

void FixThread::setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize)
{
    qCDebug(serviceLog) << "Setting fix info for device:" << devicePath << "with" << list.count() << "blocks and check size" << checkSize;
    m_devicePath = devicePath;
    m_list = list;
    m_checkSize = checkSize;
}

ProbeThread::ProbeThread(QObject *parent)
{
    Q_UNUSED(parent);
    qCDebug(serviceLog) << "ProbeThread created";
}

void ProbeThread::probeDeviceInfo()
{
    qCDebug(serviceLog) << "ProbeDeviceInfo started in thread";
    QString rootFsName;
    m_inforesult.clear();
    m_deviceMap.clear();
    QVector<QString> devicePaths;
    devicePaths.clear();
    BlockSpecial::clearCache();
    ProcPartitionsInfo::loadCache();
    FsInfo::loadCache();
    MountInfo::loadCache(rootFsName);
    ped_device_probe_all();
    PedDevice *lpDevice = ped_device_get_next(nullptr);
    while (lpDevice) {
        /* TO TRANSLATORS: looks like   Confirming /dev/sda */
        qCDebug(serviceLog) << "Probing device:" << lpDevice->path;

        //only add this device if we can read the first sector (which means it's a real device)

        if (PartedCore::useableDevice(lpDevice)) {
            qCDebug(serviceLog) << "Device is usable, adding to paths:" << lpDevice->path;
            devicePaths.push_back(lpDevice->path);
        }
//        qCDebug(serviceLog) << lpDevice->path;
        lpDevice = ped_device_get_next(lpDevice);
    }
//    qCDebug(serviceLog) << __FUNCTION__ << "devicepaths size=" << devicepaths.size();
    std::sort(devicePaths.begin(), devicePaths.end());
//    qCDebug(serviceLog) << __FUNCTION__ << "**8";
    static PartedCore pcl;
    for (int t = 0; t < devicePaths.size(); t++) {
        /*TO TRANSLATORS: looks like Searching /dev/sda partitions */
        Device tempDevice;
        //PartedCore pcl;
        pcl.setDeviceFromDisk(tempDevice, devicePaths[t]);
        DeviceStorage device;
        tempDevice.m_mediaType = device.getDiskInfoMediaType(devicePaths[t]);
        device.getDiskInfoModel(devicePaths[t], tempDevice.m_model);
        device.getDiskInfoInterface(devicePaths[t], tempDevice.m_interface, tempDevice.m_model);
        m_deviceMap.insert(devicePaths.at(t), tempDevice);
    }
//    qCDebug(serviceLog) << __FUNCTION__ << "**9";
    //这里的代码有可能会恢复，与文管对移动设备的处理相关
//    getPartitionHiddenFlag();
    for (auto it = m_deviceMap.begin(); it != m_deviceMap.end(); it++) {
        DeviceInfo devinfo = it.value().getDeviceInfo();
        for (int i = 0; i < it.value().m_partitions.size(); i++) {
            const Partition &pat = *(it.value().m_partitions.at(i)); //拷贝构造速度提升 const 引用
            PartitionInfo partinfo = pat.getPartitionInfo();
            //这里的代码有可能会恢复，与文管对移动设备的处理相关
//            if(m_hiddenPartition.indexOf(partinfo.m_uuid) != -1 && !partinfo.m_uuid.isEmpty()) {
//                partinfo.m_flag = 1;
//            } else {
//                partinfo.m_flag = 0;
//            }

            //partinfo = pat.getPartitionInfo();
            if (rootFsName == pat.getPath()) {
                partinfo.m_flag = 4;
                qCDebug(serviceLog) << "Set systemfs Flags3 for:" << pat.m_devicePath << "name:" << pat.m_name << "uuid:" << pat.m_uuid;
            }

            if (pat.m_type == PartitionType::TYPE_EXTENDED) {
                devinfo.m_partition.push_back(partinfo);
                for (int k = 0; k < pat.m_logicals.size(); k++) {
                    const Partition &plogic = *(pat.m_logicals.at(k));
                    partinfo = plogic.getPartitionInfo();
                    if (rootFsName == plogic.m_name) {
                        partinfo.m_flag = 4;
                        qCDebug(serviceLog) << "Set systemfs Flags4 for:" << plogic.m_devicePath << "name:" << plogic.m_name << "uuid:" << plogic.m_uuid;
                    }
                    qCDebug(serviceLog) << "Logical partition:" << plogic.m_devicePath << "name:" << plogic.m_name << "uuid:" << plogic.m_uuid;
                    devinfo.m_partition.push_back(partinfo);
                }
            } else {
                devinfo.m_partition.push_back(partinfo);
            }
        }
        m_inforesult.insert(devinfo.m_path, devinfo);
    }
//    qCDebug(serviceLog) << __FUNCTION__ << m_inforesult.count();
//    qCDebug(serviceLog) << __FUNCTION__ << "**10";

    //todo 2022.1.26 获取m_lvminfo
    LVMOperator::getDeviceDataAndLVMInfo(m_inforesult, m_lvmInfo);
    LUKSOperator::updateLUKSInfo(m_inforesult,m_lvmInfo,m_luksInfo);

    emit updateDeviceInfo(/*m_deviceMap,*/ m_inforesult, m_lvmInfo,m_luksInfo);

    qCDebug(serviceLog) << "ProbeDeviceInfo completed";
    qCDebug(serviceLog) << "Current thread ID:" << QThread::currentThreadId();
}

QMap<QString, Device> ProbeThread::getDeviceMap()
{
    return m_deviceMap;
}


LVMThread::LVMThread(QObject *parent)
{
    Q_UNUSED(parent)
    qRegisterMetaType<LVMInfo>();
}

void LVMThread::deletePVList(LVMInfo lvmInfo, QList<PVData> devList)
{
    qCDebug(serviceLog) << "LVMThread::deletePVList BEGIN";
    static bool flag = true; //同时只允许一个线程进入
    if (flag) {
        qCDebug(serviceLog) << "flag is true";
        flag = false;
        emit deletePVListFinished(LVMOperator::deletePVList(lvmInfo, devList));
    }
    flag = true;
    qCDebug(serviceLog) << "LVMThread::deletePVList END";
}

void LVMThread::resizeVG(LVMInfo lvmInfo, QString vgName, QList<PVData> devList, long long size)
{
    qCDebug(serviceLog) << "LVMThread::resizeVG BEGIN";
    static bool flag = true;
    if (flag) {
        qCDebug(serviceLog) << "flag is true";
        flag = false;
        emit resizeVGFinished(LVMOperator::resizeVG(lvmInfo, vgName, devList, size));
    }
    flag = true;
    qCDebug(serviceLog) << "LVMThread::resizeVG END";
}

}
