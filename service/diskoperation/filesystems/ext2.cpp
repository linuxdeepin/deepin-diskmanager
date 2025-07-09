// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ext2.h"
#include "utils.h"

#include <QDebug>

namespace DiskManager {

EXT2::EXT2(FSType type)
    : m_forceAuto64bit(false)
    , m_specificType(type)
{
}

FS EXT2::getFilesystemSupport()
{
    FS fs(m_specificType);

    fs.busy = FS::GPARTED;
    QString output, error;
    m_mkfsCmd = "mkfs." + Utils::fileSystemTypeToString(m_specificType);
    bool have_64bit_feature = false;
    if (!Utils::findProgramInPath(m_mkfsCmd).isEmpty()) {
        fs.create = FS::EXTERNAL;
        fs.create_with_label = FS::EXTERNAL;

        // Determine mkfs.ext4 version specific capabilities.
        m_forceAuto64bit = false;
        if (m_specificType == FS_EXT4) {
            Utils::executCmd(QString("%1%2").arg(m_mkfsCmd).arg(" -V"), output, error);
            int mke2fs_major_ver = 0;
            int mke2fs_minor_ver = 0;
            int mke2fs_patch_ver = 0;
            if (sscanf(output.toLatin1().data(), "mke2fs %d.%d.%d",
                       &mke2fs_major_ver, &mke2fs_minor_ver, &mke2fs_patch_ver)
                    >= 2) {
                // Ext4 64bit feature was added in e2fsprogs 1.42, but
                // only enable large volumes from 1.42.9 when a large
                // number of 64bit bugs were fixed.
                // *   Release notes, E2fsprogs 1.42 (November 29, 2011)
                //     http://e2fsprogs.sourceforge.net/e2fsprogs-release.html#1.42
                // *   Release notes, E2fsprogs 1.42.9 (December 28, 2013)
                //     http://e2fsprogs.sourceforge.net/e2fsprogs-release.html#1.42.9
                have_64bit_feature = (mke2fs_major_ver > 1)
                                     || (mke2fs_major_ver == 1 && mke2fs_minor_ver > 42)
                                     || (mke2fs_major_ver == 1 && mke2fs_minor_ver == 42 && mke2fs_patch_ver >= 9);

                // (#766910) E2fsprogs 1.43 creates 64bit ext4 file
                // systems by default.  RHEL/CentOS 7 configured e2fsprogs
                // 1.42.9 to create 64bit ext4 file systems by default.
                // Theoretically this can be done when 64bit feature was
                // added in e2fsprogs 1.42.  GParted will re-implement the
                // removed mke2fs.conf(5) auto_64-bit_support option to
                // avoid the issues with multiple boot loaders not working
                // with 64bit ext4 file systems.
                m_forceAuto64bit = (mke2fs_major_ver > 1)
                                   || (mke2fs_major_ver == 1 && mke2fs_minor_ver >= 42);
            }
        }
    }

    if (!Utils::findProgramInPath("dumpe2fs").isEmpty()) {
        fs.read = FS::EXTERNAL;
        fs.online_read = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("tune2fs").isEmpty()) {
        fs.read_uuid = FS::EXTERNAL;
        fs.write_uuid = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("e2label").isEmpty()) {
        fs.read_label = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("e2fsck").isEmpty())
        fs.check = FS::EXTERNAL;

    if (!Utils::findProgramInPath("resize2fs").isEmpty()) {
        fs.grow = FS::EXTERNAL;

        if (fs.read) // Needed to determine a min file system size..
            fs.shrink = FS::EXTERNAL;
    }

    if (fs.check) {
        fs.copy = fs.move = FS::GPARTED;

        // If supported, use e2image to copy/move the file system as it only
        // copies used blocks, skipping unused blocks.  This is more efficient
        // than copying all blocks used by GParted's internal method.
        if (!Utils::findProgramInPath("e2image").isEmpty()) {
            Utils::executCmd("e2image", output, error);
            if (Utils::regexpLabel(output, "(-o src_offset)") == "-o src_offset")
                fs.copy = fs.move = FS::EXTERNAL;
        }
    }
    // Maximum size of an ext2/3/4 volume is 2^32 - 1 blocks, except for ext4 with
    // 64bit feature.  That is just under 16 TiB with a 4K block size.
    // *   Ext4 Disk Layout, Blocks
    //     https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout#Blocks
    // FIXME: Rounding down to whole MiB here should not be necessary.  The Copy, New
    // and Resize/Move dialogs should limit FS correctly without this.  See bug
    // #766910 comment #12 onwards for further discussion.
    //     https://bugzilla.gnome.org/show_bug.cgi?id=766910#c12
    if (m_specificType == FS_EXT2 || m_specificType == FS_EXT3 || (m_specificType == FS_EXT4 && !have_64bit_feature))
        m_fsLimits.max_size = Utils::floorSize(16 * TEBIBYTE - 4 * KIBIBYTE, MEBIBYTE);

    return fs;
}

void EXT2::setUsedSectors(Partition &partition)
{
    QString output, error, strcmd;
    strcmd = QString("dumpe2fs -h %1").arg(partition.getPath());
    
    if (!Utils::executCmd(strcmd, output, error)) {
        long long blockCount = -1;
        long long blockSize = -1;
        long long freeBlocks = -1;
        long long reservedBlocks = 0;
        
        // 获取总块数
        QString strmatch = "Block count:";
        int index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            blockCount = Utils::regexpLabel(output, QString("(?<=Block count:).*(?=\n)")).trimmed().toLong();
        }
        
        // 获取块大小
        strmatch = "Block size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            blockSize = Utils::regexpLabel(output, QString("(?<=Block size:).*(?=\n)")).trimmed().toLong();
        }
        
