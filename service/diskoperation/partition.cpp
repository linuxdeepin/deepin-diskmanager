/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partition.cpp
 *
 * @brief 分区信息类
 *
 * @date 2020-09-04 17:06
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

#include "partition.h"
#include "utils.h"
#include <QDebug>

namespace DiskManager {

Partition::Partition()
{
    reset();
}

Partition *Partition::clone() const
{
    return new Partition(*this);
}

bool Partition::sectorUsageKnown() const
{
    return m_sectorsUsed >= 0 && m_sectorsUnused >= 0;
}

Sector Partition::getSectorsUnallocated() const
{
    if (m_sectorsUnallocated < m_significantThreshold)
        return 0;
    else
        return m_sectorsUnallocated;
}

void Partition::reset()
{
    m_path.clear();
    m_status = STAT_REAL;
    m_type = TYPE_UNALLOCATED;
    m_alignment = ALIGN_STRICT;
    m_fstype = FS_UNALLOCATED;
    m_haveFileSystemLabel = false;
    m_uuid.clear();
    m_name.clear();
    m_partitionNumber = m_sectorStart = m_sectorEnd = m_sectorsUsed = m_sectorsUnused = -1;
    m_sectorsUnallocated = 0;
    m_significantThreshold = 1;
    m_freeSpaceBefore = -1;
    m_sectorSize = 0;
    m_fsBlockSize = -1;
    m_insideExtended = m_busy = m_strictStart = false;
    m_fsReadonly = false;
    m_logicals.clear();
    m_flags.clear();
    m_mountpoints.clear();
}

void Partition::reset(const PartitionInfo &info)
{
    m_status = static_cast<PartitionStatus>(info.status);
    m_type = static_cast<PartitionType>(info.type);
    m_alignment = static_cast<PartitionAlignment>(info.alignment);
    m_fstype = static_cast<FSType>(info.fstype);
    m_sectorStart = info.sector_start;
    m_sectorEnd = info.sector_end;
    m_sectorsUsed = info.sectors_used;
    m_sectorsUnused = info.sectors_unused;
    m_sectorsUnallocated = info.sectors_unallocated;
    m_significantThreshold = info.significant_threshold;
    m_freeSpaceBefore = info.free_space_before;
    m_sectorSize = info.sector_size;
    m_fsBlockSize = info.fs_block_size;
    m_devicePath = info.device_path;
    setFilesystemLabel(info.filesystem_label);
}

void Partition::set(const QString &devicePath, const QString &partition, int partitionNumber, PartitionType type, FSType fstype, Sector sectorStart, Sector sectorEnd, Byte_Value sectorSize, bool insideExtended, bool busy)
{
    m_devicePath = devicePath;
    m_path = partition;
    m_partitionNumber = partitionNumber;
    m_type = type;
    m_fstype = fstype;
    m_sectorStart = sectorStart;
    m_sectorEnd = sectorEnd;
    m_sectorSize = sectorSize;
    m_insideExtended = insideExtended;
    m_busy = busy;
}

void Partition::setUnpartitioned(const QString &devicePath, const QString &partitionPath, FSType fstype, Sector length, Byte_Value sectorSize, bool busy)
{
    reset();
    // The path from the parent Device object and this child Partition object
    // spanning the whole device would appear to be the same.  However the former
    // may have come from the command line and the later is the canonicalised
    // name returned from libparted.  (See GParted_Core::set_device_from_disk()
    // "loop" table and GParted_Core::set_device_one_partition() calls to
    // setUnpartitioned()).  Therefore they can be different.
    //
    // For unrecognised whole disk device partitions use "unallocated" as the
    // partition path to match what setUnallocated() does when it created such
    // whole disk device partitions.
    set(devicePath,
        (fstype == FS_UNALLOCATED) ? Utils::FSTypeToString(FS_UNALLOCATED) : partitionPath,
        1,
        TYPE_UNPARTITIONED,
        fstype,
        0LL,
        length - 1LL,
        sectorSize,
        false,
        busy);
}

void Partition::setUnallocated(const QString &devicePath, Sector sectorStart, Sector sectorEnd, Byte_Value sectorSize, bool insideExtended)
{
    reset();

    set(devicePath,
        Utils::FSTypeToString(FS_UNALLOCATED),
        -1,
        TYPE_UNALLOCATED,
        FS_UNALLOCATED,
        sectorStart,
        sectorEnd,
        sectorSize,
        insideExtended,
        false);

    m_status = STAT_REAL;
}

bool Partition::filesystemLabelKnown() const
{
    return m_haveFileSystemLabel;
}

void Partition::setFilesystemLabel(const QString &filesystemLabel)
{
    if (!filesystemLabel.isEmpty()) {
        m_filesystemLabel = filesystemLabel;
        m_haveFileSystemLabel = true;
    }
}

void Partition::addMountPoint(const QString &mountpoint)
{
    m_mountpoints.push_back(mountpoint);
}

void Partition::addMountPoints(const QVector<QString> &mountpoints)
{
    m_mountpoints = mountpoints;
}

QVector<QString> Partition::getMountPoints() const
{
    return m_mountpoints;
}

QString Partition::getMountPoint() const
{
    if (m_mountpoints.size() > 0)
        return m_mountpoints.front();

    return "";
}

void Partition::setSectorUsage(Sector sectorsFsSize, Sector sectorsFsUnused)
{
    Sector length = getSectorLength();
    if (0 <= sectorsFsSize && sectorsFsSize <= length
            && 0 <= sectorsFsUnused && sectorsFsUnused <= sectorsFsSize) {
        m_sectorsUsed = sectorsFsSize - sectorsFsUnused;
        m_sectorsUnused = sectorsFsUnused;
        m_sectorsUnallocated = length - sectorsFsSize;
        m_significantThreshold = calcSignificantUnallocatedSectors();
    } else if (sectorsFsSize == -1) {
        if (0 <= sectorsFsUnused && sectorsFsUnused <= length) {
            m_sectorsUsed = length - sectorsFsUnused;
            m_sectorsUnused = sectorsFsUnused;
        } else {
            m_sectorsUsed = -1;
            m_sectorsUnused = -1;
        }
        m_sectorsUnallocated = 0;
        m_significantThreshold = 1;
    }
}

Byte_Value Partition::getByteLength() const
{
    if (getSectorLength() >= 0)
        return getSectorLength() * m_sectorSize;
    else
        return -1;
}

Sector Partition::getSectorLength() const
{
    if (m_sectorStart >= 0 && m_sectorEnd >= 0)
        return m_sectorEnd - m_sectorStart + 1;
    else
        return -1;
}

QString Partition::getFileSystemLabel() const
{
    if (m_haveFileSystemLabel)
        return m_filesystemLabel;
    return "";
}

PartitionInfo Partition::getPartitionInfo()
{
    PartitionInfo info;
    info.device_path = m_devicePath;
    info.partition_number = m_partitionNumber;
    info.type = m_type;
    info.status = m_status;
    info.alignment = m_alignment;
    info.fstype = m_fstype;
    info.uuid = m_uuid;
    info.name = m_name;
    info.sector_start = m_sectorStart;
    info.sector_end = m_sectorEnd;
    info.sectors_used = m_sectorsUsed;
    info.sectors_unused = m_sectorsUnused;
    info.sectors_unallocated = m_sectorsUnallocated;
    info.significant_threshold = m_significantThreshold;
    info.free_space_before = m_freeSpaceBefore;
    info.sector_size = m_sectorSize;
    info.fs_block_size = m_fsBlockSize;
    info.path = m_path;
    info.filesystem_label = m_filesystemLabel;
    info.inside_extended = m_insideExtended;
    info.busy = m_busy;
    info.fs_readonly = m_fsReadonly;
    info.mountpoints = m_mountpoints;
//        qDebug() << info.device_path << info.partition_number << info.type << info.status << info.alignment << info.fstype << info.uuid
//                 << info.name << info.sector_start << info.sector_end << info.sectors_used << info.sectors_unused
//                 << info.sectors_unallocated << info.significant_threshold << info.free_space_before
//                 << info.sector_size << info.fs_block_size << info.path << info.filesystem_label;
    return info;
}

Sector Partition::calcSignificantUnallocatedSectors() const
{
    const double HIGHERUNALLOCATEDFRACTION = 0.05;
    const double LOWERUNALLOCATEDFRACTION = 0.02;
    Sector length = getSectorLength();
    Byte_Value byteLen = length * m_sectorSize;
    Sector significant;

    if (byteLen <= 0) {
        significant = 1;
    } else if (byteLen <= 100 * MEBIBYTE) {
        significant = qRound(length * HIGHERUNALLOCATEDFRACTION);
    } else if (byteLen <= 1 * GIBIBYTE) {
        double fraction = (HIGHERUNALLOCATEDFRACTION - LOWERUNALLOCATEDFRACTION) - (byteLen - 100 * MEBIBYTE) * (HIGHERUNALLOCATEDFRACTION - LOWERUNALLOCATEDFRACTION) / (1 * GIBIBYTE - 100 * MEBIBYTE) + LOWERUNALLOCATEDFRACTION;
        significant = qRound(length * fraction);
    } else {
        significant = qRound(length * LOWERUNALLOCATEDFRACTION);
    }
    if (significant <= 1)
        significant = 1;
    return significant;
}

} //namespace DiskManager
