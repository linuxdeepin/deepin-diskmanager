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
#ifndef LVMOPERATOR_H
#define LVMOPERATOR_H

#include "lvmstruct.h"
#include "deviceinfo.h"
#include "../supportedfilesystems.h"

class LVMOperator
{
public:
    LVMOperator();

    /**
     * @brief 更新lvm信息
     * @param lvmInfo:lvm数据结构体
     * @return true 成功 false 失败
     */
    static bool updateLVMInfo(LVMInfo &lvmInfo);

    /**
     * @brief 获取设备上lvmData
     * @param dev: 设备集合
     * @param lvmInfo:lvm数据结构体
     * @return true 成功 false 失败
     */
    static bool getDeviceLVMData(DeviceInfoMap &dev, const LVMInfo &lvmInfo);

    /**
     * @brief 更新lvm信息 并且获取设备上lvmData
     * @param dev: 设备集合
     * @param lvmInfo:lvm数据结构体
     * @return true 成功 false 失败
     */
    static bool getDeviceDataAndLVMInfo(DeviceInfoMap &dev, LVMInfo &lvmInfo);

    /**
     * @brief 清空lvm结构体数据
     * @param lvmInfo:lvm数据结构体
     */
    static void resetLVMInfo(LVMInfo &lvmInfo);

    /**
     * @brief 打印lvm错误信息
     * @param error:lvm数据结构体
     */
    static void printError(const LVMError &error);


    /**
     * @brief 打印lvm信息
     * @param lvmInfo:lvm数据结构体
     */
    static void printLVMInfo(const LVMInfo &lvmInfo);

    /**
     * @brief 打印设备上lvm信息
     * @param dev: 设备集合
     */
    static void printDeviceLVMData(const DeviceInfoMap &dev);

    /**
     * @brief 创建vg
     * @param lvmInfo: lvm数据集合
     * @param vgName:待创建vg名称
     * @param devList: pv设备集合
     * @param size:vg总大小
     * @return true 成功 false 失败
     */
    static bool createVG(LVMInfo &lvmInfo, QString vgName, QList<PVData>devList, long long size);

    /**
     * @brief 创建lv
     * @param lvmInfo: lvm数据集合
     * @param vgName:vg名称
     * @param lvList: 待创建lv列表
     * @return true 成功 false 失败
     */
    static bool createLV(LVMInfo &lvmInfo, QString vgName, QList<LVAction>lvList);

    /**
     * @brief 删除vg
     * @param lvmInfo: lvm数据集合
     * @param vglist: 待删除vg列表
     * @return true 成功 false 失败
     */
    static bool deleteVG(LVMInfo &lvmInfo, QStringList vglist);

    /**
     * @brief 删除lv
     * @param lvmInfo: lvm数据集合
     * @param lvlist: 待删除lv列表
     * @return true 成功 false 失败
     */
    static bool deleteLV(LVMInfo &lvmInfo, QStringList lvlist);

    /**
     * @brief vg空间调整
     * @param lvmInfo: lvm数据集合
     * @param vgName:vg名称
     * @param devList: pv设备集合
     * @param size:调整后vg总大小
     * @return true 成功 false 失败
     */
    static bool resizeVG(LVMInfo &lvmInfo, QString vgName, QList<PVData>devList, long long size);

    /**
     * @brief lv空间调整
     * @param lvmInfo: lvm数据集合
     * @param lvPath:lv路径
     * @param size: 调整后lv总大小
     * @return true 成功 false 失败
     */
    static bool resizeLV(LVMInfo &lvmInfo, LVAction &lvAction);
private:
    /**
     * @brief 更新lvm PV信息
     * @param lvmInfo:lvm数据结构体
     * @return true 成功 false 失败
     */
    static bool updatePVInfo(LVMInfo &lvmInfo);


    /**
     * @brief 更新lvm VG信息
     * @param lvmInfo:lvm数据结构体
     * @return true 成功 false 失败
     */
    static bool updateVGInfo(LVMInfo &lvmInfo);

    /**
     * @brief 更新lvm LV信息
     * @param lvmInfo:lvm数据结构体
     * @param info:vg数据结构体
     * @return true 成功 false 失败
     */
    static bool updateLVInfo(LVMInfo &lvmInfo, VGInfo &info);

    /**
     * @brief 打印设备上VG信息
     * @param info: VG结构体
     */
    static void printVGInfo(const VGInfo &info);

    /**
     * @brief 打印设备上LV信息
     * @param info: LV结构体
     */
    static void printLVInfo(const LVInfo &info);

    /**
     * @brief 打印设备上PV信息
     * @param info: PV结构体
     */
    static void printPVInfo(const PVInfo &info);

    /**
     * @brief 初始化系统命令支持
     * @return true 成功 false 失败
     */
    static bool initSuport();

    /**
     * @brief 检查并尝试修复系统上的VG
     * @return true 成功 false 失败
     */
    static bool checkVG();


    /**
     * @brief vg重命名
     * @param uuid:vgUUid
     * @param newName:vg新名称
     * @return true 成功 false 失败
     */
    static bool vgRename(QString uuid, QString newName);

    /**
     * @brief 获取系统命令支持
     * @param cmd: 命令
     * @return NONE 不支持 EXTERNAL 支持外部命令
     */
    static inline LVM_CMD_Support::LVM_Support getExecSupport(const QString &cmd)
    {
        return Utils::findProgramInPath(cmd).isEmpty() ? LVM_CMD_Support::NONE : LVM_CMD_Support::EXTERNAL;
    }

    /**
     * @brief 获取系统命令支持
     * @param list: 字符串列表
     * @param i:index
     * @return list指定index的字符串 返回空代表index无效
     */
    static inline QString getStringListItem(const QStringList &list, const int &i)
    {
        return (i >= 0 && list.size() > i) ? list[i] : QString();
    }


    /**
     * @brief 获取文件系统缩小最小值
     * @return 文件系统缩小最小值
     */
    static Byte_Value getFsReduceMin(const QString &devPath, const FSType &fs);


    /**
     * @brief 删除pv
     * @param devPath:pv路径
     * @return true 成功 false 失败
     */
    static bool p_deletePV(const QString &devPath);

    /**
     * @brief 删除lv
     * @param lvPath:lv路径
     * @return true 成功 false 失败
     */
    static bool p_deleteLV(const QString &lvPath);

    /**
     * @brief 删除vg
     * @param vgInfo:vg数据结构体
     * @return true 成功 false 失败
     */
    static bool p_deleteVG(const VGInfo &vgInfo);


    /**
     * @brief 设置lvm错误
     * @param lvmInfo:lvm数据
     * @param err:错误枚举
     * @return true 成功 false 失败
     */
    static inline bool setLVMErr(LVMInfo &lvmInfo, const LVMError &err)
    {
        lvmInfo.m_lvmErr = err;
        m_lvmErr = err;
        printError(err);
        return lvmInfo.m_lvmErr == LVMError::LVM_ERR_NORMAL;
    }

public:
    static LVM_CMD_Support m_lvmSupport; //lvm系统支持集合
    static bool m_initSupport;//初始化lvm系统支持标志位
    static DeviceInfoMap  m_devInfo; //磁盘属性集合
    static DiskManager::SupportedFileSystems m_supportFs; //文件系统支持集合
    static LVMError m_lvmErr;

};

#endif // LVMOPERATOR_H
