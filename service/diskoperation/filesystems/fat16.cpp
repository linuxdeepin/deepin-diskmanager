// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "fat16.h"
#include "utils.h"

#include <QDebug>
#include <QStringList>
#if QT_VERSION_MAJOR > 5
#include <QRegularExpression>
#endif


namespace DiskManager {

FS FAT16::getFilesystemSupport()
{
    qDebug() << "Checking FAT16/FAT32 filesystem support";
    FS fs(specificType);

	// hack to disable silly mtools warnings
	setenv( "MTOOLS_SKIP_CHECK", "1", 0 );

    fs.busy = FS::GPARTED ;

    if (!Utils::findProgramInPath("mdir").isEmpty()) {
        qDebug() << "mdir found, enabling uuid and read support";
		fs.read_uuid = FS::EXTERNAL;
        if (!Utils::findProgramInPath("minfo").isEmpty()) {
            qDebug() << "minfo found, enabling read support";
			fs.read = FS::EXTERNAL;
        }
	}

	//find out if we can create fat file systems
    if (!Utils::findProgramInPath( "mkfs.fat" ).isEmpty()) {
        qDebug() << "mkfs.fat found, enabling create support";
		fs.create = FS::EXTERNAL;
		fs.create_with_label = FS::EXTERNAL;
	}

    if (!Utils::findProgramInPath( "fsck.fat" ).isEmpty()) {
        qDebug() << "fsck.fat found, enabling check support";
		fs.check = FS::EXTERNAL;
	}

    if (!Utils::findProgramInPath( "mlabel" ).isEmpty()) {
        qDebug() << "mlabel found, enabling label and uuid write support";
        fs.read_label = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;
        fs.write_uuid = FS::EXTERNAL;
	}

#ifdef HAVE_LIBPARTED_FS_RESIZE
    qDebug() << "libparted fs resize support found, enabling grow and shrink";
	//resizing of start and endpoint are provided by libparted
	fs.grow = FS::LIBPARTED;
	fs.shrink = FS::LIBPARTED;
#endif
    fs.move = FS::GPARTED;
	fs.copy = FS::GPARTED;
    fs.online_read = FS::GPARTED;

    if (fs.fstype == FS_FAT16) {
        qDebug() << "Setting filesystem limits for FAT16";
        m_fsLimits.min_size = 16 * MEBIBYTE;
        m_fsLimits.max_size = (4096 - 1) * MEBIBYTE;  // Maximum seems to be just less than 4096 MiB.
    } else {
        qDebug() << "Setting filesystem limits for FAT32";
        //FS_FAT32
        m_fsLimits.min_size = 33 * MEBIBYTE;  // Smaller file systems will cause Windows' scandisk to fail.

		// Maximum FAT32 volume size with 512 byte sectors is 2 TiB.
		// *   Wikipedia: File Allocation Table / FAT32
		//     https://en.wikipedia.org/wiki/File_Allocation_Table#FAT32
        m_fsLimits.max_size = 2 * TEBIBYTE;
	}

    fs.grow = FS::LIBPARTED;
    fs.shrink = FS::LIBPARTED;
    return fs;
}

void FAT16::setUsedSectors( Partition & partition )
{
    qDebug() << "Setting used sectors for FAT partition:" << partition.getPath();
    QString output, error, strmatch, strcmd;

    Sector logicalSectorSize,clusterSize,smallSize,bigSize;
    logicalSectorSize = clusterSize = smallSize = bigSize = -1;

    strcmd = QString("mdir -i %1 ::/").arg(partition.getPath());

	// Use mdir's scanning of the FAT to get the free space.
	// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#File_Allocation_Table
    if (!Utils::executCmd(strcmd, output, error)) {

	// Bytes free.  Parse the value from the bottom of the directory listing by mdir.
	// Example line "                        277 221 376 bytes free".
#if QT_VERSION_MAJOR > 5
    QString spacedNumberStr = Utils::regexpLabel(output, "^ *([0-9 ]*) bytes free$").remove(QRegularExpression("\\s")).trimmed();
#else
    QString spacedNumberStr = Utils::regexpLabel(output, "^ *([0-9 ]*) bytes free$").remove(QRegExp("\\s")).trimmed();
#endif

    if (spacedNumberStr.size() > 0) {
        qDebug() << "Found free bytes string:" << spacedNumberStr;
        m_numOfFreeOrUsedBlocks = atoll(spacedNumberStr.toStdString().c_str());
    }
    }
	// Use minfo's reporting of the BPB (BIOS Parameter Block) to get the file system
	// size and FS block (cluster) size.
	// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#BIOS_Parameter_Block
    strcmd = QString("minfo -i %1 ::").arg(partition.getPath());
    if (!Utils::executCmd(strcmd, output, error)) {

        QStringList list;

        // FS logical sector size in bytes
        strmatch = "sector size:";
        int index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            qDebug() << "Found sector size";
            list = Utils::regexpLabel(output, QString("(?<=sector size:).*(?=\n)")).trimmed().split(" ");
            logicalSectorSize = list.at(0).toLong();
        }

        // Cluster size in FS logical sectors
        strmatch = "cluster size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            qDebug() << "Found cluster size";
            list = Utils::regexpLabel(output, QString("(?<=cluster size:).*(?=\n)")).trimmed().split(" ");
            clusterSize = list.at(0).toLong();
        }

