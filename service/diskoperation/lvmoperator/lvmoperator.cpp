/*
* Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     liuwenhao <liuwenhao@uniontech.com>
*
* Maintainer: liuwenhao <liuwenhao@uniontech.com>
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
#include "lvmoperator.h"
#include "utils.h"
#include <sys/vfs.h>
#include <QStringList>
#include <QDebug>
LVM_CMD_Support LVMOperator::m_lvmSupport;
bool LVMOperator::m_initSupport = false;

#define getExecSupport(cmd) Utils::findProgramInPath(cmd).isEmpty()? LVM_CMD_Support::NONE : LVM_CMD_Support::EXTERNAL
/******************************** public ******************************/
LVMOperator::LVMOperator()
{

}

bool LVMOperator::updateLVMInfo(LVMInfo &m_lvmInfo)
{
    initSuport();
    resetLVMInfo(m_lvmInfo);
    updateVGInfo(m_lvmInfo);
    updatePVInfo(m_lvmInfo);
    return m_lvmInfo.m_lvmErr == LVMError::LVM_Err_Normal;
}

bool LVMOperator::getDeviceLVMData(DeviceInfoMap &dev, const LVMInfo &lvmInfo)
{
    auto func = [ = ](const PVInfo & pv) {
        VGData vg;
        vg.m_vgName = pv.m_vgName;
        vg.m_vgSize = pv.m_pvSize;
        vg.m_vgByteSize = (pv.m_pvUsedPE + pv.m_pvUnusedPE) * pv.m_PESize;
        for (QMap<QString, QVector<LV_PV_Ranges>>::ConstIterator lvIt = pv.m_lvRangesList.begin(); lvIt != pv.m_lvRangesList.end(); ++lvIt) {
            LVData lvData;
            lvData.m_lvPath = lvIt.key();
            long long sum = 0;
            foreach (const LV_PV_Ranges &it, lvIt.value()) {
                sum += (it.m_end - it.m_start + 1);
            }
            lvData.m_lvByteSize = sum * pv.m_PESize;
            lvData.m_lvSize = Utils::LVMFormatSize(lvData.m_lvByteSize);
        }
        return vg;
    };

    for (QMap<QString, DeviceInfo>::iterator diskIter = dev.begin(); diskIter != dev.end(); ++diskIter) {
        diskIter->m_vgFlag = false;
        QMap<QString, PVInfo>::ConstIterator pvIter = lvmInfo.m_pvInfo.find(diskIter->m_path);
        if (pvIter != lvmInfo.m_pvInfo.end() && diskIter.value().m_disktype == "unrecognized") { //不存在分区表 磁盘加入
            diskIter->m_vgFlag = true;
            diskIter->m_vglist.push_back(func(pvIter.value()));
            continue;
        }

        for (QVector<PartitionInfo>::iterator partIter = diskIter.value().m_partition.begin(); partIter != diskIter.value().m_partition.end(); ++partIter) {
            QMap<QString, PVInfo>::ConstIterator pvIter = lvmInfo.m_pvInfo.find(partIter->m_path);
            if (pvIter != lvmInfo.m_pvInfo.end()) {
                const PVInfo &pv = pvIter.value();
                diskIter->m_vgFlag = true;
                partIter->m_vgFlag = true;
                partIter->m_vgData = func(pv);
                diskIter->m_vglist.push_back(partIter->m_vgData);
            }
        }
    }
    return lvmInfo.m_lvmErr == LVMError::LVM_Err_Normal;
}

bool LVMOperator::getDeviceDataAndLVMInfo(DeviceInfoMap &dev, LVMInfo &m_lvmInfo)
{
    updateLVMInfo(m_lvmInfo);
    getDeviceLVMData(dev, m_lvmInfo);
    return m_lvmInfo.m_lvmErr == LVM_Err_Normal;
}

void LVMOperator::printError(const LVMError &error)
{

}

