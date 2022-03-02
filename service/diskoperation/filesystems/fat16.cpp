/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file fat16.cpp
 *
 * @brief FAT16(32)格式操作类
 *
 * @date 2021-01-04 14:14
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
#include "fat16.h"
#include "utils.h"

#include <QDebug>
#include <QStringList>


namespace DiskManager
{

FS FAT16::getFilesystemSupport()
{
    FS fs(specificType);

	// hack to disable silly mtools warnings
	setenv( "MTOOLS_SKIP_CHECK", "1", 0 );

    fs.busy = FS::GPARTED ;

    if (!Utils::findProgramInPath("mdir").isEmpty()) {
		fs.read_uuid = FS::EXTERNAL;
        if (!Utils::findProgramInPath("minfo").isEmpty())
			fs.read = FS::EXTERNAL;
	}

	//find out if we can create fat file systems
    if (!Utils::findProgramInPath( "mkfs.fat" ).isEmpty()) {
		fs.create = FS::EXTERNAL;
		fs.create_with_label = FS::EXTERNAL;
	}

    if (!Utils::findProgramInPath( "fsck.fat" ).isEmpty()) {
		fs.check = FS::EXTERNAL;
	}

    if (!Utils::findProgramInPath( "mlabel" ).isEmpty()) {
        fs.read_label = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;
        fs.write_uuid = FS::EXTERNAL;
	}

#ifdef HAVE_LIBPARTED_FS_RESIZE
	//resizing of start and endpoint are provided by libparted
	fs.grow = FS::LIBPARTED;
	fs.shrink = FS::LIBPARTED;
#endif
    fs.move = FS::GPARTED;
	fs.copy = FS::GPARTED;
    fs.online_read = FS::GPARTED;

    if (fs.fstype == FS_FAT16) {
        m_fsLimits.min_size = 16 * MEBIBYTE;
        m_fsLimits.max_size = (4096 - 1) * MEBIBYTE;  // Maximum seems to be just less than 4096 MiB.
    } else {
        //FS_FAT32
        m_fsLimits.min_size = 33 * MEBIBYTE;  // Smaller file systems will cause Windows' scandisk to fail.

		// Maximum FAT32 volume size with 512 byte sectors is 2 TiB.
		// *   Wikipedia: File Allocation Table / FAT32
		//     https://en.wikipedia.org/wiki/File_Allocation_Table#FAT32
        m_fsLimits.max_size = 2 * TEBIBYTE;
	}

    return fs;
}

void FAT16::setUsedSectors( Partition & partition )
{
    QString output, error, strmatch, strcmd;

    Sector logicalSectorSize,clusterSize,smallSize,bigSize;
    logicalSectorSize = clusterSize = smallSize = bigSize = -1;

    strcmd = QString("mdir -i %1 ::/").arg(partition.getPath());

	// Use mdir's scanning of the FAT to get the free space.
	// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#File_Allocation_Table
    if (!Utils::executCmd(strcmd, output, error)) {

	// Bytes free.  Parse the value from the bottom of the directory listing by mdir.
	// Example line "                        277 221 376 bytes free".
    QString spacedNumberStr = Utils::regexpLabel(output, "^ *([0-9 ]*) bytes free$").remove(QRegExp("\\s")).trimmed();

    if (spacedNumberStr.size() > 0)
        m_numOfFreeOrUsedBlocks = atoll(spacedNumberStr.toStdString().c_str());
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
            list = Utils::regexpLabel(output, QString("(?<=sector size:).*(?=\n)")).trimmed().split(" ");
            logicalSectorSize = list.at(0).toLong();
        }

        // Cluster size in FS logical sectors
        strmatch = "cluster size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            list = Utils::regexpLabel(output, QString("(?<=cluster size:).*(?=\n)")).trimmed().split(" ");
            clusterSize = list.at(0).toLong();
        }

