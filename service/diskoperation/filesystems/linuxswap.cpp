/* modified by Uniontech Software Technology Co., Ltd. */
/* Copyright (C) 2004 Bart
 * Copyright (C) 2008, 2009, 2010 Curtis Gedak
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "linuxswap.h"
#include "utils.h"
#include "diskoperation/blockspecial.h"

#include <QFile>
#include <QDebug>

namespace DiskManager {

FS LinuxSwap::getFilesystemSupport()
{
    qDebug() << "Checking Linux swap filesystem support";
    FS fs(FS_LINUX_SWAP);

    fs.busy = FS::GPARTED;
    fs.read = FS::EXTERNAL;
    fs.online_read = FS::EXTERNAL;

    if (!Utils::findProgramInPath("mkswap").isEmpty()) {
        qDebug() << "mkswap found, enabling create, grow, shrink, copy, move support.";
        fs.create = FS::EXTERNAL;
        fs.create_with_label = FS::EXTERNAL;
        fs.grow = FS::EXTERNAL;
        fs.shrink = FS::EXTERNAL;
        fs.copy = FS::EXTERNAL;
        fs.move = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("swaplabel").isEmpty()) {
        qDebug() << "swaplabel found, enabling label and uuid support.";
        fs.read_label = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;
        fs.read_uuid = FS::EXTERNAL;
        fs.write_uuid = FS::EXTERNAL;
    }

    return fs;
}

void LinuxSwap::setUsedSectors(Partition &partition)
{
    qDebug() << "Setting used sectors for swap partition:" << partition.getPath();
    if (partition.m_busy) {
        qDebug() << "Partition is busy, checking /proc/swaps.";
        m_numOfFreeOrUsedBlocks = -1;
        QFile file("/proc/swaps");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "/proc/swaps opened successfully.";
            QString line = file.readLine();
            BlockSpecial bsPath = BlockSpecial(partition.getPath());
            while (!file.atEnd() || !line.isEmpty()) {
                QString filename = Utils::regexpLabel(line, ".*?(?= )");
                if (bsPath == BlockSpecial(filename)) {
                    qDebug() << "Found swap entry for partition" << partition.getPath();
                    sscanf(line.toLatin1(), "%*s %*s %*d %lld", &m_numOfFreeOrUsedBlocks);
                    break;
                }
                line = file.readLine();
            }
            file.close();
        } else {
            qDebug() << "Failed to open /proc/swaps.";
        }
        if (m_numOfFreeOrUsedBlocks > -1) {
            qDebug() << "Calculating and setting sector usage.";
            // Ignore swap space reported size to ignore 1 page format
            // overhead.  Instead use partition size as sectors_fs_size so
            // reported used figure for active swap space starts from 0
            // upwards, matching what 'swapon -s' reports.
            m_totalNumOfBlock = partition.getSectorLength();
            m_numOfFreeOrUsedBlocks = qRound(m_numOfFreeOrUsedBlocks * (KIBIBYTE / double(partition.m_sectorSize)));
            partition.setSectorUsage(m_totalNumOfBlock, m_totalNumOfBlock - m_numOfFreeOrUsedBlocks);
        }
    } else {
        qDebug() << "Partition is not busy, setting as 100% free.";
        //By definition inactive swap space is 100% free
        Sector size = partition.getSectorLength();
        partition.setSectorUsage(size, size);
    }
}

void LinuxSwap::readLabel(Partition &partition)
{
    qDebug() << "Reading label for swap partition:" << partition.getPath();
    QString output, error, label;
    Utils::executCmd(QString("swaplabel %1").arg(partition.getPath()), output, error);
    label = Utils::regexpLabel(output, "(?<=LABEL:).*(?=\n)");
    partition.setFilesystemLabel(label);

//    qDebug() << output << error << "----" << label;
}

bool LinuxSwap::writeLabel(const Partition &partition)
{
    qDebug() << "Writing label for swap partition:" << partition.getPath()
            << "new label:" << partition.getFileSystemLabel();
    QString output, error;
    int exitcode = Utils::executCmd(QString("swaplabel -L %1 %2").arg(partition.getFileSystemLabel()).arg(partition.getPath()),
                                    output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

void LinuxSwap::readUuid(Partition &partition)
{
    qDebug() << "Reading UUID for swap partition:" << partition.getPath();
    QString output, error;
    Utils::executCmd(QString("swaplabel %1").arg(partition.getPath()), output, error);
    partition.m_uuid = Utils::regexpLabel(output, "(?<=UUID:).*(?=\n)");
}

bool LinuxSwap::writeUuid(const Partition &partition)
{
    qDebug() << "Writing new UUID for swap partition:" << partition.getPath();
    QString output, error;
    int exitcode = Utils::executCmd(QString("swaplabel -L %1 %2").arg(Utils::createUuid()).arg(partition.getPath()),
                                    output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool LinuxSwap::create(const Partition &new_partition)
{
    qDebug() << "Creating swap partition on:" << new_partition.getPath()
            << "with label:" << new_partition.getFileSystemLabel();
    QString output, error;
    int exitcode = Utils::executCmd(QString("mkswap -L %1 %2").arg(new_partition.getFileSystemLabel()).arg(new_partition.getPath()),
                                    output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

bool LinuxSwap::resize(const Partition &partitionNew, bool fillPartition)
{
    qDebug() << "Resizing swap partition:" << partitionNew.getPath()
            << "fill partition:" << fillPartition;
    QString output, error;
    QString command = QString("mkswap -L %1 ").arg(partitionNew.getFileSystemLabel());
    if (!partitionNew.m_uuid.isEmpty()) {
        qDebug() << "Adding UUID to resize command:" << partitionNew.m_uuid;
        command.append(QString(" -U %1 ").arg(partitionNew.m_uuid));
    }
    command.append(partitionNew.getPath());
    int exitcode = Utils::executCmd(command, output, error);
    return exitcode == 0 && error.compare("Unknown error") == 0;
}

} // namespace DiskManager
