// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ntfs.h"
#include "filesystem.h"
#include "utils.h"

#include <QDebug>

namespace DiskManager {

FS NTFS::getFilesystemSupport()
{
    qDebug() << "[NTFS]::getFilesystemSupport - Enter";
    FS fs(FS_NTFS);

    fs.busy = FS::GPARTED;

    if (!Utils::findProgramInPath("ntfsinfo").isEmpty()) {
        qDebug() << "ntfsinfo found, enabling read support.";
        fs.read = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("ntfslabel").isEmpty()) {
        qDebug() << "ntfslabel found, enabling label and uuid support.";
        fs .read_label = FS::EXTERNAL ;
        fs .write_label = FS::EXTERNAL ;
        fs.write_uuid = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("mkntfs").isEmpty()) {
        qDebug() << "mkntfs found, enabling create support.";
        fs.create = FS::EXTERNAL;
        fs.create_with_label = FS::EXTERNAL;
    }

    //resizing is a delicate process ...
    if (!Utils::findProgramInPath("ntfsresize").isEmpty()) {
        qDebug() << "ntfsresize found, enabling check, grow, shrink, move support.";
        fs.check = FS::EXTERNAL;
        fs.grow = FS::EXTERNAL;

        if (fs.read)  {//needed to determine a min file system size..
            qDebug() << "Read support is available, enabling shrink.";
            fs.shrink = FS::EXTERNAL;
        }

        fs.move = FS::GPARTED;
    }

    if (!Utils::findProgramInPath("ntfsclone").isEmpty()) {
        qDebug() << "ntfsclone found, enabling copy support.";
        fs.copy = FS::EXTERNAL;
    }

    fs.online_read = FS::GPARTED;

    //Minimum NTFS partition size = (Minimum NTFS volume size) + (backup NTFS boot sector)
    //                            = (1 MiB) + (1 sector)
    // For GParted this means 2 MiB because smallest GUI unit is MiB.
    m_fsLimits.min_size = 2 * MEBIBYTE;

    qDebug() << "[NTFS]::getFilesystemSupport - Exit";
    return fs;
}

void NTFS::setUsedSectors( Partition & partition )
{
    qDebug() << "[NTFS]::setUsedSectors - Enter";
    QString output, error, strmatch;

    m_blocksSize = m_numOfFreeOrUsedBlocks = m_totalNumOfBlock = -1;
    QString cmd = QString("ntfsinfo --mft --force %1").arg(partition.getPath());

    if (!Utils::executCmd(cmd, output, error)) {
        qDebug() << "ntfsinfo executed successfully.";
        strmatch = "Cluster Size:";
        int index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            m_blocksSize = Utils::regexpLabel(output, QString("(?<=Cluster Size:).*(?=\n)")).trimmed().toLong();
            qDebug() << "Found Cluster Size:" << m_blocksSize;
        }

        strmatch = "Volume Size in Clusters:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            m_totalNumOfBlock = Utils::regexpLabel(output, QString("(?<=Volume Size in Clusters:).*(?=\n)")).trimmed().toLong();
            qDebug() << "Found Volume Size in Clusters:" << m_totalNumOfBlock;
        }

        strmatch = "Free Clusters:";
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            m_numOfFreeOrUsedBlocks = Utils::regexpLabel(output, QString("(?<=Free Clusters:).*(?=[(])")).trimmed().toLong();
            qDebug() << "Found Free Clusters:" << m_numOfFreeOrUsedBlocks;
        }
    } else {
        qDebug() << "ntfsinfo execution failed. Error:" << error;
    }

    if (m_blocksSize > -1 && m_totalNumOfBlock > -1 && m_numOfFreeOrUsedBlocks > -1) {
        qDebug() << "Calculating and setting sector usage.";
        m_totalNumOfBlock = m_totalNumOfBlock * (m_blocksSize / partition.m_sectorSize);
        m_numOfFreeOrUsedBlocks = m_numOfFreeOrUsedBlocks * (m_blocksSize / partition.m_sectorSize);

        partition.setSectorUsage(m_totalNumOfBlock, m_numOfFreeOrUsedBlocks);
        partition.m_fsBlockSize = m_blocksSize;
    }

    qDebug() << "[NTFS]::setUsedSectors - Exit";
}

