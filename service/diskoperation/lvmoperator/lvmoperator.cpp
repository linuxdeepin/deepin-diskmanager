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
#include "../mountinfo.h"

#include <QStringList>
#include <QDebug>
#include <set>
using namespace DiskManager;
using std::set;

LVM_CMD_Support LVMOperator::m_lvmSupport;
bool LVMOperator::m_initSupport = false;
DeviceInfoMap LVMOperator::m_devInfo;
SupportedFileSystems LVMOperator::m_supportFs;
LVMError LVMOperator::m_lvmErr = LVM_ERR_NORMAL;
/******************************** 初始化操作 ******************************/
LVMOperator::LVMOperator()
{

}

void LVMOperator::resetLVMInfo(LVMInfo &lvmInfo)
{
    lvmInfo.m_pvInfo.clear();
    lvmInfo.m_vgInfo.clear();
    lvmInfo.m_lvmErr = LVMError::LVM_ERR_NORMAL;
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
        m_supportFs.findSupportedFilesystems();
        m_initSupport = true;
    }
    return true;
}

/******************************** 更新DATA ******************************/
bool LVMOperator::getDeviceLVMData(DeviceInfoMap &dev, const LVMInfo &lvmInfo)
{
    auto pv2vg_func = [ = ](const PVInfo & pv) {
        VGData vg;
        vg.m_vgName = pv.m_vgName;
        vg.m_vgSize = pv.m_pvSize;
        vg.m_vgByteSize = pv.m_pvByteTotalSize;
        vg.m_vgUuid = pv.m_vgUuid;
        for (QMap<QString, QVector<LV_PV_Ranges>>::ConstIterator lvIt = pv.m_lvRangesList.begin(); lvIt != pv.m_lvRangesList.end(); ++lvIt) {
            LVData lvData;
            lvData.m_lvName = lvIt->begin()->m_lvName;
            lvData.m_lvPath = lvIt.key();
            long long sum = 0;
            foreach (const LV_PV_Ranges &it, lvIt.value()) {
                sum += (it.m_end - it.m_start + 1);
            }
            lvData.m_lvByteSize = sum * pv.m_PESize;
            lvData.m_lvSize = Utils::LVMFormatSize(lvData.m_lvByteSize);
            vg.m_lvList.push_back(lvData);
        }
        return vg;
    };

    for (QMap<QString, DeviceInfo>::iterator diskIter = dev.begin(); diskIter != dev.end(); ++diskIter) {
        diskIter.value().m_vglist.clear();
        diskIter->m_vgFlag = 0;
        QMap<QString, PVInfo>::ConstIterator pvIter = lvmInfo.m_pvInfo.find(diskIter->m_path);
        if (pvIter != lvmInfo.m_pvInfo.end() && diskIter.value().m_disktype == "unrecognized") { //不存在分区表 磁盘加入
            diskIter->m_vgFlag = true;
            diskIter->m_vglist.push_back(pv2vg_func(pvIter.value()));
            continue;
        }

        for (QVector<PartitionInfo>::iterator partIter = diskIter.value().m_partition.begin(); partIter != diskIter.value().m_partition.end(); ++partIter) {
            QMap<QString, PVInfo>::ConstIterator pvIter = lvmInfo.m_pvInfo.find(partIter->m_path);
            if (pvIter != lvmInfo.m_pvInfo.end()) {
                const PVInfo &pv = pvIter.value();
                diskIter->m_vgFlag = true;
                partIter->m_vgFlag = true;
                partIter->m_vgData = pv2vg_func(pv);
                diskIter->m_vglist.push_back(partIter->m_vgData);
            }
        }
    }

    return true;
}

