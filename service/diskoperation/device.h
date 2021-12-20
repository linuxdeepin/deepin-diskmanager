/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file device.h
 *
 * @brief 设备信息类
 *
 * @date 2020-09-09 11:37
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

#ifndef DEVICE_H
#define DEVICE_H
#include "deviceinfo.h"
#include "partition.h"

#include <QtDBus/QtDBus>
#include <QVector>

namespace DiskManager {


/**
 * @class Device
 * @brief 设备信息类
 */

class Device
{
public:
    Device();

    /**
     * @brief 启用分区命名
     * @param length:长度
     */
    void enablePartitionNaming(int length); // > 0 => enable partition naming support

    /**
     * @brief 支持分区命名
     * @return true成功false失败
     */
    bool partitionNamingSupported() const;

//    /**
//     * @brief 获得最大分区命名长度
//     * @return 最大分区命名长度
//     */
//    int getMaxPartitionNameLength() const;

    /**
     * @brief 获得设备信息
     * @return 设备信息
     */
    DeviceInfo getDeviceInfo();

public:
    Sector m_length;        //长度
    Sector m_heads;           //扇区头
    QString m_path;         //路径
    Sector m_sectors;         //扇区
    Sector m_cylinders;       //柱面
    Sector m_cylsize;         //柱面大小
    QString m_model;          //模型
    QString m_serialNumber;  //序列号
    QString m_diskType;       //磁盘类型
    QString m_mediaType;        //介质类型
    QString m_interface;                //接口
    int m_sectorSize;        //扇区大小
    int m_maxPrims;          //最大分区个数
    int m_highestBusy;       //挂载
    bool m_readonly;          //是否只读
    QVector<Partition *> m_partitions; //分区信息

private:
    int m_maxPartitionNameLength; //最大分区命名长度
};

} //namespace DiskManager
#endif // DEVICE_H
