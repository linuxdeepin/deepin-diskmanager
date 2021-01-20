/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file ntfs.cpp
 *
 * @brief NTFS格式操作类
 *
 * @date 2021-01-04 14:13
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
#include "ntfs.h"
#include "filesystem.h"
#include "utils.h"

#include <QDebug>

namespace DiskManager
{

FS ntfs::getFilesystemSupport()
{
	FS fs( FS_NTFS );

	fs .busy = FS::GPARTED ;

    if (!Utils::findProgramInPath("ntfsinfo").isEmpty())
		fs.read = FS::EXTERNAL;

    if (! Utils::findProgramInPath("ntfslabel").isEmpty())
	{
		fs .read_label = FS::EXTERNAL ;
		fs .write_label = FS::EXTERNAL ;
		fs.write_uuid = FS::EXTERNAL;
	}

    if (!Utils::findProgramInPath( "mkntfs" ).isEmpty())
	{
		fs.create = FS::EXTERNAL;
		fs.create_with_label = FS::EXTERNAL;
	}

	//resizing is a delicate process ...
    if (!Utils::findProgramInPath("ntfsresize").isEmpty())
	{
		fs.check = FS::EXTERNAL;
		fs.grow = FS::EXTERNAL;

		if ( fs .read ) //needed to determine a min file system size..
			fs.shrink = FS::EXTERNAL;

		fs.move = FS::GPARTED;
	}

    if (!Utils::findProgramInPath( "ntfsclone" ).isEmpty())
		fs.copy = FS::EXTERNAL;

    fs.online_read = FS::GPARTED ;

	//Minimum NTFS partition size = (Minimum NTFS volume size) + (backup NTFS boot sector)
	//                            = (1 MiB) + (1 sector)
	// For GParted this means 2 MiB because smallest GUI unit is MiB.
    m_fsLimits.min_size = 2 * MEBIBYTE;

	return fs ;
}

void ntfs::setUsedSectors( Partition & partition )
{
    QString output, error, strmatch;

    m_blocksSize = m_numOfFreeOrUsedBlocks = m_totalNumOfBlock = -1;
    QString cmd = QString("ntfsinfo --mft --force %1").arg(partition.getPath());

    if (!Utils::executCmd(cmd, output, error))
    {
        strmatch = ("Cluster Size:");
        int index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            m_blocksSize = Utils::regexpLabel(output, QString("(?<=Cluster Size:).*(?=\n)")).trimmed().toLong();
//             qDebug() << __FUNCTION__ << m_blocksSize << "22222222222" << endl;
        }

        strmatch = ("Volume Size in Clusters:");
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            m_totalNumOfBlock = Utils::regexpLabel(output, QString("(?<=Volume Size in Clusters:).*(?=\n)")).trimmed().toLong();
//            qDebug() << __FUNCTION__ << m_totalNumOfBlock << "33333333333333" << endl;
        }

        strmatch = ("Free Clusters:");
        index = output.indexOf(strmatch);
        if (index >= 0 && index < output.length()) {
            m_numOfFreeOrUsedBlocks = Utils::regexpLabel(output, QString("(?<=Free Clusters:).*(?=[(])")).trimmed().toLong();
//            qDebug() << __FUNCTION__ << m_numOfFreeOrUsedBlocks << "4444444444444444" << endl;
        }
    }
    if (m_blocksSize > -1 && m_totalNumOfBlock > -1 && m_numOfFreeOrUsedBlocks > -1)
    {
        m_totalNumOfBlock = m_totalNumOfBlock * (m_blocksSize / partition.m_sectorSize);
        m_numOfFreeOrUsedBlocks = m_numOfFreeOrUsedBlocks * (m_blocksSize / partition.m_sectorSize);

//        qDebug() << __FUNCTION__ << m_totalNumOfBlock << m_numOfFreeOrUsedBlocks << "1111111111111111111" << endl;

        partition.setSectorUsage(m_totalNumOfBlock, m_numOfFreeOrUsedBlocks);
        partition.m_fsBlockSize = m_blocksSize;
    }

}

void ntfs::readLabel( Partition & partition )
{
    QString output, error;
    if (!Utils::executCmd(QString("ntfslabel --force").arg(partition.getPath()), output, error)) {
        partition.setFilesystemLabel(output.trimmed());
    }
}

bool ntfs::writeLabel( const Partition & partition)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("ntfslabel --force %1 %2").arg(partition.getPath()).arg(partition.getFileSystemLabel()), output, error);
//    qDebug() << __FUNCTION__ << output << error;
    return exitcode == 0;
}

void ntfs::readUuid(Partition & partition)
{
}

bool ntfs::writeUuid( const Partition & partition)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("ntfslabel --new-serial ").arg(partition.getPath()), output, error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

bool ntfs::create(const Partition & newPartition)
{
    QString output, error;
    int exitcode = -1;
    if(newPartition.getFileSystemLabel().isEmpty() || newPartition.getFileSystemLabel() == " ")
    {
        exitcode = Utils::executCmd(QString("mkntfs -Q -v -F %1").arg(newPartition.getPath()), output, error);
    }else {
        exitcode = Utils::executCmd(QString("mkntfs -Q -v -F %1 -L %2").arg(newPartition.getPath()).arg(newPartition.getFileSystemLabel()),output, error);
    }
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool ntfs::resize(const Partition & partitionNew, bool fillPartition )
{
	bool success;
    QString output, error;
    QString size;
    if ( ! fillPartition )
	{
        size = QString(" -s %1").arg(floor(Utils::sectorToUnit(partitionNew.getSectorLength(), partitionNew.m_sectorSize, UNIT_BYTE)));
	}
    QString cmd = "ntfsresize --force --force" + size ;

	// Real resize
    cmd = QString("%1 %2").arg(cmd).arg(partitionNew.getPath());
    success = !Utils::executCmd(cmd, output, error);


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

bool ntfs::checkRepair( const Partition & partition)
{
    QString output, error;
    int exitcode = Utils::executCmd(QString("ntfsresize -i -f -v %1").arg(partition.getPath()), output, error);
//    qDebug() << QString("NTFS::check_repair---%1----%2").arg(output).arg(error);
    return exitcode == 0 || error.compare("Unknown error") == 0;
}

} //DiskManager