bool LVMOperator::getDeviceDataAndLVMInfo(DeviceInfoMap &dev, LVMInfo &lvmInfo)
{
    m_devInfo = dev;
    updateLVMInfo(lvmInfo);
    getDeviceLVMData(dev, lvmInfo);
    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

/******************************** 更新info ******************************/
bool LVMOperator::updateLVMInfo(LVMInfo &lvmInfo)
{
    initSuport();
    resetLVMInfo(lvmInfo);
    updateVGInfo(lvmInfo);
    updatePVInfo(lvmInfo);
    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

bool LVMOperator::updatePVInfo(LVMInfo &lvmInfo)
{
    if (Utils::findProgramInPath("lsblk").isEmpty() || m_lvmSupport.LVM_CMD_pvs == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }

    lvmInfo.m_pvInfo.clear();

    QString strout, strerror;
    Utils::executCmd("pvs --noheadings -o pv_name", strout, strerror);
    strout = strout.trimmed();
    foreach (QString str, strout.split("\n")) {
        if (str.isEmpty()) {
            continue;
        }

        PVInfo pv;
        pv.m_pvPath = str.trimmed();
        QString cmd = QString("pvs %1  --units b --noheadings --separator # "
                              "-o vg_name,pv_fmt,pv_size,pv_free,pv_uuid,pv_mda_count,pv_attr,"
                              "pv_pe_alloc_count,pv_pe_count,pv_mda_size,vg_extent_size,vg_uuid"
                              "").arg(pv.m_pvPath);
        Utils::executCmd(cmd, strout, strerror);
        QStringList pvsRes = strout.split("#");

        //获取基本属性
        pv.m_vgName = getStringListItem(pvsRes, 0).trimmed();
        pv.m_pvFmt = getStringListItem(pvsRes, 1).trimmed();
        pv.m_pvByteTotalSize = getStringListItem(pvsRes, 2).trimmed().replace('B', "").toLongLong();
        pv.m_pvByteFreeSize = getStringListItem(pvsRes, 3).trimmed().replace('B', "").toLongLong();
        pv.m_pvSize = Utils::LVMFormatSize(pv.m_pvByteTotalSize);
        pv.m_pvFree = Utils::LVMFormatSize(pv.m_pvByteFreeSize);
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
            pv.m_lvmDevType = LVMDevType::LVM_DEV_PARTITION;
        } else if (strout.contains("disk")) {
            pv.m_lvmDevType = LVMDevType::LVM_DEV_DISK;
        } else if (strout.contains("loop")) {
            pv.m_lvmDevType = LVMDevType::LVM_DEV_LOOP;
        } else if (strout.contains("raid")) {
            pv.m_lvmDevType = LVMDevType::LVM_DEV_META_DEVICES;
        } else {
            pv.m_lvmDevType = LVMDevType::LVM_DEV_UNKNOW_DEVICES;
        }

        //pv上pe使用情况
        cmd = QString("pvs %1 -o seg_type,pvseg_all,lv_path,lv_name --noheadings  --separator ### ").arg(pv.m_pvPath);
        Utils::executCmd(cmd, strout, strerror);
        foreach (QString str, strout.split("\n")) {
            QStringList list = str.trimmed().split("###");
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
        lvmInfo.m_pvInfo.insert(pv.m_pvPath, pv);
    }

    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

bool LVMOperator::updateVGInfo(LVMInfo &lvmInfo)
{
    if (m_lvmSupport.LVM_CMD_vgs == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }
    //查看是否有重名vg存在 如果存在 将其中一个vg更换名称
    if (!checkVG()) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_VG_ALREADY_EXISTS);
    }

    lvmInfo.m_vgInfo.clear();

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
                              "-o vg_name,vg_size,vg_free,pv_count,vg_extent_count,vg_free_count,lv_count,vg_extent_size,vg_attr "
                              "").arg(vg.m_vgUuid);
        Utils::executCmd(cmd, strout, strerror);
        QStringList vgsRes = getStringListItem(strout.split("\n"), 0).split("#");
        if (vgsRes.size() < 9) {
            vg.m_vgError = LVMError::LVM_ERR_VG;
            vg.m_vgName = getStringListItem(vgsRes, 0).trimmed();
            lvmInfo.m_vgInfo.insert(vg.m_vgName, vg);
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
        vg.m_peUnused  = getStringListItem(vgsRes, 5).toInt();;
        vg.m_peUsed = vg.m_peCount - vg.m_peUnused;
        vg.m_curLV  = getStringListItem(vgsRes, 6).toInt();
        vg.m_PESize = getStringListItem(vgsRes, 7).trimmed().replace('B', "").toInt();
        vg.m_vgStatus = getStringListItem(vgsRes, 8).trimmed();
        //获取lv基本属性
        updateLVInfo(lvmInfo, vg);
        //获取vg下pv
        cmd = QString("vgs -S vg_uuid=%1 --noheadings -o pv_name").arg(vg.m_vgUuid);
        Utils::executCmd(cmd, strout, strerror);
        foreach (QString str, strout.trimmed().split("\n")) {
            vg.m_pvList.push_back(str.trimmed());
        }
        lvmInfo.m_vgInfo.insert(vg.m_vgName, vg);
    }

    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

bool LVMOperator::updateLVInfo(LVMInfo &lvmInfo, VGInfo &vg)
{
    if (m_lvmSupport.LVM_CMD_lvs == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }

    vg.m_lvlist.clear();

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

        QString lvPath = QString("/dev/mapper/%1-%2").arg(lv.m_vgName).arg(lv.m_lvName);
        auto devIt = m_devInfo.find(lvPath);
        if (devIt != m_devInfo.end()) {
            auto partIt = devIt.value().m_partition.begin();
            if (partIt != devIt.value().m_partition.end() && partIt->m_devicePath == lvPath) {
                PartitionInfo part = *partIt;
                lv.m_busy = part.m_busy;
                lv.m_mountPoints = part.m_mountPoints;
                lv.m_lvFsType = static_cast<FSType>(part.m_fileSystemType);
                lv.m_fsUsed = part.m_sectorsUsed * devIt.value().m_sectorSize;
                lv.m_fsUnused = part.m_sectorsUnused * devIt.value().m_sectorSize;
            }
            lv.m_minReduceSize = getFsReduceMin(lv.m_lvPath, lv.m_lvFsType);
            if (-1 == lv.m_minReduceSize) {
                lv.m_minReduceSize = (lv.m_fsUsed + lv.m_fsUnused);
            }

        }
        vg.m_lvlist.push_back(lv);
    }

    LVInfo unallocLv;
    unallocLv.m_lvPath = "Unallocated";
    unallocLv.m_vgName = vg.m_vgName; //vg名称
    unallocLv.m_LESize = vg.m_PESize;
    long long Size = vg.m_peUnused * vg.m_PESize;
    unallocLv.m_lvSize = Utils::LVMFormatSize(Size); //字符串类型 展示用
    unallocLv.m_lvLECount = Size / unallocLv.m_LESize;
    unallocLv.m_fsUsed = -1;
    unallocLv.m_fsUnused = -1;
    unallocLv.m_lvFsType = FSType::FS_UNALLOCATED;
    unallocLv.m_busy = false;
    unallocLv.m_minReduceSize = 0;

    vg.m_lvlist.push_back(unallocLv);

    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

/******************************** lvm操作 ******************************/
bool LVMOperator::createVG(LVMInfo &lvmInfo, QString vgName, QList<PVData> devList, long long size)
{
    if (m_lvmSupport.LVM_CMD_vgcreate == LVM_CMD_Support::NONE || m_lvmSupport.LVM_CMD_pvcreate == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }

    //判断vg是否已经存在 存在退出
    auto vgIt = lvmInfo.m_vgInfo.find(vgName);
    if (vgIt != lvmInfo.m_vgInfo.end()) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_VG_ALREADY_EXISTS);
    }
    //创建pv
    QString vgPV, strout, strerror;
    foreach (PVData pv, devList) {
        if (Utils::executCmd(QString("pvcreate %1").arg(pv.m_devicePath), strout, strerror) != 0) {
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_PV_CREATE_FAILED);
        }
        vgPV += (" " + pv.m_devicePath);
    }

    //创建vg
    if (Utils::executCmd(QString("vgcreate %1 %2").arg(vgName).arg(vgPV), strout, strerror) != 0) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_VG_CREATE_FAILED);
    }

    //更新lvm数据
    updateLVMInfo(lvmInfo);
    return setLVMErr(lvmInfo, lvmInfo.m_vgInfo.find(vgName) != lvmInfo.m_vgInfo.end() ? LVM_ERR_NORMAL : LVM_ERR_VG_CREATE_FAILED);

}