void LVMOperator::printLVMInfo(const LVMInfo &m_lvmInfo)
{
    qDebug() << "**********************************************************************";
    qDebug() << QString("LVMVGINFO:====================================being");
    foreach (auto it, m_lvmInfo.m_vgInfo) {
        printVGInfo(it);
    }
    qDebug() << QString("LVMVGINFO:====================================end");
    qDebug() << "**********************************************************************";

    qDebug() << QString("LVMPVINFO:====================================being");

    foreach (auto it, m_lvmInfo.m_pvInfo) {
        printPVInfo(it);
    }
    qDebug() << QString("LVMPVINFO:====================================end");
    qDebug() << "**********************************************************************";
}

void LVMOperator::resetLVMInfo(LVMInfo &m_lvmInfo)
{
    m_lvmInfo.m_pvInfo.clear();
}

/******************************** private ******************************/

bool LVMOperator::updatePVInfo(LVMInfo &m_lvmInfo)
{

    if (Utils::findProgramInPath("lsblk").isEmpty()) {
        m_lvmInfo.m_lvmErr = LVMError::LVM_Err_NO_CMD_Support;
        return  false;
    }

    if (m_lvmSupport.LVM_CMD_pvs == LVM_CMD_Support::NONE) {
        m_lvmInfo.m_lvmErr = LVMError::LVM_Err_NO_CMD_Support;
    } else {

        QString strout, strerror;
        Utils::executCmd("pvs --noheadings -o pv_name", strout, strerror);
        strout = strout.trimmed();
        foreach (QString str, strout.split("\n")) {
            if (str.isEmpty()) {
                continue;
            }

            PVInfo pv;
            QString cmd;
            pv.m_pvPath = str.trimmed();

            cmd = QString("pvs %1  --units b --noheadings --separator # "
                          "-o vg_name,pv_fmt,pv_size,pv_free,pv_uuid,pv_mda_count,pv_attr,"
                          "pv_pe_alloc_count,pv_pe_count,pv_mda_size,vg_extent_size,vg_uuid"
                          "").arg(pv.m_pvPath);
            Utils::executCmd(cmd, strout, strerror);
            QStringList pvsRes = strout.split("#");

            //获取基本属性
            pv.m_vgName = getStringListItem(pvsRes, 0).trimmed();
            pv.m_pvFmt = getStringListItem(pvsRes, 1).trimmed();
            pv.m_pvSize = Utils::LVMFormatSize(getStringListItem(pvsRes, 2).trimmed().replace('B', "").toLongLong());
            pv.m_pvFree = Utils::LVMFormatSize(getStringListItem(pvsRes, 3).trimmed().replace('B', "").toLongLong());
            pv.m_pvUuid = getStringListItem(pvsRes, 4).trimmed();
            pv.m_pvMdaCount = getStringListItem(pvsRes, 5).toInt();
            pv.m_pvStatus = getStringListItem(pvsRes, 6).trimmed();
            pv.m_pvUsedPE = getStringListItem(pvsRes, 7).toInt();
            pv.m_pvUnusedPE = getStringListItem(pvsRes, 8).toInt() - pv.m_pvUsedPE;
            pv.m_pvMdaSize = getStringListItem(pvsRes, 9).replace('B', "").toInt();
            pv.m_PESize = getStringListItem(pvsRes, 10).replace('B', "").toInt();
            pv.m_vgUuid = getStringListItem(pvsRes, 11).trimmed();

            //设备类型
            cmd = QString("lsblk %1 -o type -nd").arg(pv.m_pvPath);
            Utils::executCmd(cmd, strout, strerror);
            if (strout.contains("part")) {
                pv.m_lvmDevType = LVMDevType::LVM_Dev_Partition;
            } else if (strout.contains("disk")) {
                pv.m_lvmDevType = LVMDevType::LVM_Dev_DISK;
            } else if (strout.contains("loop")) {
                pv.m_lvmDevType = LVMDevType::LVM_Dev_LOOP;
            } else if (strout.contains("raid")) {
                pv.m_lvmDevType = LVMDevType::LVM_Dev_Meta_Devices;
            } else {
                pv.m_lvmDevType = LVMDevType::LVM_Dev_Unknow_Devices;
            }

            //pv上pe使用情况
            cmd = QString("pvs %1 -o seg_type,pvseg_all,lv_path,lv_name --noheadings  --separator ### ").arg(pv.m_pvPath);
            Utils::executCmd(cmd, strout, strerror);
            qDebug() << strout;
            foreach (QString str, strout.split("\n")) {
                QStringList list = str.trimmed().split("###");
                qDebug() << list;
                if (list.size() < 4) {
                    continue;
                }
                QVector<LV_PV_Ranges>lvVec;
                VG_PV_Ranges vgRanges;
                vgRanges.m_vgName = pv.m_vgName;
                vgRanges.m_vgUuid = pv.m_vgUuid;
                vgRanges.m_used = false;
                vgRanges.m_start = getStringListItem(list, 1).toLongLong();
                vgRanges.m_end = getStringListItem(list, 2).toLongLong() + vgRanges.m_start - 1;
                if (!list[0].contains("free")) {
                    vgRanges.m_used = true;
                    LV_PV_Ranges lvRanges = vgRanges;
                    lvRanges.m_devPath = getStringListItem(list, 3);
                    lvRanges.m_lvName = getStringListItem(list, 4);
                    lvVec.push_back(lvRanges);
                    pv.m_lvRangesList.insert(lvRanges.m_devPath, lvVec);
                }
                pv.m_vgRangesList.push_back(vgRanges);
            }
            m_lvmInfo.m_pvInfo.insert(pv.m_pvPath, pv);
        }
    }
    return m_lvmInfo.m_lvmErr == LVM_Err_Normal;
}