        // 获取空闲块数
        strmatch = "Free blocks:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            freeBlocks = Utils::regexpLabel(output, QString("(?<=Free blocks:).*(?=\n)")).trimmed().toLong();
        }
        
        // 获取保留块数
        strmatch = "Reserved block count:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            reservedBlocks = Utils::regexpLabel(output, QString("(?<=Reserved block count:).*(?=\n)")).trimmed().toLong();
        }
        
        // 验证数据有效性并计算扇区使用情况
        if (blockCount > 0 && blockSize > 0 && freeBlocks >= 0 && reservedBlocks >= 0) {
            // 计算用户实际可用的空闲块数：可用 = (freeBlocks - reservedBlocks)
            long long userAvailableBlocks = (freeBlocks > reservedBlocks) ? (freeBlocks - reservedBlocks) : 0;
            
            // 转换为扇区数
            Sector totalSectors = qRound64(blockCount * (blockSize / double(partition.m_sectorSize)));
            Sector availableSectors = qRound64(userAvailableBlocks * (blockSize / double(partition.m_sectorSize)));
            
            qDebug() << "EXT* usage calculation: blockCount=" << blockCount 
                     << "blockSize=" << blockSize 
                     << "freeBlocks=" << freeBlocks 
                     << "reservedBlocks=" << reservedBlocks
                     << "userAvailableBlocks=" << userAvailableBlocks
                     << "totalSectors=" << totalSectors 
                     << "availableSectors=" << availableSectors;
            
            // 如果分区已挂载，使用 stat 获取更准确的可用块数
            if (partition.m_busy && !partition.getMountPoints().empty()) {
                Byte_Value statTotalSize, statAvailableSize;
                if (Utils::getMountedFileSystemUsage(partition.getMountPoint(), statTotalSize, statAvailableSize) == 0) {
                    // 使用 stat 获取的可用空间（已经排除了保留空间）
                    Sector statAvailableSectors = qRound64(statAvailableSize / double(partition.m_sectorSize));
                    qDebug() << "Using stat data for mounted partition: statAvailableSectors=" << statAvailableSectors;
                    partition.setSectorUsage(totalSectors, statAvailableSectors);
                } else {
                    partition.setSectorUsage(totalSectors, availableSectors);
                }
            } else {
                partition.setSectorUsage(totalSectors, availableSectors);
            }
            partition.m_fsBlockSize = blockSize;
        }
    } else {
        qDebug() << __FUNCTION__ << "dumpe2fs -h failed :" << output << error;
    }
}

void EXT2::readLabel(Partition &partition)
{
    QString output, error;
    if (!Utils::executCmd(QString("e2label %1").arg(partition.getPath()), output, error)) {
        partition.setFilesystemLabel(output.trimmed());
    }
//    qDebug() << __FUNCTION__ << output << error;
}

bool EXT2::writeLabel(const Partition &partition)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("e2label %1 %2").arg(partition.getPath()).arg(partition.getFileSystemLabel()), output, error);
//    qDebug() << __FUNCTION__ << output << error;
    return exitcode == 0;
}

void EXT2::readUuid(Partition &partition)
{
    QString output, error;
    if (!Utils::executCmd(QString("tune2fs -l %1").arg(partition.getPath()), output, error)) {
        partition.m_uuid = Utils::regexpLabel(output, "(?<=Filesystem UUID:).*(?=\n)").trimmed();
    }
//    qDebug() << __FUNCTION__ << output << error;
}

bool EXT2::writeUuid(const Partition &partition)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("tune2fs -U random ").arg(partition.getPath()), output, error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