bool LVMOperator::createLV(LVMInfo &lvmInfo, QString vgName, QList<LVAction> lvList)
{
    if (m_lvmSupport.LVM_CMD_lvcreate == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }

    //判断参数是否正确
    if (vgName.isEmpty()  || lvList.count() == 0) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_LV_ARGUMENT);
    }

    //vg是否存在
    auto vgIt = lvmInfo.m_vgInfo.find(vgName);
    if (lvmInfo.m_vgInfo.end() == vgIt) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_VG_NO_EXISTS);
    }

    VGInfo &vg = vgIt.value();
    long long vgSize = vg.m_peUnused * vg.m_PESize;
    set<QString>s_tmp;
    foreach (LVAction clv, lvList) {
        vgSize -= clv.m_lvByteSize;
        if (vgSize < 0  || clv.m_user.isEmpty() || clv.m_vgName != vgName
                || clv.m_lvName.isEmpty() || clv.m_lvName.count() >= 128 || clv.m_lvByteSize <= 0  // lv大小不超过vg未使用空间 文件系统支持 lv是否重复 用户名是否存在等条件
                || m_supportFs.getFsObject(clv.m_lvFs) == nullptr   //判断文件系统是否支持
                || !(s_tmp.insert(clv.m_lvName).second)) {          //插入失败 说明lvList 列表中存在重复的lv名称
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_LV_ARGUMENT);
        }

        //查找vg中是否已经存在了lvName 存在报错退出
        auto lvIt = std::find_if(vg.m_lvlist.begin(), vg.m_lvlist.end(), [ = ](const LVInfo & lvInfo)->bool{
            return lvInfo.m_lvName == clv.m_lvName;
        });

        if (lvIt != vg.m_lvlist.end()) {
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_LV_ARGUMENT);
        }
    }

    foreach (const LVAction &clv, lvList) {
        //创建lv
        QString strout, strerror;
        if (Utils::executCmd(QString("lvcreate -L %1b -n %2 %3 -y").arg(clv.m_lvByteSize).arg(clv.m_lvName).arg(clv.m_vgName), strout, strerror) != 0) {
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_LV_CREATE_FAILED);
        }

        Partition p;
        p.setPath(QString("/dev/%1/%2").arg(clv.m_vgName).arg(clv.m_lvName));
        if (!m_supportFs.getFsObject(clv.m_lvFs)->create(p)) { //创建文件系统  上面已经判断过指针  不需要重复判断
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_LV_CREATE_FS_FAILED);
        }
    }
    updateLVInfo(lvmInfo, vg);
    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

