// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "partition.h"
#include "utils.h"
#include "supportedfilesystems.h"

#include <QDebug>

namespace DiskManager {

Partition::Partition()
{
    qCDebug(serviceLog) << "Creating new partition";
    reset();
}

Partition *Partition::clone() const
{
    qCDebug(serviceLog) << "Cloning partition";
    return new Partition(*this);
}

bool Partition::sectorUsageKnown() const
{
    qCDebug(serviceLog) << "Checking if sector usage is known";
    return m_sectorsUsed >= 0 && m_sectorsUnused >= 0;
}

Sector Partition::getSectorsUnallocated() const
{
    qCDebug(serviceLog) << "Getting unallocated sectors";
    if (m_sectorsUnallocated < m_significantThreshold) {
        qCDebug(serviceLog) << "Unallocated sectors less than threshold, returning 0";
        return 0;
    } else {
        qCDebug(serviceLog) << "Returning unallocated sectors:" << m_sectorsUnallocated;
        return m_sectorsUnallocated;
    }
}

void Partition::reset()
{
    qCDebug(serviceLog) << "Resetting partition data";
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
    m_fsLimits = FS_Limits();
    m_luksFlag = LUKSFlag::NOT_CRYPT_LUKS;
    m_cryptCipher = CRYPT_CIPHER::NOT_CRYPT;
    m_tokenList.clear();
    m_decryptStr = "";
    m_dmName = "";
}

void Partition::reset(const PartitionInfo &info)
{
    qCDebug(serviceLog) << "Resetting partition with PartitionInfo";
    m_status = static_cast<PartitionStatus>(info.m_status);
    m_type = static_cast<PartitionType>(info.m_type);
    m_alignment = static_cast<PartitionAlignment>(info.m_alignment);
    m_fstype = static_cast<FSType>(info.m_fileSystemType);
    m_sectorStart = info.m_sectorStart;
    m_sectorEnd = info.m_sectorEnd;
    m_sectorsUsed = info.m_sectorsUsed;
    m_sectorsUnused = info.m_sectorsUnused;
    m_sectorsUnallocated = info.m_sectorsUnallocated;
    m_significantThreshold = info.m_significantThreshold;
    m_freeSpaceBefore = info.m_freeSpaceBefore;
    m_sectorSize = info.m_sectorSize;
    m_fsBlockSize = info.m_fileSystemBlockSize;
    m_devicePath = info.m_devicePath;
    setFilesystemLabel(info.m_fileSystemLabel);
    m_fsLimits = info.m_fsLimits;

    m_luksFlag = info.m_luksFlag;
    m_cryptCipher = info.m_crypt;
    m_tokenList = info.m_tokenList;
    m_decryptStr = info.m_decryptStr;
    m_dmName = info.m_dmName;

}

void Partition::set(const QString &devicePath, const QString &partition, int partitionNumber, PartitionType type, FSType fstype, Sector sectorStart, Sector sectorEnd, Byte_Value sectorSize, bool insideExtended, bool busy)
{
    qCDebug(serviceLog) << "set partition entry";
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

    qCDebug(serviceLog) << "Setting partition:" << partition
                    << "type:" << type
                    << "fs:" << fstype
                    << "sectors:" << sectorStart << "-" << sectorEnd;

    SupportedFileSystems s;
    FileSystem *fs =  s.getFsObject(m_fstype);
    if (fs) {
        qCDebug(serviceLog) << "FileSystem object found, getting limits";
        m_fsLimits = fs->getFilesystemLimits(*this);
    } else {
        qCDebug(serviceLog) << "FileSystem object not found, setting default limits";
        m_fsLimits = FS_Limits{-1, -1};
    }
}

void Partition::setUnpartitioned(const QString &devicePath, const QString &partitionPath, FSType fstype, Sector length, Byte_Value sectorSize, bool busy)
{
    qCDebug(serviceLog) << "setUnpartitioned entry";
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
        (fstype == FS_UNALLOCATED) ? Utils::fileSystemTypeToString(FS_UNALLOCATED) : partitionPath,
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
    qCDebug(serviceLog) << "Setting unallocated partition at sectors:"
                    << sectorStart << "-" << sectorEnd;
    reset();

    set(devicePath,
        Utils::fileSystemTypeToString(FS_UNALLOCATED),
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
    qCDebug(serviceLog) << "Checking if filesystem label is known";
    return m_haveFileSystemLabel;
}

void Partition::setFilesystemLabel(const QString &filesystemLabel)
{
    qCDebug(serviceLog) << "Setting filesystem label:" << filesystemLabel;
    if (!filesystemLabel.isEmpty()) {
        qCDebug(serviceLog) << "Filesystem label is not empty, setting it";
        m_filesystemLabel = filesystemLabel;
        m_haveFileSystemLabel = true;
    }
}

//void Partition::addMountPoint(const QString &mountpoint)
//{
//    m_mountpoints.push_back(mountpoint);
//}

void Partition::addMountPoints(const QVector<QString> &mountpoints)
{
    qCDebug(serviceLog) << "Adding mount points";
    m_mountpoints = mountpoints;
}

QVector<QString> Partition::getMountPoints() const
{
    qCDebug(serviceLog) << "Getting mount points";
    return m_mountpoints;
}

QString Partition::getMountPoint() const
{
    qCDebug(serviceLog) << "Getting single mount point";
    if (m_mountpoints.size() > 0) {
        qCDebug(serviceLog) << "Mount points exist, returning the first one";
        return m_mountpoints.front();
    }

    qCDebug(serviceLog) << "No mount points, returning empty string";
    return "";
}

void Partition::setSectorUsage(Sector sectorsFsSize, Sector sectorsFsUnused)
{
    qCDebug(serviceLog) << "setSectorUsage entry";
    Sector length = getSectorLength();
    qCDebug(serviceLog) << "Setting sector usage, partition length:" << length;
    if (0 <= sectorsFsSize && sectorsFsSize <= length
            && 0 <= sectorsFsUnused && sectorsFsUnused <= sectorsFsSize) {
        qCDebug(serviceLog) << "Valid sector usage data provided";
        m_sectorsUsed = sectorsFsSize - sectorsFsUnused;
        m_sectorsUnused = sectorsFsUnused;
        m_sectorsUnallocated = length - sectorsFsSize;
        m_significantThreshold = calcSignificantUnallocatedSectors();
    } else if (sectorsFsSize == -1) {
        qCDebug(serviceLog) << "sectorsFsSize is -1, calculating usage differently";
        if (0 <= sectorsFsUnused && sectorsFsUnused <= length) {
            qCDebug(serviceLog) << "Valid sectorsFsUnused";
            m_sectorsUsed = length - sectorsFsUnused;
            m_sectorsUnused = sectorsFsUnused;
        } else {
            qCDebug(serviceLog) << "Invalid sectorsFsUnused";
            m_sectorsUsed = -1;
            m_sectorsUnused = -1;
        }
        m_sectorsUnallocated = 0;
        m_significantThreshold = 1;
    }
}

Byte_Value Partition::getByteLength() const
{
    qCDebug(serviceLog) << "Getting byte length";
    if (getSectorLength() >= 0) {
        qCDebug(serviceLog) << "Sector length is valid, calculating byte length";
        return getSectorLength() * m_sectorSize;
    } else {
        qCDebug(serviceLog) << "Sector length is invalid, returning -1";
        return -1;
    }
}

Sector Partition::getSectorLength() const
{
    qCDebug(serviceLog) << "Getting sector length";
    if (m_sectorStart >= 0 && m_sectorEnd >= 0 && m_sectorEnd < LLONG_MAX) {
        qCDebug(serviceLog) << "Start and end sectors are valid";
        return m_sectorEnd - m_sectorStart + 1;
    } else {
        qCDebug(serviceLog) << "Start or end sectors are invalid, returning -1";
        return -1;
    }
}

QString Partition::getFileSystemLabel() const
{
    qCDebug(serviceLog) << "Getting filesystem label";
    if (m_haveFileSystemLabel) {
        qCDebug(serviceLog) << "Filesystem label is known, returning it";
        return m_filesystemLabel;
    }
    qCDebug(serviceLog) << "Filesystem label is not known, returning empty string";
    return "";
}

PartitionInfo Partition::getPartitionInfo() const
{
    qCDebug(serviceLog) << "Getting partition info";
    PartitionInfo info;
    info.m_devicePath = m_devicePath;
    info.m_partitionNumber = m_partitionNumber;
    info.m_type = m_type;
    info.m_status = m_status;
    info.m_alignment = m_alignment;
    info.m_fileSystemType = m_fstype;
    info.m_uuid = m_uuid;
    info.m_name = m_name;
    info.m_sectorStart = m_sectorStart;
    info.m_sectorEnd = m_sectorEnd;
    info.m_sectorsUsed = m_sectorsUsed;
    info.m_sectorsUnused = m_sectorsUnused;
    info.m_sectorsUnallocated = m_sectorsUnallocated;
    info.m_significantThreshold = m_significantThreshold;
    info.m_freeSpaceBefore = m_freeSpaceBefore;
    info.m_sectorSize = m_sectorSize;
    info.m_fileSystemBlockSize = m_fsBlockSize;
    info.m_path = m_path;
    info.m_fileSystemLabel = m_filesystemLabel;
    info.m_insideExtended = m_insideExtended;
    info.m_busy = m_busy;
    info.m_fileSystemReadOnly = m_fsReadonly;
    info.m_mountPoints = m_mountpoints;
    info.m_fsLimits = m_fsLimits;
    qCDebug(serviceLog) << "Getting partition info:"
                    << "path:" << info.m_path
                    << "type:" << info.m_type
                    << "fs:" << info.m_fileSystemType
                    << "sectors:" << info.m_sectorStart << "-" << info.m_sectorEnd;
    return info;
}

Sector Partition::calcSignificantUnallocatedSectors() const
{
    qCDebug(serviceLog) << "Calculating significant unallocated sectors";
    const double HIGHERUNALLOCATEDFRACTION = 0.05;
    const double LOWERUNALLOCATEDFRACTION = 0.02;
    Sector length = getSectorLength();
    Byte_Value byteLen = length * m_sectorSize;
    Sector significant;

    if (byteLen <= 0) {
        qCDebug(serviceLog) << "byteLen <= 0, setting significant to 1";
        significant = 1;
    } else if (byteLen <= 100 * MEBIBYTE) {
        qCDebug(serviceLog) << "byteLen <= 100MiB, using higher fraction";
        significant = qRound(length * HIGHERUNALLOCATEDFRACTION);
    } else if (byteLen <= 1 * GIBIBYTE) {
        qCDebug(serviceLog) << "byteLen <= 1GiB, calculating fraction";
        double fraction = (HIGHERUNALLOCATEDFRACTION - LOWERUNALLOCATEDFRACTION) - (byteLen - 100 * MEBIBYTE) * (HIGHERUNALLOCATEDFRACTION - LOWERUNALLOCATEDFRACTION) / (1 * GIBIBYTE - 100 * MEBIBYTE) + LOWERUNALLOCATEDFRACTION;
        significant = qRound(length * fraction);
    } else {
        qCDebug(serviceLog) << "byteLen > 1GiB, using lower fraction";
        significant = qRound(length * LOWERUNALLOCATEDFRACTION);
    }

    qCDebug(serviceLog) << "Calculated significant sectors (before check):" << significant;

    if (significant <= 1) {
        qCDebug(serviceLog) << "Significant sectors <= 1, setting to 1";
        significant = 1;
    }

    qCDebug(serviceLog) << "Final significant sectors:" << significant;
    return significant;
}

} //namespace DiskManager
