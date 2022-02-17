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
#ifndef LVMENUM_H
#define LVMENUM_H

#include <QDBusArgument>


//new by liuwh 2022/1/17
/**
 * @enum LVMPVAction
 * @brief lvm 动作
 */
enum LVMAction {
    LVM_ACT_UNkNOW,     //未知动作
    LVM_ACT_ADDPV,      //添加PV
    LVM_ACT_DELETEPV,   //删除PV
    LVM_ACT_PVMOVE      //移动PV 暂时未用
};

//new by liuwh 2022/1/17
/**
 * @enum LVMDevType
 * @brief lvm 设备类型
 */
enum LVMDevType {
    LVM_DEV_UNKNOW_DEVICES,         //未知设备
    LVM_DEV_DISK,                   //磁盘
    LVM_DEV_UNALLOCATED_PARTITION,  //未分配的分区
    LVM_DEV_PARTITION,              //分区
    LVM_DEV_LOOP,                   // loop设备
    LVM_DEV_META_DEVICES            //元数据设备 raid 加密磁盘映射等
};

enum LVMError {
    LVM_ERR_NORMAL,             //正常
    LVM_ERR_NO_CMD_SUPPORT,     //外部命令不支持
    LVM_ERR_LV,                 //lv 错误
    LVM_ERR_VG,                 //vg 错误
    LVM_ERR_VG_ALREADY_EXISTS,  //VG重名
    LVM_ERR_VG_NO_EXISTS,        //vg不存在
    LVM_ERR_LV_ARGUMENT         //lv 创建参数错误
};


//new by liuwh 2022/1/20
/**
 * @enum LVM_CMD_Support
 * @brief lvm 命令支持
 */

struct LVM_CMD_Support {
    enum LVM_Support {
        NONE = 0, //not support
        EXTERNAL = 1 //using external func
    };

    LVM_Support LVM_CMD_lvchange = NONE;
    LVM_Support LVM_CMD_lvconvert = NONE;
    LVM_Support LVM_CMD_lvcreate = NONE;
    LVM_Support LVM_CMD_lvdisplay = NONE;
    LVM_Support LVM_CMD_lvextend = NONE;
    LVM_Support LVM_CMD_lvreduce = NONE;
    LVM_Support LVM_CMD_lvremove = NONE;
    LVM_Support LVM_CMD_lvrename = NONE;
    LVM_Support LVM_CMD_lvresize = NONE;
    LVM_Support LVM_CMD_lvs = NONE;
    LVM_Support LVM_CMD_lvscan = NONE;
    LVM_Support LVM_CMD_pvchange = NONE;
    LVM_Support LVM_CMD_pvck = NONE;
    LVM_Support LVM_CMD_pvcreate = NONE;
    LVM_Support LVM_CMD_pvdisplay = NONE;
    LVM_Support LVM_CMD_pvmove = NONE;
    LVM_Support LVM_CMD_pvremove = NONE;
    LVM_Support LVM_CMD_pvresize = NONE;
    LVM_Support LVM_CMD_pvs = NONE;
    LVM_Support LVM_CMD_pvscan = NONE;
    LVM_Support LVM_CMD_vgcfgbackup = NONE;
    LVM_Support LVM_CMD_vgcfgrestore = NONE;
    LVM_Support LVM_CMD_vgchange = NONE;
    LVM_Support LVM_CMD_vgck = NONE;
    LVM_Support LVM_CMD_vgconvert = NONE;
    LVM_Support LVM_CMD_vgcreate = NONE;
    LVM_Support LVM_CMD_vgdisplay = NONE;
    LVM_Support LVM_CMD_vgexport = NONE;
    LVM_Support LVM_CMD_vgextend = NONE;
    LVM_Support LVM_CMD_vgimport = NONE;
    LVM_Support LVM_CMD_vgimportclone = NONE;
    LVM_Support LVM_CMD_vgmerge = NONE;
    LVM_Support LVM_CMD_vgmknodes = NONE;
    LVM_Support LVM_CMD_vgreduce = NONE;
    LVM_Support LVM_CMD_vgremove = NONE;
    LVM_Support LVM_CMD_vgrename = NONE;
    LVM_Support LVM_CMD_vgs = NONE;
    LVM_Support LVM_CMD_vgscan = NONE;
    LVM_Support LVM_CMD_vgsplit = NONE;
};



#endif // LVMENUM_H