bool LVMOperator::deleteVG(LVMInfo &lvmInfo, QStringList vglist)
{
    if (m_lvmSupport.LVM_CMD_pvremove == LVM_CMD_Support::NONE
            || m_lvmSupport.LVM_CMD_vgremove == LVM_CMD_Support::NONE
            || m_lvmSupport.LVM_CMD_lvremove == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }
    foreach (const QString &vgName, vglist) {
        auto it = lvmInfo.m_vgInfo.find(vgName);
        if (lvmInfo.m_vgInfo.end() == it) {  //vg 不存在 报错返回
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_VG_NO_EXISTS);
        }
        if (!p_deleteVG(*it)) { //vg删除失败 报错返回 需要修改错误枚举
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_VG_DELETE_FAILED);
        }
    }
    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

bool LVMOperator::deleteLV(LVMInfo &lvmInfo, QStringList lvlist)
{
    if (m_lvmSupport.LVM_CMD_lvremove == LVM_CMD_Support::NONE) {
        return setLVMErr(lvmInfo, LVMError::LVM_ERR_NO_CMD_SUPPORT);
    }
    foreach (const QString &lvPath, lvlist) {
        if (!p_deleteLV(lvPath)) {
            return setLVMErr(lvmInfo, LVMError::LVM_ERR_LV_DELETE_FAILED);
        }
    }
    return setLVMErr(lvmInfo, LVMError::LVM_ERR_NORMAL);
}

bool LVMOperator::resizeVG(LVMInfo &lvmInfo, QString vgName, QList<PVData> devList, long long size)
{
    qDebug() << __FUNCTION__;
    return true;
}

bool LVMOperator::resizeLV(LVMInfo &lvmInfo,  LVAction &lvAction)
{


    return false;
}

Byte_Value LVMOperator::getFsReduceMin(const QString &devPath, const FSType &fs)
{
    Partition p;
    p.setPath(devPath);
    //qDebug() << __FUNCTION__ << 1 <<__LINE__ << devPath<<fs;
    FileSystem *f = m_supportFs.getFsObject(fs);
    if (f) {
        //qDebug() << __FUNCTION__ << 2 <<__LINE__ << devPath<<fs;
        return f->getFilesystemLimits(p).min_size;
    }
    //qDebug() << __FUNCTION__ << 3 <<__LINE__ << devPath<<fs;
    return -1;
}