        // FS size in logical sectors if <= 65535, or 0 otherwise
        strmatch = "small size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            qDebug() << "Found small size";
            list = Utils::regexpLabel(output, QString("(?<=small size:).*(?=\n)")).trimmed().split(" ");
            smallSize = list.at(0).toLong();
        }

        // FS size in logical sectors if > 65535, or 0 otherwise
        strmatch = "big size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            qDebug() << "Found big size";
            list = Utils::regexpLabel(output, QString("(?<=big size:).*(?=\n)")).trimmed().split(" ");
            bigSize = list.at(0).toLong();
        }

        if (bigSize <= 0 && smallSize <= 0) {
            qDebug() << "Big size and small size are 0, parsing mformat command line";
            list = output.split("\n");
            for (int i = 0; i < list.size(); i++) {
                if (list.at(i).contains("mformat command line: mformat -T ")) {
                    qDebug() << "Found mformat command line";
                    QStringList slist = list.at(i).split("-T");
                    list.clear();
                    list = slist.at(1).split("-h");
                    m_totalNumOfBlock = list.at(0).trimmed().toLong();
                    break;
                }
            }
        }
    }

	// FS size in logical sectors
	long long logical_sectors = -1;
    if (smallSize > 0) {
        qDebug() << "Using small size for logical sectors";
        logical_sectors = smallSize;
    } else if (bigSize > 0) {
        qDebug() << "Using big size for logical sectors";
        logical_sectors = bigSize;
    } else {
        qDebug() << "Using total number of blocks for logical sectors";
        logical_sectors = m_totalNumOfBlock;
    }

    if (m_numOfFreeOrUsedBlocks > -1 && logicalSectorSize > -1 && clusterSize > -1 && logical_sectors > -1) {
        qDebug() << "All values found, calculating and setting sector usage";
        Sector fsfree = m_numOfFreeOrUsedBlocks / partition.m_sectorSize;
        Sector fsSize = logical_sectors * logicalSectorSize / partition.m_sectorSize;

//        qDebug() << __FUNCTION__ << fs_free << fs_size;
        partition.setSectorUsage(fsSize, fsfree);
        partition.m_fsBlockSize = logicalSectorSize * clusterSize;
    }

}


