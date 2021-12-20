/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file deviceinfo.h
 *
 * @brief 设备信息结构
 *
 * @date 2021-04-06 15:36
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
#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "utils.h"
#include "partitioninfo.h"

#include <QVector>

//dubs使用自定义数据结构中的成员变量建议初始化，在测试中发现数据结构中包含的bool类型变量未赋值，该数据结构作为槽函数返回值通过dbus调用可能导致应用崩溃退出
class QDBusArgument;

/**
 * @class CUSSTRUCTTEST
 * @brief qdbus测试结构体
 */
typedef struct CUSSTRUCTTEST {
    Sector m_length;
    Sector m_heads;
    QString m_path;
} stCustest;
Q_DECLARE_METATYPE(stCustest)
QDBusArgument &operator<<(QDBusArgument &argument, const stCustest &stcus);
const QDBusArgument &operator>>(const QDBusArgument &argument, stCustest &stcus);

//new by li 2020/7/20

/**
 * @class HARDDISKINFO
 * @brief 设备硬件信息结构体
 */
typedef struct HARDDISKINFO {
    QString m_model;              //<! 【型号】1
    QString m_vendor;             //<! 【制造商】2 //有可能会没有
    QString m_mediaType;          //<! 【介质类型】3
    QString m_size;               //<! 【大小】4
    QString m_rotationRate;       //<! 【转速】
    QString m_interface;          //<! 【接口】6
    QString m_serialNumber;       //<! 【序列号】7
    QString m_version;            //<! 【版本】
    QString m_capabilities;       //<! 【功能】
    QString m_description;        //<! 【描述】
    QString m_powerOnHours;       //<! 【通电时间】9
    QString m_powerCycleCount;    //<! 【通电次数】10
    QString m_firmwareVersion;    //<! 【固件版本】8
    QString m_speed;              //<! 【速度】5
} HardDiskInfo;
Q_DECLARE_METATYPE(HardDiskInfo)
QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskInfo &inhdinfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskInfo &inhdinfo);


/**
 * @class HARDDISKSTATUSINFO
 * @brief 设备健康信息结构体
 */
typedef struct HARDDISKSTATUSINFO {
    QString m_id;                   //ID
    QString m_attributeName;        //硬盘制造商定义的属性名
    QString m_flag;                 //属性操作标志
    QString m_value;                //当前值
    QString m_worst;                //最差值
    QString m_thresh;               //临界值
    QString m_type;                 //属性的类型（Pre-fail或Oldage）
    QString m_updated;              //表示属性的更新频率
    QString m_whenFailed;           //如果VALUE小于等于THRESH，会被设置成“FAILING_NOW”；如果WORST小于等于THRESH会被设置成“In_the_past”；如果都不是，会被设置成“-”。在“FAILING_NOW”情况下，需要尽快备份重要 文件，特别是属性是Pre-fail类型时。“In_the_past”代表属性已经故障了，但在运行测试的时候没问题。“-”代表这个属性从没故障过。
    QString m_rawValue;             //原始值
    //int m_flag;                   //标记位
} HardDiskStatusInfo;
Q_DECLARE_METATYPE(HardDiskStatusInfo)
QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskStatusInfo &inhdinfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskStatusInfo &inhdinfo);


/**
 * @class DeviceInfo
 * @brief 设备信息类
 */
class DeviceInfo
{
public:
    DeviceInfo();

public:
    Sector m_length;
    Sector m_heads;
    QString m_path;
    Sector m_sectors;
    Sector m_cylinders;
    Sector m_cylsize;
    QString m_model;
    QString m_serialNumber;
    QString m_disktype;
    QString m_mediaType;        //介质类型
    QString m_interface;                //接口
    int m_sectorSize;
    int m_maxPrims;
    int m_highestBusy;
    bool m_readonly;
    int m_maxPartitionNameLength;
    PartitionVec m_partition;
};
Q_DECLARE_METATYPE(DeviceInfo)

QDBusArgument &operator<<(QDBusArgument &argument, const DeviceInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, DeviceInfo &info);

typedef QMap<QString, DeviceInfo> DeviceInfoMap;
Q_DECLARE_METATYPE(DeviceInfoMap)

typedef QList<HardDiskStatusInfo> HardDiskStatusInfoList;
Q_DECLARE_METATYPE(HardDiskStatusInfoList)

#endif // DEVICEINFO_H