bool LVMOperator::vgRename(QString uuid, QString newName)
{
    if (m_lvmSupport.LVM_CMD_vgrename == LVM_CMD_Support::NONE) {
        return false;
    }
    QString strout, strerror;
    return Utils::executCmd(QString("vgrename %1 %2").arg(uuid).arg(newName), strout, strerror) == 0;
}

bool LVMOperator::checkVG()
{
    struct VG {
        QString vgName;
        QString vgUuid;

        bool operator<(const VG &tmp)const
        {
            return this->vgUuid < tmp.vgUuid;
        }
    } ;

    QString strout, strerror;
    Utils::executCmd("vgs --noheadings --separator # "
                     "-o vg_name,vg_uuid", strout, strerror);
    strout = strout.trimmed();

    //获取所有vg集合
    QMap<QString, set<VG>>vgList;
    foreach (const QString &str, strout.split("\n")) {
        QStringList list = str.trimmed().split("#");
        if (list.count() == 2) {
            VG vg { list[0], list[1]};
            auto it = vgList.find(vg.vgName);
            if (it != vgList.end()) {
                it.value().insert(vg);
            } else {
                vgList.insert(vg.vgName, set<VG>() = {vg});
            }
        }
    }

    //查找重复
    QMap<QString, set<VG>>copy = vgList;
    QVector<VG> renameVg;
    for (QMap<QString, set<VG>>::iterator  it = vgList.begin(); it != vgList.end(); ++it) {
        int i = 1;
        for (auto vit = (++it.value().begin()); vit != it.value().end(); ++vit) {
            QString tmpVgName =  QString("%1_%2").arg(vit->vgName).arg(i);
            auto mit = copy.find(tmpVgName);
            while (mit != copy.end()) {
                tmpVgName =  QString("%1_%2").arg(vit->vgName).arg(++i);
                mit = copy.find(tmpVgName);
            }
            VG vg{tmpVgName, vit->vgUuid};
            copy.insert(tmpVgName, set<VG>() = {vg});
            renameVg.push_back(vg);
        }
    }

    //修改名称
    foreach (const VG &vg, renameVg) {
        vgRename(vg.vgUuid, vg.vgName);
    }
    return true;
}

bool LVMOperator::p_deleteLV(const QString &lvPath)
{
    if (DiskManager::MountInfo::getMountedMountpoints(lvPath).count() == 0) {
        QString strout, strerror;
        return Utils::executCmd(QString("lvremove %1 -y").arg(lvPath), strout, strerror) == 0;
    }
    return false;
}

bool LVMOperator::p_deletePV(const QString &devPath)
{
    QString strout, strerror;
    return Utils::executCmd(QString("pvremove %1 -y").arg(devPath), strout, strerror) == 0;
}

bool LVMOperator::p_deleteVG(const VGInfo &vgInfo)
{
    for (int i = 0; i < vgInfo.m_lvlist.count() - 1; ++i) { //删除lv
        if (!p_deleteLV(vgInfo.m_lvlist[i].m_lvPath)) { //最后一个lv不要删除 前端显示占位用
            return false;
        }
    }

    QString strout, strerror;
    if (Utils::executCmd(QString("vgremove %1 -y").arg(vgInfo.m_vgName), strout, strerror) != 0) { //删除vg
        return false;
    }

    foreach (const QString &devPath, vgInfo.m_pvList) { //删除pv
        if (!p_deletePV(devPath)) {
            return false;
        }
    }

    return true;
}

/******************************** 打印输出 ******************************/