bool LVMOperator::updateVGInfo(LVMInfo &m_lvmInfo)
{
    if (m_lvmSupport.LVM_CMD_vgs == LVM_CMD_Support::NONE) {
        m_lvmInfo.m_lvmErr = LVMError::LVM_Err_NO_CMD_Support;
    } else {
        QString strout, strerror;
        Utils::executCmd("vgs --noheadings -o vg_uuid", strout, strerror);
        strout = strout.trimmed();

        foreach (QString str, strout.split("\n")) {
            if (str.isEmpty()) {
                continue;
            }

            VGInfo vg;
            vg.m_vgUuid = str.trimmed();

            QString cmd = QString("vgs -S vg_uuid=%1 --units b --noheadings --separator # "
                                  "-o vg_name,vg_size,vg_free,pv_count,pv_pe_count,pv_pe_alloc_count,lv_count,vg_extent_size,vg_attr "
                                  "").arg(vg.m_vgUuid);
            Utils::executCmd(cmd, strout, strerror);
            QStringList vgsRes = getStringListItem(strout.split("\n"), 0).split("#");
            if (vgsRes.size() < 9) {
                vg.m_vgError = LVMError::LVM_Err_VG_Error;
                m_lvmInfo.m_vgInfo.insert(vg.m_vgUuid, vg);
                continue;
            }

            //获取基本属性
            vg.m_vgName = getStringListItem(vgsRes, 0).trimmed();
            long long Size = getStringListItem(vgsRes, 1).trimmed().replace('B', "").toLongLong();
            long long unUsed = getStringListItem(vgsRes, 2).trimmed().replace('B', "").toLongLong();
            vg.m_vgSize = Utils::LVMFormatSize(Size);
            vg.m_vgUnused = Utils::LVMFormatSize(unUsed);
            vg.m_vgUsed = Utils::LVMFormatSize(Size - unUsed);
            vg.m_pvCount = getStringListItem(vgsRes, 3).toInt();
            vg.m_peCount = getStringListItem(vgsRes, 4).toInt();;
            vg.m_peUsed  = getStringListItem(vgsRes, 5).toInt();;
            vg.m_peUnused = vg.m_peCount - vg.m_peUsed;
            vg.m_curLV  = getStringListItem(vgsRes, 6).toInt();
            vg.m_PESize = getStringListItem(vgsRes, 7).trimmed().replace('B', "").toInt();
            vg.m_vgStatus = getStringListItem(vgsRes, 8).trimmed();
            updateLVInfo(m_lvmInfo, vg);
            cmd = QString("vgs -S vg_uuid=%1 --noheadings -o pv_name").arg(vg.m_vgUuid);
            Utils::executCmd(cmd, strout, strerror);
            foreach (QString str, strout.trimmed().split("\n")) {
                vg.m_pvList.push_back(str.trimmed());
            }
            m_lvmInfo.m_vgInfo.insert(vg.m_vgUuid, vg);
        }
    }

    return m_lvmInfo.m_lvmErr == LVM_Err_Normal;
}

