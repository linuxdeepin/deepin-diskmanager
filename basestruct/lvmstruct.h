/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file lvmstruct.h
 *
 * @brief lvm 结构体
 *
 * @date 2022-01-17
 *
 * Author: liuwenhao  <liuwenhao@uniontech.com>
 *
 * Maintainer: liuwenhao  <liuwenhao@uniontech.com>
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
#ifndef LVMSTRUCT_H
#define LVMSTRUCT_H

#include "utils.h"
#include "lvmenum.h"
#include <QDBusArgument>
#include <QVector>

//new by liuwh 2022/1/17
#define LVMStructEnd(className) Q_DECLARE_METATYPE(className)\
    QDBusArgument &operator<<(QDBusArgument &argument, const className &data);\
    const QDBusArgument &operator>>(const QDBusArgument &argument, className &data);



//new by liuwh 2022/1/17
/**
 * @class PVDATA
 * @brief pv设备信息数据结构体
 */
typedef struct PVData {
    PVData();
    QString m_devicePath;  //pv路径
    Sector m_startSector;  //开始扇区
    Sector m_endSector;    //结束扇区
    LVMAction m_pvAct;     //执行动作
    LVMDevType m_type;     //设备类型
} PVData;
LVMStructEnd(PVData)

//new by liuwh 2022/2/15
/**
 * @class CreateLVInfo
 * @brief 创建lv结构体 前后端通信用
 */
typedef struct CreateLVInfo {
    CreateLVInfo();
    QString m_vgName;       //vg名称
    QString m_lvName;       //lv名称
    QString m_lvSize;       //lv大小
    long long m_lvByteSize; //lv大小 byte单位
    FSType m_lvFs;          //文件系统类型
    QString m_user;         //当前用户名称
} CreateLVInfo;
LVMStructEnd(CreateLVInfo)


//new by liuwh 2022/1/17
/**
 * @class LVDATA
 * @brief 逻辑卷信息数据结构体
 */
typedef struct LVDATA {
    LVDATA();
    QString m_lvName;
    QString m_lvPath;  //lv path
    QString m_lvSize;
    long long m_lvByteSize;
} LVData;
LVMStructEnd(LVData)


//new by liuwh 2022/1/17
/**
 * @class VGDATA
 * @brief 逻辑卷组信息数据结构体
 */
typedef struct VGDATA {
    VGDATA();
    QString m_vgName;
    QString m_vgSize;
    QString m_vgUuid;
    long long m_vgByteSize;
    QVector <LVData>m_lvList;
} VGData;
LVMStructEnd(VGData)



//new by liuwh 2022/1/17
/**
 * @class PV_RANGES
 * @brief pv设备 使用分布范围
 */
typedef struct PVRANGES {
    PVRANGES();
    QString m_lvName;       //lvName
    QString m_devPath;      //lv path
    QString m_vgName;
    QString m_vgUuid;
    long long m_start;      //单位 pe
    long long m_end;        //单位 pe
    bool m_used;            //是否使用  针对vg lv不使用该属性
} PVRanges;
LVMStructEnd(PVRanges)

typedef PVRanges LV_PV_Ranges;
typedef PVRanges VG_PV_Ranges;



//new by liuwh 2022/1/20
/**
 * @class PVInfo
 * @brief 物理卷信息 属性结构体
 */

/*pvs -o pv_attr
 * Attr
 * a--
1 (d)uplicate复制, (a)llocatable 可分配, (u)sed 使用
2 e(x)ported 导出
3 (m)issing 丢失
*/
class PVInfo
{
public:
    PVInfo();
    bool isDuplicate() {return m_pvStatus[0] == 'd';}
    bool isAllocatable() {return m_pvStatus[0] == 'a';}//已经使用并且分配
    bool isUsed() {return m_pvStatus[0] == 'u';} //已经加入vg 但是未分配
    bool isExported() {return m_pvStatus[1] == 'x';}
    bool isMissing() {return m_pvStatus[2] == 'm';}
public:
    QString m_pvFmt;  //pv格式 lvm1 lvm2
    QString m_vgName; //vgName
    QString m_pvPath; //pv路径 /dev/sdb1 ...
    QString m_pvUuid; //pv uuid 唯一名称
    QString m_vgUuid; //vg uuid 唯一名称
    long long m_pvMdaSize;//pv metadata size 单位byte
    long long m_pvMdaCount;//pv metadata 个数
    QString m_pvSize; //字符串类型 展示用
    QString m_pvFree; //字符串类型 展示用
    long long m_pvUsedPE; //已经使用pe个数
    long long m_pvUnusedPE;//未使用pe个数
    int m_PESize;  //单个pe大小  单位byte
    QString m_pvStatus{"---"}; //状态
    LVMError m_pvError;//物理卷错误码
    QMap<QString, QVector<LV_PV_Ranges>> m_lvRangesList; //lv pv分布情况 key：lvPath  value：lv集合
    QVector<VG_PV_Ranges> m_vgRangesList; //vg pv分布情况
    LVMDevType m_lvmDevType;    //lvm 设备类型
    long long m_pvByteTotalSize;//pv总大小  单位byte
    long long m_pvByteFreeSize;//pv未使用大小 单位byte

};
LVMStructEnd(PVInfo)