void FAT16::readLabel( Partition & partition )
{
    qDebug() << "Reading label for FAT partition:" << partition.getPath();
    QString output, error, filesystemLabel;
    QString partitionPath = partition.getPath().remove("/dev/");
    if (!Utils::executCmd(QString("ls -l /dev/disk/by-label"), output, error)) {
        if(output.indexOf(partitionPath)) {
            qDebug() << "Found partition path in ls output";
            QStringList list = output.split("\n");
            for (int i = 0; i < list.size(); i++) {
                if(list.at(i).indexOf(partitionPath) != -1) {
                    // qDebug() << "Found line with partition path:" << list.at(i);
                    QStringList slist = list.at(i).split(" ");
                    for (int j = 0; j < slist.size(); j++) {
                        if(slist.at(j) == "->" && j != 0) {
                            qDebug() << "Found label:" << slist.at(j-1);
                            filesystemLabel = slist.at(j-1);
                            break;
                        }
                    }
                }
            }
        }
    }
    if (!filesystemLabel.isEmpty()) {
        qDebug() << "Setting filesystem label:" << filesystemLabel;
        partition.setFilesystemLabel(filesystemLabel);
    }

}

bool FAT16::writeLabel(const Partition & partition)
{
    qDebug() << "Writing label for FAT partition:" << partition.getPath()
            << "new label:" << partition.getFileSystemLabel();
    QString output, error, cmd;
    if (partition.getFileSystemLabel().isEmpty() || partition.getFileSystemLabel() == " ") {
        qDebug() << "Label is empty, clearing it";
        cmd = QString("mlabel -c :: -i %1").arg(partition.getPath());
	} else {
        qDebug() << "Label is not empty, setting it";
        cmd = QString("mlabel :: %1 -i %2").arg(partition.getFileSystemLabel()).arg(partition.getPath());
    }

//    qDebug() << __FUNCTION__ << cmd;
    int exitcode = Utils::executCmd(cmd, output, error);
//    qDebug() << __FUNCTION__ << exitcode;
//    qDebug() << __FUNCTION__ << output << error;
    return exitcode == 0;

}

void FAT16::readUuid( Partition & partition )
{
    qDebug() << "Reading UUID for FAT partition:" << partition.getPath();
    QString output, error;
    QString cmd = QString("mdir -f :: -i %1").arg(partition.getPath());

    if (!Utils::executCmd( cmd, output, error)) {
        qDebug() << "mdir successful, parsing volume serial number";
        partition.m_uuid = Utils::regexpLabel(output, "Volume Serial Number is[[:blank:]]([^[:space:]]+)");
        if (partition .m_uuid == "0000-0000") {
            qDebug() << "Volume serial number is 0000-0000, clearing it";
            partition .m_uuid .clear() ;
        }
	}
//    qDebug() << __FUNCTION__ << output << error;
}