bool LVMOperator::updateLVInfo(LVMInfo &m_lvmInfo, VGInfo &vg)
{
    if (m_lvmSupport.LVM_CMD_lvs == LVM_CMD_Support::NONE) {
        m_lvmInfo.m_lvmErr = LVMError::LVM_Err_NO_CMD_Support;
    } else {

        QString strout, strerror;
        QString cmd = QString("lvs -S vg_uuid=%1 --noheadings -o lv_path").arg(vg.m_vgUuid);
        Utils::executCmd(cmd, strout, strerror);
        strout = strout.trimmed();
        foreach (QString str, strout.split("\n")) {
            if (str.isEmpty()) {
                continue;
            }
            LVInfo lv;
            //基本属性
            lv.m_lvPath = str.trimmed(); //lv名称 lv0 lv1 ...
            lv.m_vgName = vg.m_vgName; //vg名称
            lv.m_LESize = vg.m_PESize;  //单个pe大小 与所在vg的pe大小相同 单位byte

            QString cmd = QString("lvs %1 -S vg_uuid=%2 --units b --noheadings --separator # "
                                  " -o lv_name,lv_uuid,lv_attr,lv_size"
                                  "").arg(lv.m_lvPath).arg(vg.m_vgUuid);
            Utils::executCmd(cmd, strout, strerror);
            QStringList lvsRes = strout.split("#");


            lv.m_lvName = getStringListItem(lvsRes, 0).trimmed();
            lv.m_lvUuid = getStringListItem(lvsRes, 1).trimmed(); //lv uuid 唯一名称
            lv.m_lvStatus = getStringListItem(lvsRes, 2).trimmed();
            long long Size = getStringListItem(lvsRes, 3).trimmed().replace('B', "").toLongLong();
            lv.m_lvSize = Utils::LVMFormatSize(Size); //字符串类型 展示用
            lv.m_lvLECount = Size / lv.m_LESize;

            lv.m_fsUsed;
            lv.m_fsUnused;
            lv.m_lvFsType;
            lv.m_busy;
            lv.m_mountpoints;
            lv.m_minReduceSize = Utils::getFsReduceMin(lv.m_lvPath, lv.m_lvFsType);
            vg.m_lvlist.push_back(lv);
        }
    }

    return m_lvmInfo.m_lvmErr == LVM_Err_Normal;
}