bool EXT2::create(const Partition &new_partition)
{
    QString features, output, error, cmd;
    if (m_forceAuto64bit) {
        // (#766910) Manually implement mke2fs.conf(5) auto_64-bit_support option
        // by setting or clearing the 64bit feature on the command line depending
        // of the partition size.
        if (new_partition.getByteLength() >= 16 * TEBIBYTE)
            features = " -O 64bit";
        else
            features = " -O ^64bit";
    }
    QString strlabel = new_partition.getFileSystemLabel();
    if (strlabel == " ") {
        cmd = QString("%1%2%3%4%5").arg(m_mkfsCmd).arg(" -F").arg(features).arg(" ").arg(new_partition.getPath());
    } else {
        strlabel = strlabel.isEmpty() ? strlabel : QString(" -L %1").arg(strlabel);
        cmd = QString("%1%2%3%4%5%6").arg(m_mkfsCmd).arg(" -F").arg(features).arg(strlabel).arg(" ").arg(new_partition.getPath());
    }
    qDebug() << " EXT2::create***** " << cmd;
    int exitcode = Utils::executCmd(cmd, output, error);
//    qDebug() << "EXT2::create-------" << output << error;
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

/*
The size parameter specifies the requested new size of the filesystem.
If no units are specified, the units of the size parameter shall be the filesystem blocksize of the filesystem.
Optionally, the size parameter may be suffixed by one of the following the units designators: 's', 'K', 'M', or 'G', for 512 byte sectors, kilobytes, megabytes, or gigabytes, respectively.
The size of the filesystem may never be larger than the size of the partition. If size parameter is not specified, it will default to the size of the partition.

size参数指定所请求的文件系统的新大小。如果未指定单位，则size参数的单位应为文件系统的文件系统块大小。
可选地，大小参数可以以下列单位指示符中的一个为后缀：“s”、“K”、“M”或“G”，分别表示512字节扇区、千字节、兆字节或千兆字节。
文件系统的大小永远不会大于分区的大小。如果没有指定size参数，它将默认为分区的大小。
K=1024
M=1024*1024
G=1024*1024*1024
*/
bool EXT2::resize(const Partition &partitionNew, bool fillPartition)
{
    double d = Utils::sectorToUnit(partitionNew.getSectorLength(), partitionNew.m_sectorSize, UNIT_KIB);
    return resize(partitionNew.getPath(), QString::number(((long long)d)), fillPartition);
}

bool EXT2::resize(const QString &path, const QString &size, bool fillPartition)
{
    QString str_temp = QString("resize2fs -p %1").arg(path);

    if (!fillPartition) {
        str_temp = QString("%1 %2K").arg(str_temp).arg(size);
    }
    QString output, error;
    int exitcode = Utils::executCmd(str_temp, output, error);
    return  0 == exitcode || 0 == error.compare("Unknown error");
}

bool EXT2::checkRepair(const Partition &partition)
{
    return checkRepair(partition.getPath());
}

bool EXT2::checkRepair(const QString &devpath)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("e2fsck -f -y -v -C 0 %1").arg(devpath), output, error);
//    qDebug() << QString("EXT2::check_repair---%1----%2").arg(output).arg(error);
    return  0 == exitcode || 0 == error.compare("Unknown error");
}

FS_Limits EXT2::getFilesystemLimits(const Partition &partition)
{
    return getFilesystemLimits(partition.getPath());
}

FS_Limits EXT2::getFilesystemLimits(const QString &path)
{
    m_fsLimits = FS_Limits{-1, -1};
    QString output, error;
    int exitcode = Utils::executCmd(QString("e2fsck -f %1").arg(path), output, error);
    if (exitcode != 0 && error.compare("Unknown error") != 0) {
        return m_fsLimits;
    }


    auto getNumber = [ = ](QString cmd, QString split1, QString split2)->long long{
        QString output, error;
        int exitcode = Utils::executCmd(cmd, output, error);
        if (exitcode != 0 && error.compare("Unknown error") != 0) {
            return -1;
        }

        foreach (QString str, output.split("\n"))
        {
            if (str.contains(split1)) {
                auto list = str.split(split2);
                if (list.count() == 2) {
                    return list[1].trimmed().toLongLong();
                }
            }
        }
        return -1;
    };

    long long blockSize = getNumber(QString("tune2fs -l %1").arg(path), "Block size:", ":");

    if (-1 == blockSize) {
        return m_fsLimits;
    }
    long long blockCount = getNumber(QString("resize2fs -P %1").arg(path), "Estimated minimum size of the filesystem:", ":");

    if (-1 == blockCount) {
        return m_fsLimits;
    }

    m_fsLimits.max_size = 0;
    m_fsLimits.min_size = blockSize * blockCount;

    return m_fsLimits;
}

} // namespace DiskManager