bool FAT16::writeUuid(const Partition & partition)
{
    qDebug() << "Writing new UUID for FAT partition:" << partition.getPath();
    QString output, error;
    int exitcode = Utils::executCmd(QString("mlabel -s -n :: -i ").arg(partition.getPath()), output, error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

bool FAT16::create(const Partition & new_partition)
{
    qDebug() << "Creating FAT filesystem on:" << new_partition.getPath()
            << "with label:" << new_partition.getFileSystemLabel();
    QString output, error, cmd;
    QString fat_size = (specificType == FS_FAT16 ? "16" : "32");
    int exitcode = -1;
    if (new_partition.getFileSystemLabel().isEmpty() || new_partition.getFileSystemLabel() == " ") {
        qDebug() << "Label is empty, creating without label";
        cmd = QString("mkfs.fat -F %1 -v -I %2").arg(fat_size).arg(new_partition.getPath());
    } else {
        qDebug() << "Label is not empty, creating with label";
        cmd = QString("mkfs.fat -F %1 -v -I -n %2 %3").arg(fat_size).arg(new_partition.getFileSystemLabel()).arg(new_partition.getPath());
    }

    qDebug() << "FAT16::create***** " << cmd;
    exitcode = Utils::executCmd(cmd ,output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool FAT16::checkRepair(const Partition &partition)
{
    qDebug() << "Checking/repairing FAT filesystem on partition:" << partition.getPath();
    return checkRepair(partition.getPath());
}

bool FAT16::checkRepair(const QString &devpath)
{
    qDebug() << "Checking/repairing FAT filesystem on:" << devpath;
    QString output, error;
    int exitcode = Utils::executCmd(QString("fsck.fat -a -w -v %1").arg(devpath), output, error);
//    qDebug() << QString("EXT2::check_repair---%1----%2").arg(output).arg(error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

FS_Limits FAT16::getFilesystemLimits(const Partition &partition)
{
    qDebug() << "FAT16::getFilesystemLimits(Partition) BEGIN";
    return getFilesystemLimits(partition.getPath());
}

FS_Limits FAT16::getFilesystemLimits(const QString &path)
{
    qDebug() << "FAT16::getFilesystemLimits(path) BEGIN";
    m_fsLimits = FS_Limits {-1, 0};
    if (Utils::findProgramInPath("fatresize").isEmpty()) {
        qDebug() << "fatresize not found, return";
        return m_fsLimits;
    }


    //通过fatresize获取文件系统类型以及最小文件系统
    QString output, error;
    int exitcode = Utils::executCmd(QString("fatresize %1 -i").arg(path), output, error);
    if (exitcode != 0) {
        qDebug() << "fatresize failed, return";
        return m_fsLimits;
    }
    long long minSize = 0, maxSize = 0;
    FSType fs = FS_UNKNOWN;



    foreach (QString str, output.split("\n")) {
        if (str.contains("FAT")) {
            qDebug() << "found FAT string";
            if (str.contains("fat32")) {
                qDebug() << "is fat32";
                fs = FS_FAT32;
            } else if (str.contains("fat16")) {
                qDebug() << "is fat16";
                fs = FS_FAT16;
            }
        }

        if (str.contains("Min size:")) {
            qDebug() << "found Min size";
            QStringList list = str.split(":");
            minSize = list.size() == 2 ? list[1].trimmed().toLongLong() : static_cast<long long>(-1);
        }

        if (str.contains("Max size:")) {
            qDebug() << "found Max size";
            QStringList list = str.split(":");
            maxSize = list.size() == 2 ? list[1].trimmed().toLongLong() : static_cast<long long>(-1);
        }
    }


    if (fs == FS_UNKNOWN) {
        qDebug() << "fs is unknown, return";
        return m_fsLimits;
    }

    if (fs == FS_FAT16) {
        qDebug() << "fs is fat16";
        if (maxSize > 64 * MEBIBYTE) {
            qDebug() << "maxSize > 64MB";
            maxSize = 64 * MEBIBYTE;
        }
    }


    if (fs == FS_FAT32) {
        qDebug() << "fs is fat32";
        if (minSize < 512 * MEBIBYTE) {
            qDebug() << "minSize < 512MB";
            minSize = 512 * MEBIBYTE;
        }
    }

    m_fsLimits.max_size = maxSize;
    m_fsLimits.min_size = minSize;
    qDebug() << "FAT16::getFilesystemLimits(path) END";
    return m_fsLimits;
}

//fat格式需要操作分区表 lvm不支持fat格式的调整大小  fatresize命令也不是一个好的调整分区大小方式 相对来说libparted库更合适
bool FAT16::resize(const Partition &partitionNew, bool fillPartition)
{
    // qDebug() << "FAT16::resize(Partition) BEGIN";
    double d = Utils::sectorToUnit(partitionNew.getSectorLength(), partitionNew.m_sectorSize, UNIT_KIB);
    return resize(partitionNew.getPath(), QString::number(((long long)d)), fillPartition);
}
/*
fatresize
-s --size
    Resize volume to SIZE[k|M|G|ki|Mi|Gi]
*/
bool FAT16::resize(const QString &path, const QString &size, bool fillPartition)
{
    // qDebug() << "FAT16::resize(path, size) BEGIN";
    Q_UNUSED(fillPartition)
    //通过fatresize获取文件系统类型以及最小文件系统
    QString output, error;
    int exitcode = Utils::executCmd(QString("fatresize %1 -s %2ki").arg(path).arg(size), output, error);
    qDebug() << "FAT16::resize(path, size) END";
    return  0 == exitcode || error.compare("Unknown error") == 0;
}


} //DiskManager