void LVMOperator::printPVInfo(const PVInfo &info)
{
    qDebug() << "PVInfo:====================================being";
    qDebug() << "vg_name: " << info.m_vgName;
    qDebug() << "pvFmt: " << info.m_pvFmt; //pv格式 lvm1 lvm2
    qDebug() << "pvPath: " << info.m_pvPath; //pv路径 /dev/sdb1 ...
    qDebug() << "pvUuid: " << info.m_pvUuid; //pv uuid 唯一名称
    qDebug() << "pvMdaSize: " << info.m_pvMdaSize; //pv metadata size
    qDebug() << "pvMdaCount: " << info.m_pvMdaCount; //pv metadata 个数
    qDebug() << "pvSize: " << info.m_pvSize; //字符串类型 展示用
    qDebug() << "pvFree: " << info.m_pvFree; //字符串类型 展示用
    qDebug() << "pvUsedPE: " << info.m_pvUsedPE; //已经使用pe个数
    qDebug() << "pvUnusedPE: " << info.m_pvUnusedPE; //未使用pe个数
    qDebug() << "PESize: " << info.m_PESize; //单个pe大小
    qDebug() << "pvStatus: " << info.m_pvStatus; //状态
    qDebug() << "pvError: " << info.m_pvError; //物理卷错误码

    for (auto it = info.m_lvRangesList.begin(); it != info.m_lvRangesList.end(); ++it) {
        //lv pv分布情况 vgName
        foreach (const LV_PV_Ranges &tmp, it.value()) {
            qDebug() << "";
            qDebug() << "lvName :" << tmp.m_lvName;
            qDebug() << "lv path:" << tmp.m_devPath ;
            qDebug() << "lv start:" << tmp.m_start ;
            qDebug() << "lv end:" << tmp.m_end ;
            qDebug() << "";
        }
    }


    foreach (const VG_PV_Ranges &tmp, info.m_vgRangesList) {
        //vg pv分布情况
        qDebug() << "";
        qDebug() << "vgName :" << tmp.m_vgName;
        qDebug() << "vg used:" << tmp.m_used ;
        qDebug() << "vg start:" << tmp.m_start ;
        qDebug() << "vg end:" << tmp.m_end ;
        qDebug() << "";
    }
    qDebug() << "lvmDevType: " << info.m_lvmDevType; //lvm 设备类型
    qDebug() << "PVInfo:====================================end";
}

void LVMOperator::printVGInfo(const VGInfo &info)
{
    qDebug() << "VGInfo:====================================being";
    qDebug() << "vgName: " << info.m_vgName; //vg名称 vg0, vg1 ....
    qDebug() << "vgUuid: " << info.m_vgUuid; //vg唯一名称 uuid
    qDebug() << "vgSize: " << info.m_vgSize; //字符串类型 展示用  vg总大小
    qDebug() << "vgUsed: " << info.m_vgUsed; //字符串类型 展示用  vg使用
    qDebug() << "vgUnused: " << info.m_vgUnused; //字符串类型 展示用 vg已用
    qDebug() << "pvCount: " << info.m_pvCount; //物理卷个数
    qDebug() << "peCount: " << info.m_peCount; //pe个数
    qDebug() << "peUsed: " << info.m_peUsed; //pe使用个数
    qDebug() << "peUnused: " << info.m_peUnused; //pe未使用个数
    qDebug() << "PESize: " << info.m_PESize; //单个pe大小
    qDebug() << "curLV: " << info.m_curLV; //当前lv个数
    qDebug() << "vgStatus: " << QString(info.m_vgStatus); //状态
    qDebug() << "vgError: " << info.m_vgError; //逻辑卷组错误码

    foreach (auto it, info.m_lvlist) {
        printLVInfo(it);
    }
    qDebug() << "pvList: " << info.m_pvList; //逻辑卷pv
    qDebug() << "VGInfo:====================================end";
}

void LVMOperator::printLVInfo(const LVInfo &info)
{
    qDebug() << "LVInfo:====================================being";
    qDebug() << "vgName: " << info.m_vgName; //vg名称
    qDebug() << "lvPath: " << info.m_lvPath; //lv路径
    qDebug() << "lvUuid: " << info.m_lvUuid; //lv uuid 唯一名称
    qDebug() << "lvName: " << info.m_lvName; //lv名称 lv0 lv1 ....
    qDebug() << "lvFsType: " << Utils::fileSystemTypeToString(info.m_lvFsType); //文件系统类型
    qDebug() << "lvSize: " << info.m_lvSize; //字符串类型 展示用
    qDebug() << "lvLECount: " << info.m_lvLECount; //le个数
    qDebug() << "fsUsed: " << info.m_fsUsed; //文件已经使用大小
    qDebug() << "fsUnused: " << info.m_fsUnused; //文件未使用大小
    qDebug() << "LESize: " << info.m_LESize; //单个pe大小 与所在vg的pe大小相同
    qDebug() << "busy: " << info.m_busy; //挂载标志
    qDebug() << "mountpoints: " << info.m_mountpoints;
    qDebug() << "lvStatus: " << QString(info.m_lvStatus);
    qDebug() << "minReduceSize: " << info.m_minReduceSize; //文件系统最小可缩小大小
    qDebug() << "lvError: " << info.m_lvError; //逻辑卷错误码
    qDebug() << "LVInfo:====================================end";

}

