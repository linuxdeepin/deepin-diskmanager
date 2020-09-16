/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file device.cpp
 *
 * @brief 设备信息类
 *
 * @date 2020-09-09 13:39
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

#include "device.h"

#include <QDebug>

namespace DiskManager {

Device::Device()
{
    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
    qDBusRegisterMetaType<PartitionVec>();
    qDBusRegisterMetaType<stCustest>();
    qDBusRegisterMetaType<HardDiskInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfo>();
    qDBusRegisterMetaType<HardDiskStatusInfoList>();
}

void Device::enablePartitionNaming(int length)
{
    if (length > 0)
        m_maxPartitionNameLength = length;
    else
        m_maxPartitionNameLength = 0;
}

bool Device::partitionNamingSupported() const
{
    return m_maxPartitionNameLength > 0;
}

//int Device::getMaxPartitionNameLength() const
//{
//    return m_maxPartitionNameLength;
//}

DeviceInfo Device::getDeviceInfo()
{
    DeviceInfo info;
    info.length = m_length;
    info.heads = m_heads;
    info.m_path = m_path;
    info.sectors = m_sectors;
    info.cylinders = m_cylinders;
    info.cylsize = m_cylsize;
    info.model = m_model;
    info.serial_number = m_serialNumber;
    info.disktype = m_diskType;
    info.sector_size = m_sectorSize;
    info.max_prims = m_maxPrims;
    info.highest_busy = m_highestBusy;
    info.max_partition_name_length = m_maxPartitionNameLength;
//        qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
//                 << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
//                 << info.sector_size << info.max_prims << info.highest_busy << info.readonly
//                 << info.max_partition_name_length;
    return info;
}

} // namespace DiskManager