void NTFS::readLabel( Partition & partition )
{
    qDebug() << "[NTFS]::readLabel - Enter";
    QString output, error;
    if (!Utils::executCmd(QString("ntfslabel --force %1").arg(partition.getPath()), output, error)) {
        qDebug() << "Successfully read label:" << output.trimmed();
        partition.setFilesystemLabel(output.trimmed());
    } else {
        qDebug() << "Failed to read label. Error:" << error;
    }
    qDebug() << "[NTFS]::readLabel - Exit";
}

bool NTFS::writeLabel( const Partition & partition)
{
    qDebug() << "[NTFS]::writeLabel - Enter";
    QString output, error;
    int exitcode = Utils::executCmd(QString("ntfslabel --force %1 %2").arg(partition.getPath()).arg(partition.getFileSystemLabel()), output, error);
    bool success = (exitcode == 0);
    qDebug() << "[NTFS]::writeLabel - Exit with status:" << success;
    return success;
}

void NTFS::readUuid(Partition & partition)
{
    qDebug() << "[NTFS]::readUuid - Enter/Exit";
}

bool NTFS::writeUuid( const Partition & partition)
{
    qDebug() << "[NTFS]::writeUuid - Enter";
    QString output, error;
    int exitcode = Utils::executCmd(QString("ntfslabel --new-serial %1").arg(partition.getPath()), output, error);
    bool success = (exitcode == 0 || error.compare("Unknown error") == 0);
    qDebug() << "[NTFS]::writeUuid - Exit with status:" << success;
    return success;
}

bool NTFS::create(const Partition & newPartition)
{
    qDebug() << "[NTFS]::create - Enter";
    QString output, error;
    int exitcode = -1;
    if (newPartition.getFileSystemLabel().isEmpty() || newPartition.getFileSystemLabel() == " ") {
        qDebug() << "Creating NTFS without label on" << newPartition.getPath();
        exitcode = Utils::executCmd(QString("mkntfs -Q -v -F %1").arg(newPartition.getPath()), output, error);
    } else {
        qDebug() << "Creating NTFS with label" << newPartition.getFileSystemLabel() << "on" << newPartition.getPath();
        exitcode = Utils::executCmd(QString("mkntfs -Q -v -F %1 -L %2").arg(newPartition.getPath()).arg(newPartition.getFileSystemLabel()),output, error);
    }
    bool success = (exitcode == 0 && error.compare("Unknown error") == 0);
    qDebug() << "[NTFS]::create - Exit with status:" << success;
    return success;
}

/*
Ntfsresize can be used to shrink or enlarge any NTFS filesystem located on an unmounted DEVICE (usually a disk partition).
The new filesystem will have SIZE bytes.
The SIZE parameter may have one of the optional modifiers k, M, G, which means the SIZE parameter is given in kilo-,mega- or gigabytes respectively.
Ntfsresize conforms to the SI, ATA, IEEE standards and the disk manufacturers by using k=10^3, M=10^6 and G=10^9.
Ntfsresize可用于缩小或扩大位于未安装设备（通常是磁盘分区）上的任何NTFS文件系统。
新的文件系统将具有SIZE字节。
尺寸参数可以具有可选修饰符K、M、G中的一个，这意味着尺寸参数分别以千字节、兆字节或千兆字节给出。
NTFSRESIZE使用K=10^3、M=10^6和G=10^9，符合Si、ATA、IEEE标准和磁盘制造商的要求。
*/

bool NTFS::resize(const Partition &partitionNew, bool fillPartition)
{
    // qDebug() << "[NTFS]::resize(Partition) - Enter, partition:" << partitionNew.getPath() << "fill:" << fillPartition;
    double d = Utils::sectorToUnit(partitionNew.getSectorLength(), partitionNew.m_sectorSize, UNIT_BYTE);
    bool result = resize(partitionNew.getPath(), QString::number(((long long)d / 1000)), fillPartition);
    // qDebug() << "[NTFS]::resize(Partition) - Exit with status:" << result;
    return  result;
}