        // FS size in logical sectors if <= 65535, or 0 otherwise
        strmatch = "small size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            list = Utils::regexpLabel(output, QString("(?<=small size:).*(?=\n)")).trimmed().split(" ");
            smallSize = list.at(0).toLong();
        }

        // FS size in logical sectors if > 65535, or 0 otherwise
        strmatch = "big size:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            list = Utils::regexpLabel(output, QString("(?<=big size:).*(?=\n)")).trimmed().split(" ");
            bigSize = list.at(0).toLong();
        }

        if (bigSize <= 0 && smallSize <= 0) {
            list = output.split("\n");
            for (int i = 0; i < list.size(); i++) {
                if (list.at(i).contains("mformat command line: mformat -T ")) {
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
        logical_sectors = smallSize;
    } else if (bigSize > 0) {
        logical_sectors = bigSize;
    } else {
        logical_sectors = m_totalNumOfBlock;
    }

    if (m_numOfFreeOrUsedBlocks > -1 && logicalSectorSize > -1 && clusterSize > -1 && logical_sectors > -1) {
        Sector fsfree = m_numOfFreeOrUsedBlocks / partition.m_sectorSize;
        Sector fsSize = logical_sectors * logicalSectorSize / partition.m_sectorSize;

//        qDebug() << __FUNCTION__ << fs_free << fs_size;
        partition.setSectorUsage(fsSize, fsfree);
        partition.m_fsBlockSize = logicalSectorSize * clusterSize;
    }

}


void FAT16::readLabel( Partition & partition )
{
    QString output, error, filesystemLabel;
    QString partitionPath = partition.getPath().remove("/dev/");
    if (!Utils::executCmd(QString("ls -l /dev/disk/by-label"), output, error)) {
        if(output.indexOf(partitionPath)) {
            QStringList list = output.split("\n");
            for (int i = 0; i < list.size(); i++) {
                if(list.at(i).indexOf(partitionPath) != -1) {
                    QStringList slist = list.at(i).split(" ");
                    for (int j = 0; j < slist.size(); j++) {
                        if(slist.at(j) == "->" && j != 0) {
                            filesystemLabel = slist.at(j-1);
                            break;
                        }
                    }
                }
            }
        }
    }
    if (!filesystemLabel.isEmpty()) {
        partition.setFilesystemLabel(filesystemLabel);
    }

}

bool FAT16::writeLabel(const Partition & partition)
{
    QString output, error, cmd;
    if (partition.getFileSystemLabel().isEmpty() || partition.getFileSystemLabel() == " ")
        cmd = QString("mlabel -c :: -i %1").arg(partition.getPath());
	else
        cmd = QString("mlabel :: %1 -i %2").arg(partition.getFileSystemLabel()).arg(partition.getPath());

//    qDebug() << __FUNCTION__ << cmd;
    int exitcode = Utils::executCmd(cmd, output, error);
//    qDebug() << __FUNCTION__ << exitcode;
//    qDebug() << __FUNCTION__ << output << error;
    return exitcode == 0;

}

void FAT16::readUuid( Partition & partition )
{
    QString output, error;
    QString cmd = QString("mdir -f :: -i %1").arg(partition.getPath());

    if (!Utils::executCmd( cmd, output, error)) {
        partition.m_uuid = Utils::regexpLabel(output, "Volume Serial Number is[[:blank:]]([^[:space:]]+)");
        if (partition .m_uuid == "0000-0000")
            partition .m_uuid .clear() ;
	}
//    qDebug() << __FUNCTION__ << output << error;
}

bool FAT16::writeUuid(const Partition & partition)
{

    QString output, error;
    int exitcode = Utils::executCmd(QString("mlabel -s -n :: -i ").arg(partition.getPath()), output, error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

bool FAT16::create(const Partition & new_partition)
{
    QString output, error, cmd;
    QString fat_size = (specificType == FS_FAT16 ? "16" : "32");
    int exitcode = -1;
    if(new_partition.getFileSystemLabel().isEmpty() || new_partition.getFileSystemLabel() == " ") {
        exitcode = Utils::executCmd(QString("mkfs.fat -F %1 -v -I %2").arg(fat_size).arg(new_partition.getPath()), output, error);
    } else {
        cmd = QString("mkfs.fat -F %1 -v -I -n %2 %3").arg(fat_size).arg(new_partition.getFileSystemLabel()).arg(new_partition.getPath());
        exitcode = Utils::executCmd(cmd ,output, error);
    }
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool FAT16::checkRepair(const Partition & partition)
{
    return checkRepair(partition.getPath());
}

bool FAT16::checkRepair(const QString &devpath)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("fsck.fat -a -w -v %1").arg(devpath), output, error);
//    qDebug() << QString("EXT2::check_repair---%1----%2").arg(output).arg(error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

FS_Limits FAT16::getFilesystemLimits(const Partition &partition) const
{
    return getFilesystemLimits(partition.getPath());
}

FS_Limits FAT16::getFilesystemLimits(const QString &path) const
{
    FS_Limits tmp {-1, 0};

    //todo fatresize
    return tmp;
}

bool FAT16::resize(const Partition &partitionNew, bool fillPartition)
{
    return resize(partitionNew.getPath(),QString::number(Utils::sectorToUnit(partitionNew.getSectorLength(), partitionNew.m_sectorSize, UNIT_KIB)),fillPartition);
}

bool FAT16::resize(const QString &path, const QString &size, bool fillPartition)
{

}


} //DiskManager
