/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitioninfo.cpp
 *
 * @brief 分区信息类
 *
 * @date 2020-09-21 14:49
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

#include "partitioninfo.h"

#include <QDBusArgument>

PartitionInfo::PartitionInfo()
    : inside_extended(false)
    , busy(false)
    , fs_readonly(false)
{
    //inside_extended = busy = fs_readonly = false;
    m_devicePath = uuid = name = path = filesystem_label = "";
    m_partitionNumber = type = status = alignment = fstype = 0;
    sector_start = sector_end = sectors_used = sectors_unused = sectors_unallocated = significant_threshold = free_space_before = 0;
    flag = 0;
    // mountpoints.clear();
}

Sector PartitionInfo::getSectorLength() const
{
    if (sector_start >= 0 && sector_end >= 0)
        return sector_end - sector_start + 1;
    else
        return -1;
}

Byte_Value PartitionInfo::getByteLength() const
{
    if (getSectorLength() >= 0)
        return getSectorLength() * sector_size;
    else
        return -1;
}

bool PartitionInfo::operator==(const PartitionInfo &info) const
{
    return m_devicePath == info.m_devicePath && m_partitionNumber == info.m_partitionNumber && sector_start == info.sector_start && type == info.type;
}
QDBusArgument &operator<<(QDBusArgument &argument, const PartitionInfo &info)
{
    argument.beginStructure();
    argument << info.m_devicePath
             << info.m_partitionNumber
             << info.type
             << info.status
             << info.alignment
             << info.fstype
             << info.uuid
             << info.name
             << info.sector_start
             << info.sector_end
             << info.sectors_used
             << info.sectors_unused
             << info.sectors_unallocated
             << info.significant_threshold
             << info.free_space_before
             << info.sector_size
             << info.fs_block_size
             << info.path
             << info.filesystem_label
             << info.inside_extended
             << info.busy
             << info.fs_readonly
             << info.flag
             << info.mountpoints;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, PartitionInfo &info)
{
    argument.beginStructure();

    argument >> info.m_devicePath
        >> info.m_partitionNumber
        >> info.type
        >> info.status
        >> info.alignment
        >> info.fstype
        >> info.uuid
        >> info.name
        >> info.sector_start
        >> info.sector_end
        >> info.sectors_used
        >> info.sectors_unused
        >> info.sectors_unallocated
        >> info.significant_threshold
        >> info.free_space_before
        >> info.sector_size
        >> info.fs_block_size
        >> info.path
        >> info.filesystem_label
        >> info.inside_extended
        >> info.busy
        >> info.fs_readonly
        >> info.flag
        >> info.mountpoints;
    argument.endStructure();

    return argument;
}