void LVMOperator::printError(const LVMError &error)
{
    switch (error) {
    case LVMError::LVM_ERR_NORMAL:
        qDebug() << "LVM_ERR_NORMAL";
        break;
    case LVMError::LVM_ERR_NO_CMD_SUPPORT:
        qDebug() << "LVM_ERR_NO_CMD_SUPPORT";
        break;
    case LVMError::LVM_ERR_PV:
        qDebug() << "LVM_ERR_PV";
        break;
    case LVMError::LVM_ERR_PV_CREATE_FAILED:
        qDebug() << "LVM_ERR_PV_CREATE_FAILED";
        break;
    case LVMError::LVM_ERR_PV_DELETE_FAILED:
        qDebug() << "LVM_ERR_PV_DELETE_FAILED";
        break;
    case LVMError::LVM_ERR_LV:
        qDebug() << "LVM_ERR_LV";
        break;
    case LVMError::LVM_ERR_LV_CREATE_FAILED:
        qDebug() << "LVM_ERR_LV_CREATE_FAILED";
        break;
    case LVMError::LVM_ERR_LV_ARGUMENT:
        qDebug() << "LVM_ERR_LV_ARGUMENT";
        break;
    case LVMError::LVM_ERR_LV_NO_EXISTS:
        qDebug() << "LVM_ERR_LV_NO_EXISTS";
        break;
    case LVMError::LVM_ERR_LV_DELETE_FAILED:
        qDebug() << "LVM_ERR_LV_DELETE_FAILED";
        break;
    case LVMError::LVM_ERR_VG:
        qDebug() << "LVM_ERR_VG";
        break;
    case LVMError::LVM_ERR_VG_CREATE_FAILED:
        qDebug() << "LVM_ERR_VG_CREATE_FAILED";
        break;
    case LVMError::LVM_ERR_VG_ARGUMENT:
        qDebug() << "LVM_ERR_VG_ARGUMENT";
        break;
    case LVMError::LVM_ERR_VG_NO_EXISTS:
        qDebug() << "LVM_ERR_VG_NO_EXISTS";
        break;
    case LVMError::LVM_ERR_VG_ALREADY_EXISTS:
        qDebug() << "LVM_ERR_VG_ALREADY_EXISTS";
        break;
    case LVMError::LVM_ERR_VG_DELETE_FAILED:
        qDebug() << "LVM_ERR_VG_DELETE_FAILED";
        break;
    default:
        qDebug() << "LVMError default";
    }
}

void LVMOperator::printLVMInfo(const LVMInfo &lvmInfo)
{
    qDebug() << "**********************************************************************";
    qDebug() << QString("LVMVGINFO:====================================being");
    foreach (auto it, lvmInfo.m_vgInfo) {
        printVGInfo(it);
    }
    qDebug() << QString("LVMVGINFO:====================================end");
    qDebug() << "**********************************************************************";

    qDebug() << QString("LVMPVINFO:====================================being");

    foreach (auto it, lvmInfo.m_pvInfo) {
        printPVInfo(it);
    }
    qDebug() << QString("LVMPVINFO:====================================end");
    qDebug() << "**********************************************************************";
}

void LVMOperator::printDeviceLVMData(const DeviceInfoMap &dev)
{
    auto printVGData = [ = ](const VGData & vg) {
        qDebug() << "vgName" << vg.m_vgName;
        qDebug() << "vgSize" << vg.m_vgSize;
        qDebug() << "m_vgUuid" << vg.m_vgUuid;
        qDebug() << "vgByteSize" << vg.m_vgByteSize;
        foreach (auto lvIt, vg.m_lvList) {
            qDebug() << "";
            qDebug() << "lvName" << lvIt.m_lvName;
            qDebug() << "lvPath" << lvIt.m_lvPath;
            qDebug() << "lvSize" << lvIt.m_lvSize;
            qDebug() << "lvByteSize" << lvIt.m_lvByteSize;
            qDebug() << "";
        }
    };

    qDebug() << "**********************************************************************";
    foreach (auto it, dev) {
        if (it.m_vgFlag) {
            qDebug() << QString("dev disk %1 LVMData:====================================being").arg(it.m_path);
            qDebug() << "vgData COUNT:" << it.m_vglist.count();
            foreach (auto vgIt, it.m_vglist) {
                printVGData(vgIt);
            }
            qDebug() << QString("dev disk %1 LVMData:====================================end").arg(it.m_path);
        }

        foreach (auto it2, it.m_partition) {
            if (it2.m_vgFlag) {
                qDebug() << QString("dev part %1 LVMData:====================================being").arg(it2.m_path);
                printVGData(it2.m_vgData);
                qDebug() << QString("dev part %1 LVMData:====================================end").arg(it2.m_path);
            }
        }
    }
    qDebug() << "**********************************************************************";
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
    qDebug() << "pvByteTotalSize: " << info.m_pvByteTotalSize;//pv总大小  单位byte
    qDebug() << "pvByteFreeSize: " << info.m_pvByteFreeSize;//pv未使用大小 单位byte
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
    qDebug() << "mountpoints: " << info.m_mountPoints;
    qDebug() << "lvStatus: " << QString(info.m_lvStatus);
    qDebug() << "minReduceSize: " << info.m_minReduceSize; //文件系统最小可缩小大小
    qDebug() << "lvError: " << info.m_lvError; //逻辑卷错误码
    qDebug() << "LVInfo:====================================end";
}