bool NTFS::resize(const QString &path, const QString &sizeByte, bool fillPartition)
{
    // qDebug() << "[NTFS]::resize(path) - Enter, path:" << path << "size(k):" << sizeByte << "fill:" << fillPartition;
    //  bool success;
    QString output, error;
    QString size, cmd;
    if (!fillPartition) {
        size = QString(" -s %1k").arg(sizeByte);
        // qDebug() << "Shrinking filesystem, size arg:" << size;
    }

    //Utils::executCmd(QString("umount %1").arg(path));

    cmd = "ntfsresize --force --force" + size ;
    // Real resize
    cmd = QString("%1 %2").arg(cmd).arg(path);
    // qDebug() << "Executing ntfsresize command:" << cmd;
    //    success = !Utils::executCmd(cmd, output, error);
    bool success = !Utils::executCmd(cmd, output, error);
    // qDebug() << "[NTFS]::resize(path) - Exit with status:" << success << "output:" << output << "error:" << error;
    return success;
}

//bool ntfs::copy( const Partition & src_part, Partition & dest_part)
//{
//	return ! execute_command( "ntfsclone -f --overwrite " + Glib::shell_quote( dest_part.get_path() ) +
//	                          " " + Glib::shell_quote( src_part.get_path() ),
//	                          operationdetail,
//	                          EXEC_CHECK_STATUS|EXEC_CANCEL_SAFE|EXEC_PROGRESS_STDOUT,
//		                  static_cast<StreamSlot>( sigc::mem_fun( *this, &ntfs::clone_progress ) ) );
//}

bool NTFS::checkRepair(const Partition &partition)
{
    qDebug() << "[NTFS]::checkRepair - Enter";

    bool result = checkRepair(partition.getPath());
    qDebug() << "[NTFS]::checkRepair - return " << result;
    return result;
}

bool NTFS::checkRepair(const QString &devpath)
{
    qDebug() << "[NTFS]::checkRepair(path) - Enter";
    QString output, error;
    int exitcode = Utils::executCmd(QString("ntfsresize -i -f -v %1").arg(devpath), output, error);
    bool success = (exitcode == 0 || error.compare("Unknown error") == 0);
    qDebug() << "[NTFS]::checkRepair(path) - Exit with status:" << success;
    return success;
}

FS_Limits NTFS::getFilesystemLimits(const Partition &partition)
{
    // qDebug() << "[NTFS]::getFilesystemLimits(Partition) - Enter, for partition:" << partition.getPath();
    FS_Limits limits = getFilesystemLimits(partition.getPath());
    // qDebug() << "[NTFS]::getFilesystemLimits(Partition) - Exit";
    return limits;
}

FS_Limits NTFS::getFilesystemLimits(const QString &path)
{
    qDebug() << "[NTFS]::getFilesystemLimits(path) - Enter, for path:" << path;
    m_fsLimits = FS_Limits {-1, -1};
    QString cmd, output, error;
    //Utils::executCmd(QString("umount %1").arg(path), output, error);
    cmd = QString("ntfsresize -m -f %1").arg(path);
    qDebug() << "Executing ntfsresize command:" << cmd;
    if (Utils::executCmd(cmd, output, error) != 0 && error.compare("Unknown error") != 0) {
        qDebug() << "ntfsresize execution failed. Error:" << error;
        return m_fsLimits;
    }

    foreach (QString str, output.split("\n")) {
        // qDebug()<<"getFilesystemLimits ntfs"<<str;
        if (str.contains("Minsize (in MB):")) {
            // qDebug() << "Found Minsize line:" << str;
            auto list = str.split(":");
            //qDebug()<<"getFilesystemLimits ntfs list"<<list;
            if (list.count() == 2) {
                m_fsLimits.min_size = list[1].toLongLong() * 1000 * 1000; //转换为byte  
                //qDebug()<<"getFilesystemLimits ntfs min_size"<<tmp.min_size;
                m_fsLimits.max_size = 0;
                return m_fsLimits;
            }
        }
    }
    qDebug() << "[NTFS]::getFilesystemLimits(path) - Exit, limits not found.";
    return m_fsLimits;
}

} //DiskManager
