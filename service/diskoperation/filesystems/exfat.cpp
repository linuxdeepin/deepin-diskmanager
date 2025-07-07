// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "exfat.h"
#include <QUuid>
#include <QDebug>

namespace DiskManager {

FS ExFat::getFilesystemSupport()
{
    qDebug() << "Checking exFAT filesystem support";
    FS fs(FS_EXFAT);

    fs.busy = FS::GPARTED;
    fs.copy = FS::GPARTED;
    fs.move = FS::GPARTED;
    fs.online_read = FS::GPARTED;

    if (!Utils::findProgramInPath("dump.exfat").isEmpty()) {
        qDebug() << "dump.exfat found, enabling read support";
        fs.read = FS::EXTERNAL;
    }

    QString output, error;
    if (!Utils::findProgramInPath("mkfs.exfat").isEmpty()) {
        qDebug() << "mkfs.exfat found, checking version for create support";
        if (Utils::executCmd("mkfs.exfat -V", output, error) == 0) {
            qDebug() << "mkfs.exfat version check successful, enabling create support";
            fs.create = FS::EXTERNAL;
        }
    }

    if (!Utils::findProgramInPath("tune.exfat").isEmpty()) {
        qDebug() << "tune.exfat found, enabling label and uuid support";
        fs.read_label = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;

        // Get/set exFAT Volume Serial Number support was added to exfatprogs
        // 1.1.0.  Check the help text for the feature before enabling.
        Utils::executCmd("tune.exfat", output, error);
        if (error.indexOf("Set volume serial") < error.length()) {
            qDebug() << "tune.exfat supports setting volume serial, enabling uuid read/write";
            fs.read_uuid = FS::EXTERNAL;
            fs.write_uuid = FS::EXTERNAL;
        }
    }

    if (!Utils::findProgramInPath("fsck.exfat").isEmpty()) {
        qDebug() << "fsck.exfat found, checking version for check support";
        Utils::executCmd("fsck.exfat -V", output, error);
        if (output.contains("exfatprogs version")) {
            qDebug() << "fsck.exfat from exfatprogs found, enabling check support";
            fs.check = FS::EXTERNAL;
        }
    }
    fs.grow = FS::NONE;
    fs.shrink = FS::NONE;

    return fs;
}

void ExFat::setUsedSectors(DiskManager::Partition &partition)
{
    qDebug() << "Setting used sectors for exFAT partition:" << partition.getPath();
    QString output, error;
    auto errCode = Utils::executCmd("dump.exfat ", output, error);
    // dump.exfat returns non-zero status for both success and failure.  Instead use
    // non-empty stderr to identify failure.
    if (!error.isEmpty() || errCode != 0) {
        qDebug() << "dump.exfat failed with error:" << error << "and exit code:" << errCode;
        return;
    }

    // Example output (lines of interest only):
    //     $ dump.exfat /dev/sdb1
    //     Volume Length(sectors):                  524288
    //     Sector Size Bits:                        9
    //     Sector per Cluster bits:                 3
    //     Free Clusters: 	                        23585
    //
    // "exFAT file system specification"
    // https://docs.microsoft.com/en-us/windows/win32/fileio/exfat-specification
    // Section 3.1.5 VolumeLength field
    // Section 3.1.14 BytesPerSectorShift field
    // Section 3.1.15 SectorsPerClusterShift field

    // FS size in [FS] sectors
    long long volume_length = -1;
    int index = output.indexOf("Volume Length(sectors):");
    if (index < output.length()) {
        qDebug() << "Found Volume Length";
        sscanf(output.mid(index).toStdString().c_str(), "Volume Length(sectors): %lld", &volume_length);
    }

    // FS sector size = 2^(bytes_per_sector_shift)
    long long bytes_per_sector_shift = -1;
    index = output.indexOf("Sector Size Bits:");
    if (index < output.length()) {
        qDebug() << "Found Sector Size Bits";
        sscanf(output.mid(index).toStdString().c_str(), "Sector Size Bits: %lld", &bytes_per_sector_shift);
    }

    // Cluster size = sector_size * 2^(sectors_per_cluster_shift)
    long long sectors_per_cluster_shift = -1;
    index = output.indexOf("Sector per Cluster bits:");
    if (index < output.length()) {
        qDebug() << "Found Sector per Cluster bits";
        sscanf(output.mid(index).toStdString().c_str(), "Sector per Cluster bits: %lld",
               &sectors_per_cluster_shift);
    }

    // Free clusters
    long long free_clusters = -1;
    index = output.indexOf("Free Clusters:");
    if (index < output.length()) {
        qDebug() << "Found Free Clusters";
        sscanf(output.mid(index).toStdString().c_str(), "Free Clusters: %lld", &free_clusters);
    }

    if (volume_length > -1 && bytes_per_sector_shift > -1 &&
        sectors_per_cluster_shift > -1 && free_clusters > -1) {
        qDebug() << "All required values found, calculating and setting sector usage";
        Byte_Value sector_size = 1 << bytes_per_sector_shift;
        Byte_Value cluster_size = sector_size * (1 << sectors_per_cluster_shift);
        Sector fs_free = free_clusters * cluster_size / partition.m_sectorSize;
        Sector fs_size = volume_length * sector_size / partition.m_sectorSize;
        partition.setSectorUsage(fs_size, fs_free);
        partition.m_fsBlockSize = cluster_size;
    }
}

void ExFat::readLabel(DiskManager::Partition &partition)
{
    qDebug() << "Reading label for exFAT partition:" << partition.getPath();
    QString output, error;
    auto errCode = Utils::executCmd(QString("tune.exfat -l %1").arg(partition.getPath()), output, error);
    if (errCode != 0) {
        qDebug() << "tune.exfat -l failed with exit code:" << errCode;
        return;
    }

    partition.setFilesystemLabel(Utils::regexpLabel(output, "^label: ([^\n]*)"));
}

bool ExFat::writeLabel(const DiskManager::Partition &partition)
{
    qDebug() << "Writing label for exFAT partition:" << partition.getPath()
            << "new label:" << partition.getFileSystemLabel();
    QString cmd, output, error;
    if (!partition.getFileSystemLabel().isEmpty() && partition.getFileSystemLabel() != " ") {
        qDebug() << "Label is not empty, writing it";
        cmd = QString("tune.exfat -L %1 %2").arg(partition.getFileSystemLabel()).arg(partition.getPath());
        auto errCode = Utils::executCmd(cmd, output, error);
        return errCode == 0;
    }
    qDebug() << "Label is empty, doing nothing";
    return true;
}

void ExFat::readUuid(DiskManager::Partition &partition)
{
    qDebug() << "Reading UUID for exFAT partition:" << partition.getPath();
    QString output, error;
    auto errCode = Utils::executCmd(QString("tune.exfat -i %1").arg(partition.getPath()), output, error);
    if (errCode != 0) {
        qDebug() << "tune.exfat -i failed with exit code:" << errCode;
        return;
    }
    partition.m_uuid = serial2BlkidUuid(
            Utils::regexpLabel(output, "volume serial : (0x[[:xdigit:]][[:xdigit:]]*)"));
}

bool ExFat::writeUuid(const DiskManager::Partition &partition)
{
    qDebug() << "Writing new UUID for exFAT partition:" << partition.getPath();
    QString output, error;
    auto errCode = Utils::executCmd(QString("tune.exfat -I %1 %2")
                                            .arg(randomSerial())
                                            .arg(partition.getPath()), output, error);
    return errCode == 0;
}

bool ExFat::create(const DiskManager::Partition &new_partition)
{
    qDebug() << "Creating exFAT filesystem on:" << new_partition.getPath()
            << "with label:" << new_partition.getFileSystemLabel();
    QString cmd, output, error;
    if (new_partition.getFileSystemLabel().isEmpty() || new_partition.getFileSystemLabel() == " ") {
        qDebug() << "Label is empty, creating without label";
        cmd = QString("mkfs.exfat %1").arg(new_partition.getPath());
    } else {
        qDebug() << "Label is not empty, creating with label";
        cmd = QString("mkfs.exfat -n %1 %2").arg(new_partition.getFileSystemLabel()).arg(new_partition.getPath());
    }
    auto errCode = Utils::executCmd(cmd, output, error);
    return errCode == 0;
}

bool ExFat::resize(const DiskManager::Partition &partitionNew, bool fillPartition)
{
    qDebug() << "Entering ExFat::resize for partition:" << partitionNew.getPath();
    return FileSystem::resize(partitionNew, fillPartition);
}

bool DiskManager::ExFat::resize(const QString &path, const QString &size, bool fillPartition)
{
    qDebug() << "Entering ExFat::resize for path:" << path;
    return FileSystem::resize(path, size, fillPartition);
}

bool DiskManager::ExFat::checkRepair(const DiskManager::Partition &partition)
{
    qDebug() << "Entering ExFat::checkRepair for partition:" << partition.getPath();
    QString path = partition.getPath();
    return checkRepair(path);
}

bool DiskManager::ExFat::checkRepair(const QString &devpath)
{
    qDebug() << "Checking/repairing exFAT filesystem on:" << devpath;
    QString output, error;
    auto errCode = Utils::executCmd(QString("fsck.exfat %1").arg(devpath), output, error);
    return errCode == 0;
}

FS_Limits DiskManager::ExFat::getFilesystemLimits(const DiskManager::Partition &partition)
{
    qDebug() << "Entering ExFat::getFilesystemLimits";
    m_fsLimits.min_size = -1;
    m_fsLimits.max_size = -1;
    return m_fsLimits;
}

QString DiskManager::ExFat::serial2BlkidUuid(QString serial)
{
    qDebug() << "Entering ExFat::serial2BlkidUuid for serial:" << serial;
    QString verifiedSerial = Utils::regexpLabel(serial, "^(0x[[:xdigit:]][[:xdigit:]]*)$");
    if (verifiedSerial.isEmpty()) {
        qDebug() << "Serial is not valid";
        return verifiedSerial;
    }

    QString canonicalUuid = verifiedSerial.mid(2, 4).toUpper() + "-" +
                            verifiedSerial.mid(6, 4).toUpper();
    return canonicalUuid;
}

QString DiskManager::ExFat::randomSerial()
{
    qDebug() << "Generating random serial number for exFAT";
    return "0x" + QUuid::createUuid().toString().mid(0, 8);
}

}