//new by liuwh 2022/1/17
/**
 * @class LVInfo
 * @brief 逻辑卷信息 属性结构体
 */

/*
lvs -o lv_attr
  Attr
  -wi-a-----
  -wi-a-----

 The lv_attr bits are:
       1  Volume type: (C)ache, (m)irrored, (M)irrored without initial sync, (o)rigin, (O)rigin with merging snapshot, (r)aid, (R)aid without initial sync, (s)napshot, merging (S)napshot, (p)vmove, (v)irtual, mirror or  raid  (i)mage,  mirror  or  raid  (I)mage  out-
          of-sync, mirror (l)og device, under (c)onversion, thin (V)olume, (t)hin pool, (T)hin pool data, raid or pool m(e)tadata or pool metadata spare.
       2  Permissions: (w)riteable, (r)ead-only, (R)ead-only activation of non-read-only volume
       3  Allocation policy:  (a)nywhere, (c)ontiguous, (i)nherited, c(l)ing, (n)ormal This is capitalised if the volume is currently locked against allocation changes, for example during pvmove(8).
       4  fixed (m)inor
       5  State:  (a)ctive, (h)istorical, (s)uspended, (I)nvalid snapshot, invalid (S)uspended snapshot, snapshot (m)erge failed, suspended snapshot (M)erge failed, mapped (d)evice present without tables, mapped device present with (i)nactive table, thin-pool (c)heck
          needed, suspended thin-pool (C)heck needed, (X) unknown
       6  device (o)pen, (X) unknown
       7  Target type: (C)ache, (m)irror, (r)aid, (s)napshot, (t)hin, (u)nknown, (v)irtual.  This groups logical volumes related to the same kernel target together.  So, for example, mirror images, mirror logs as well as mirrors themselves appear as (m) if  they  use
          the  original  device-mapper  mirror kernel driver; whereas the raid equivalents using the md raid kernel driver all appear as (r).  Snapshots using the original device-mapper driver appear as (s); whereas snapshots of thin volumes using the new thin provi‐
          sioning driver appear as (t).
       8  Newly-allocated data blocks are overwritten with blocks of (z)eroes before use.
       9  Volume Health, where there are currently three groups of attributes identified:
          Common ones for all Logical Volumes: (p)artial, (X) unknown.
          (p)artial signifies that one or more of the Physical Volumes this Logical Volume uses is missing from the system. (X) unknown signifies the status is unknown.

          Related to RAID Logical Volumes: (r)efresh needed, (m)ismatches exist, (w)ritemostly.
          (r)efresh signifies that one or more of the Physical Volumes this RAID Logical Volume uses had suffered a write error. The write error could be due to a temporary failure of that Physical Volume or an indication that it is failing.  The device should be re‐
          freshed  or  replaced. (m)ismatches signifies that the RAID logical volume has portions of the array that are not coherent.  Inconsistencies are detected by initiating a "check" on a RAID logical volume.  (The scrubbing operations, "check" and "repair", can
          be performed on a RAID logical volume via the 'lvchange' command.)  (w)ritemostly signifies the devices in a RAID 1 logical volume that have been marked write-mostly.  Re(s)haping signifies a RAID Logical Volume is either undergoing  a  stripe  addition/re‐
          moval, a stripe size or RAID algorithm change.  (R)emove after reshape signifies freed striped raid images to be removed.

          Related to Thin pool Logical Volumes: (F)ailed, out of (D)ata space, (M)etadata read only.
          (F)ailed  is set if thin pool encounters serious failures and hence no further I/O is permitted at all. The out of (D)ata space is set if thin pool has run out of data space. (M)etadata read only signifies that thin pool encounters certain types of failures
          but it's still possible to do reads at least, but no metadata changes are allowed.

          Related to Thin Logical Volumes: (F)ailed.
          (F)ailed is set when related thin pool enters Failed state and no further I/O is permitted at all.
       10 s(k)ip activation: this volume is flagged to be skipped during activation.
*/
class LVInfo
{
public:
    LVInfo();
    //由于属性过多 之后补上 目前只需要激活及暂停状态。
    bool isActivve() {return m_lvStatus[4] == 'a';}
    bool isSuspended() {return m_lvStatus[4] == 's';}
public:
    QString m_vgName; //vg名称
    QString m_lvPath; //lv路径
    QString m_lvUuid; //lv uuid 唯一名称
    QString m_lvName; //lv名称 lv0 lv1 ....
    FSType  m_lvFsType; //文件系统类型
    QString m_lvSize; //字符串类型 展示用
    long long m_lvLECount; //le个数
    long long  m_fsUsed; //文件已经使用大小 单位byte
    long long  m_fsUnused;//文件未使用大小 单位byte
    int m_LESize;  //单个pe大小 与所在vg的pe大小相同 单位byte
    bool m_busy; //挂载标志
    QVector<QString> m_mountPoints;//挂载点 可多次挂载
    QString m_lvStatus{"----------"};
    long long m_minReduceSize;//文件系统最小可缩小大小 单位byte
    LVMError m_lvError;//逻辑卷错误码
};
LVMStructEnd(LVInfo)