bool LVMOperator::initSuport()
{
    if (!m_initSupport) {
        m_lvmSupport.LVM_CMD_lvchange = getExecSupport("lvchange");
        m_lvmSupport.LVM_CMD_lvconvert = getExecSupport("lvconvert");
        m_lvmSupport.LVM_CMD_lvcreate = getExecSupport("lvcreate");
        m_lvmSupport.LVM_CMD_lvdisplay = getExecSupport("lvdisplay");
        m_lvmSupport.LVM_CMD_lvextend = getExecSupport("lvextend");
        m_lvmSupport.LVM_CMD_lvreduce = getExecSupport("lvreduce");
        m_lvmSupport.LVM_CMD_lvremove = getExecSupport("lvremove");
        m_lvmSupport.LVM_CMD_lvrename = getExecSupport("lvrename");
        m_lvmSupport.LVM_CMD_lvresize = getExecSupport("lvresize");
        m_lvmSupport.LVM_CMD_lvs = getExecSupport("lvs");
        m_lvmSupport.LVM_CMD_lvscan = getExecSupport("lvscan");
        m_lvmSupport.LVM_CMD_pvchange = getExecSupport("pvchange");
        m_lvmSupport.LVM_CMD_pvck = getExecSupport("pvck");
        m_lvmSupport.LVM_CMD_pvcreate = getExecSupport("pvcreate");
        m_lvmSupport.LVM_CMD_pvdisplay = getExecSupport("pvdisplay");
        m_lvmSupport.LVM_CMD_pvmove = getExecSupport("pvremove");
        m_lvmSupport.LVM_CMD_pvremove = getExecSupport("pvremove");
        m_lvmSupport.LVM_CMD_pvresize = getExecSupport("pvresize");
        m_lvmSupport.LVM_CMD_pvs = getExecSupport("pvs");
        m_lvmSupport.LVM_CMD_pvscan = getExecSupport("pvscan");
        m_lvmSupport.LVM_CMD_vgcfgbackup = getExecSupport("vgcfgbackup");
        m_lvmSupport.LVM_CMD_vgcfgrestore = getExecSupport("vgcfgbackup");
        m_lvmSupport.LVM_CMD_vgchange = getExecSupport("vgcfgrestore");
        m_lvmSupport.LVM_CMD_vgck = getExecSupport("vgck");
        m_lvmSupport.LVM_CMD_vgconvert = getExecSupport("vgconvert");
        m_lvmSupport.LVM_CMD_vgcreate = getExecSupport("vgcreate");
        m_lvmSupport.LVM_CMD_vgdisplay = getExecSupport("vgdisplay");
        m_lvmSupport.LVM_CMD_vgexport = getExecSupport("vgexport");
        m_lvmSupport.LVM_CMD_vgextend = getExecSupport("vgextend");
        m_lvmSupport.LVM_CMD_vgimport = getExecSupport("vgimport");
        m_lvmSupport.LVM_CMD_vgimportclone = getExecSupport("vgimportclone");
        m_lvmSupport.LVM_CMD_vgmerge = getExecSupport("vgmerge");
        m_lvmSupport.LVM_CMD_vgmknodes = getExecSupport("vgmknodes");
        m_lvmSupport.LVM_CMD_vgreduce = getExecSupport("vgreduce");
        m_lvmSupport.LVM_CMD_vgremove = getExecSupport("vgremove");
        m_lvmSupport.LVM_CMD_vgrename = getExecSupport("vgrename");
        m_lvmSupport.LVM_CMD_vgs = getExecSupport("vgs");
        m_lvmSupport.LVM_CMD_vgscan = getExecSupport("vgscan");
        m_lvmSupport.LVM_CMD_vgsplit = getExecSupport("vgsplit");
        m_initSupport = true;
    }
    return m_initSupport;
}

QString LVMOperator::getStringListItem(const QStringList &list, int i)
{
    return (list.size() >= i) ? list[i] : QString();
}