//new by liuwh 2022/1/17
/**
 * @class VGInfo
 * @brief 逻辑卷组属性 结构体
 */
/*
vgs -o vg_attr
  Attr
  wz--n-
The vg_attr bits are:
      1  Permissions: (w)riteable, (r)ead-only
      2  Resi(z)eable
      3  E(x)ported
      4  (p)artial: one or more physical volumes belonging to the volume group are missing from the system
      5  Allocation policy: (c)ontiguous, c(l)ing, (n)ormal, (a)nywhere
      6  (c)lustered, (s)hared
*/
class VGInfo
{
public:
    VGInfo();
    bool isWriteable() {return m_vgStatus[0] == 'w';}
    bool isReadOnly() {return m_vgStatus[0] == 'r';}
    bool isResizeable() {return m_vgStatus[1] == 'z';}
    bool isExported() {return m_vgStatus[2] == 'x';}
    bool isPartial() {return m_vgStatus[3] == 'p';}
    bool isContiguous() {return m_vgStatus[4] == 'c';}
    bool isCling() {return m_vgStatus[4] == 'l';}
    bool isNormal() {return m_vgStatus[4] == 'n';}
    bool isAnywhere() {return m_vgStatus[4] == 'a';}
    bool isClustered() {return m_vgStatus[5] == 'c';}
    bool isShared() {return m_vgStatus[5] == 's';}
public:
    QString m_vgName; //vg名称 vg0, vg1 ....
    QString m_vgUuid; //vg唯一名称 uuid
    QString m_vgSize; //字符串类型 展示用  vg总大小
    QString m_vgUsed; //字符串类型 展示用  vg使用
    QString m_vgUnused; //字符串类型 展示用 vg已用
    long long m_pvCount; //物理卷个数
    long long m_peCount; //pe个数
    long long m_peUsed;  //pe使用个数
    long long m_peUnused;//pe未使用个数
    int m_PESize;  //单个pe大小  单位byte
    int m_curLV;//当前lv个数
    QString m_vgStatus{"------"}; //状态
    LVMError m_vgError;//逻辑卷组错误码
    QVector<LVInfo>m_lvlist; //vg 下lv列表
    QStringList m_pvList;//pv列表
};
LVMStructEnd(VGInfo)

//new by liuwh 2022/1/17
/**
 * @class LVMInfo
 * @brief lvm属性结构体
 */
class LVMInfo
{
public:
    LVMInfo();
public:
    QMap<QString, VGInfo> m_vgInfo;        //lvm设备信息 key:vgName value vginfo
    QMap<QString, PVInfo> m_pvInfo;        //lvm pv信息 key:/dev/sdb1 value:pvinfo
    LVMError m_lvmErr;
};
LVMStructEnd(LVMInfo)


#endif // LVMSTRUCT_H
