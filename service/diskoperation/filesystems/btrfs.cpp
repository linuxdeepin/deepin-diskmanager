// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "btrfs.h"
#include "fsinfo.h"
#include <QDir>
#include <QDebug>

namespace DiskManager
{

std::map<BlockSpecial, BTRFS_Device> btrfs_device_cache;

//Btrfs::Btrfs(FSType type)
//{
//
//}

FS Btrfs::getFilesystemSupport()
{
    qDebug() << "Checking Btrfs filesystem support";
    FS fs(FS_BTRFS);

    fs.busy = FS::GPARTED ;

    if (!Utils::findProgramInPath("mdir").isEmpty())
    {
        qDebug() << "mdir found, enabling create support";
        fs.create = FS::EXTERNAL;
        fs.create_with_label = FS::EXTERNAL;
    }

    if (!Utils::findProgramInPath("btrfs").isEmpty())
    {
        qDebug() << "btrfs found, enabling read, check, and write_label support";
        fs.read = FS::EXTERNAL;
        fs .read_label = FS::EXTERNAL ;
        fs .read_uuid = FS::EXTERNAL ;
        fs.check = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;

        //Resizing of btrfs requires mount, umount and kernel
        //  support as well as btrfs filesystem resize
        if (!Utils::findProgramInPath("mount").isEmpty()
                && !Utils::findProgramInPath( "umount" ) .isEmpty()
                && fs.check
                && Utils::kernelSupportFS( "btrfs" ))
        {
            qDebug() << "mount, umount, and kernel support found, enabling resize support";
            fs.grow = FS::EXTERNAL ;
            if (fs.read) { //needed to determine a minimum file system size.
                qDebug() << "read support found, enabling shrink support";
                fs.shrink = FS::EXTERNAL ;
            }
        }
    }

    if (!Utils::findProgramInPath("btrfstune").isEmpty()) {
        qDebug() << "btrfstune found, enabling write_uuid support";
        fs.write_uuid = FS::EXTERNAL;
    }

    if ( fs .check ) {
        qDebug() << "check support found, enabling copy and move support";
        fs.copy = FS::GPARTED;
        fs.move = FS::GPARTED;
    }

    fs.online_read = FS::EXTERNAL ;
    fs.online_grow = fs.grow ;
    fs.online_shrink = fs.shrink ;

    return fs ;
}

void Btrfs::setUsedSectors(Partition &partition)
{
    qDebug() << "Setting used sectors for partition:" << partition.getPath();
    QString output, error;
    Utils::executCmd(QString("btrfs filesystem show %1").arg(partition.getPath()), output, error);

    Byte_Value ptn_size = partition.getByteLength();
    Byte_Value total_fs_used = -1 ;  //total fs used
    Byte_Value sum_devid_used = 0 ;  //sum devid used
    Byte_Value devid_used = -1 ;     //devid used
    Byte_Value devid_size = -1 ;     //devid size

    //Btrfs file system wide used bytes (extents and items)
    QString str ;
    if (!(str = Utils::regexpLabel(output, "FS bytes used ([0-9\\.]+( ?[KMGTPE]?i?B)?)" ) ).isEmpty()) {
        qDebug() << "Found total fs used string:" << str;
        total_fs_used = qRound64( btrfsSize2Double(str)) ;
    }
    QString::size_type offset = 0 ;
    QString::size_type index ;
    while ((index = output.indexOf("devid ", offset)) != -1)
    {
        // qDebug() << "Found devid at index:" << index;
        QString devid_path = Utils::regexpLabel(output.mid(index),
                                                        "devid .* path (/dev/[[:graph:]]+)" ) ;
        if (!devid_path.isEmpty())
        {
            // qDebug() << "Found devid path:" << devid_path;
            //Btrfs per devid used bytes (chunks)
            Byte_Value used = -1 ;
            if (!(str = Utils::regexpLabel( output.mid( index ),
                                                "devid .* used ([0-9\\.]+( ?[KMGTPE]?i?B)?) path" ) ).isEmpty())
            {
                // qDebug() << "Found used string for devid:" << str;
                used = btrfsSize2num(str, ptn_size, false) ;
                sum_devid_used += used ;
                if ( devid_path.contains(partition.getPath())) {
                    // qDebug() << "devid_path contains partition path, setting devid_used";
                    devid_used = used ;
                }
            }

            if ( devid_path.contains(partition.getPath()))
            {
                // qDebug() << "devid_path contains partition path, getting device size";
                //Btrfs per device size bytes (chunks)
                if (!(str = Utils::regexpLabel( output.mid(index),
                                                    "devid .* size ([0-9\\.]+( ?[KMGTPE]?i?B)?) used " ) ).isEmpty());
                    devid_size = btrfsSize2num( str, ptn_size, true ) ;
            }
        }
        offset = index + 5 ;  //Next find starts immediately after current "devid"
    }

    if ( total_fs_used > -1 && devid_size > -1 && devid_used > -1 && sum_devid_used > 0 )
    {
        qDebug() << "Calculating and setting sector usage";
        auto t = qRound64( devid_size / double(partition.m_sectorSize)) ;               //ptn fs size
        double ptn_fs_used = total_fs_used * ( devid_used / double(sum_devid_used) ) ;  //ptn fs used
        auto n = t - qRound64( ptn_fs_used / double(partition.m_sectorSize) ) ;
        partition.setSectorUsage(t, n);
    }
}

void Btrfs::readLabel(Partition &partition)
{
    qDebug() << "Reading label for partition:" << partition.getPath();
    QString output, error;
    Utils::executCmd(QString("btrfs filesystem show %1").arg(partition.getPath()), output, error);

    if (output.contains("Label: none  uuid:")) {
        qDebug() << "Label is none";
        partition.setFilesystemLabel("");
    } else {
        qDebug() << "Label is not none, extracting label";
        // Try matching a label enclosed in single quotes, then without quotes, to
        // handle reporting of mounted file systems by old btrfs-progs 3.12.
        QString label = Utils::regexpLabel(output, "'(.*)'") ;
        if (!label.isEmpty()) {
            qDebug() << "Label found with single quotes, removing them";
            label = label.replace("'", "");
        }
        partition.setFilesystemLabel(label);
    }


}

bool Btrfs::writeLabel(const Partition &partition)
{
    qDebug() << "Writing label for partition:" << partition.getPath()
            << "new label:" << partition.getFileSystemLabel();
    QString output, error;
    if (partition.m_busy) {
        qDebug() << "Partition is busy, unmounting it";
        Utils::executCmd(QString("umount -v %1").arg(partition.getPath()), output, error);
    }


    auto errCode = Utils::executCmd(QString("btrfs filesystem label %1 %2")
        .arg(partition.getPath()).
        arg(partition.getFileSystemLabel()),
        output, error);

    return errCode == 0;
}

void Btrfs::readUuid(Partition &partition)
{
    qDebug() << "Reading UUID for partition:" << partition.getPath();
    QString output, error;
    auto exitCode = Utils::executCmd(QString("btrfs filesystem show %1").arg(partition.getPath()), output, error);
    //In many cases the exit status doesn't reflect valid output or an error condition
    //  so rely on parsing the output to determine success.

    QString uuid = Utils::regexpLabel(output, "[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}");
    if (!uuid.isEmpty()) {
        qDebug() << "Found UUID:" << uuid;
        partition.m_uuid = uuid;
    }
}

bool Btrfs::writeUuid(const Partition &partition)
{
    qInfo() << "Writing new UUID for partition:" << partition.getPath();
    QString output, error;
    auto exitCode = Utils::executCmd(QString("btrfstune -f -u ").arg(partition.getPath()), output, error);
    return exitCode == 0;
}

bool Btrfs::create(const Partition &new_partition)
{
    qDebug() << "Creating Btrfs filesystem on:" << new_partition.getPath()
            << "with label:" << new_partition.getFileSystemLabel();
    QString output, error;
    QString cmd;
    if (new_partition.getFileSystemLabel().isEmpty() || new_partition.getFileSystemLabel() == " ") {
        qDebug() << "Label is empty, creating without label";
        cmd = QString("mkfs.btrfs -f %1").arg(new_partition.getPath());
    } else {
        qDebug() << "Label is not empty, creating with label";
        cmd = QString("mkfs.btrfs -f -L %1 %2").arg(new_partition.getFileSystemLabel()).arg(new_partition.getPath());
    }
    auto exitCode = Utils::executCmd(cmd, output, error);
    return exitCode == 0 || error.compare("Unknown error") == 0;
}

bool Btrfs::resize(const Partition &partitionNew, bool fillPartition)
{
    qDebug() << "Resizing Btrfs filesystem on:" << partitionNew.getPath()
            << "fill partition:" << fillPartition;
    bool success = true ;
    QString path = partitionNew.getPath();
    const BTRFS_Device& btrfsDev = getCacheEntry(path);

    if (btrfsDev.devid == -1 ) {
        qDebug() << "Invalid devid, returning false";
        return false;
    }
    QString devIdStr = QString("%1").arg(btrfsDev.devid);

    QString mountPoint;
    QString output, error;
    if (!partitionNew.m_busy) {
        qDebug() << "Partition is not busy, mounting to temporary directory";
        mountPoint = Utils::mkTempDir("");
        if (mountPoint.isEmpty()) {
            qDebug() << "Failed to create temporary directory";
            return false ;
        }
        success &= Utils::executCmd(QString("mount -v -t btrfs %1 %2").arg(path).arg(mountPoint), output, error) == 0;
    } else  {
        qDebug() << "Partition is busy, finding existing mount point";
        auto mountPoints = partitionNew.getMountPoints();
        foreach (auto m , mountPoints) {
            // qDebug() << "Checking mount point:" << m;
            QDir dir(m);
            if (dir.exists()) {
                // qDebug() << "Mount point exists, using it:" << m;
                mountPoint = m;
            }
        }
        if (mountPoint.isEmpty()) {
            qDebug() << "No existing mount point found";
            return false;
        }
    }

    if (success) {
        qDebug() << "Mount successful, resizing filesystem";
        QString size ;
        if (!fillPartition) {
            qDebug() << "Not filling partition, calculating size";
            size = QString("%1").arg(qFloor(Utils::sectorToUnit(partitionNew.getSectorLength(), partitionNew.m_sectorSize, UNIT_KIB)));
            size += "K";
        } else {
            qDebug() << "Filling partition, size is max";
            size = "max";
        }
        QString output, error;
        success &= Utils::executCmd(QString("btrfs filesystem resize %1:%2 %3")
                .arg(devIdStr)
                .arg(size)
                .arg(mountPoint), output, error) == 0;

        if (!partitionNew.m_busy) {
            qDebug() << "Unmounting temporary directory";
            success &= Utils::executCmd(QString("umount -v %1").arg(mountPoint), output, error) == 0;
        }
    }

    if (partitionNew.m_busy)
        Utils::rmTempDir(mountPoint) ;

    return success ;
}

bool Btrfs::resize(const QString &path, const QString &size, bool fillPartition)
{
    qDebug() << "Btrfs::resize is not implemented";
    return false;
}

bool Btrfs::checkRepair(const Partition &partition)
{
    qDebug() << "Checking/repairing Btrfs filesystem on:" << partition.getPath();
    QString path = partition.getPath();
    return checkRepair(path);
}

bool Btrfs::checkRepair(const QString &devpath)
{
    QString output, error;
    auto exitCode = Utils::executCmd(QString("btrfs check %1").arg(devpath), output, error);
    return exitCode == 0;

}

const BTRFS_Device &Btrfs::getCacheEntry(const QString &path)
{
    qDebug() << "Btrfs::getCacheEntry BEGIN";
    std::map<BlockSpecial, BTRFS_Device>::const_iterator bd_iter = btrfs_device_cache.find(BlockSpecial(path));
    if ( bd_iter != btrfs_device_cache .end() ) {
        qDebug() << "found in cache";
        return bd_iter ->second ;
    }

    QString output, error ;
    QVector<int> devIdList ;
    QVector<QString> pathList ;
    Utils::executCmd(QString("btrfs filesystem show %1").arg(path), output, error);
    //In many cases the exit status doesn't reflect valid output or an error condition
    //  so rely on parsing the output to determine success.

    //Extract devid and path for each device from output like this:
    //  Label: none  uuid: 36eb51a2-2927-4c92-820f-b2f0b5cdae50
    //          Total devices 2 FS bytes used 156.00KB
    //          devid    2 size 2.00GB used 512.00MB path /dev/sdb2
    //          devid    1 size 2.00GB used 240.75MB path /dev/sdb1
#if QT_VERSION_MAJOR > 5
    auto lines = output.split("\n", Qt::SkipEmptyParts);
    foreach (auto line, lines) {
        auto items = line.split(" ", Qt::SkipEmptyParts);
#else
    auto lines = output.split("\n", QString::SkipEmptyParts);
    foreach (auto line, lines) {
        auto items = line.split(" ", QString::SkipEmptyParts);
#endif
        if (line.trimmed().startsWith("devid") && items.size() == 8) {
            qDebug() << "found devid line";
            devIdList.push_back(items[1].toInt());
            pathList.push_back(items[7]);
        }
    }
    //Add cache entries for all found devices
    std::vector<BlockSpecial> bs_list;
    for (unsigned int i = 0 ; i < pathList.size() ; i ++ ) {
        // qDebug() << "add path to bs_list";
        bs_list.push_back( BlockSpecial(pathList[i] ) );
    }

    for (unsigned int i = 0 ; i < devIdList .size() ; i ++ )
    {
        // qDebug() << "add devid to cache";
        BTRFS_Device btrfs_dev ;
        btrfs_dev.devid = devIdList[i] ;
        btrfs_dev.members = bs_list;
        btrfs_device_cache[ BlockSpecial(pathList[i] ) ] = btrfs_dev;
    }

    bd_iter = btrfs_device_cache.find( BlockSpecial( path ) );
    if ( bd_iter != btrfs_device_cache .end() ) {
        qDebug() << "found in cache again";
        return bd_iter ->second ;
    }

    //If for any reason we fail to parse the information return an "unknown" record
    qDebug() << "fail to parse the information return an unknown record";
    static BTRFS_Device btrfs_dev = { -1, } ;
    return btrfs_dev ;
}

double Btrfs::btrfsSize2Double(QString &str)
{
    qDebug() << "Btrfs::btrfsSize2Double BEGIN";
    //"FS bytes used 51.52GiB"
    QString numStr = Utils::regexpLabel(str, "[1-9]\\d*\\.\\d*|0\\.\\d*[1-9]");
    if (!numStr.isEmpty()) {
        qDebug() << "numStr is not empty";
        double num = numStr.toDouble();
        int index = str.indexOf(numStr);
        int pos = index + numStr.length();
        if (pos < str.length()) {
            qDebug() << "pos < str.length()";
            char unit = str.at(pos).toLatin1();

        }
        char unit = str.at(pos).toLatin1();
        Byte_Value mult;
        switch (unit) {
            case 'K':	mult = KIBIBYTE ;	break ;
            case 'M':	mult = MEBIBYTE ;	break ;
            case 'G':	mult = GIBIBYTE ;	break ;
            case 'T':	mult = TEBIBYTE ;	break ;
            case 'P':	mult = PEBIBYTE ;	break ;
            case 'E':	mult = EXBIBYTE ;	break ;
            default:	mult = 1 ;		break ;
        }
        qDebug() << "Btrfs::btrfsSize2Double END";
        return mult * num;
    }
    qDebug() << "Btrfs::btrfsSize2Double END";
    return 0;
}

Byte_Value Btrfs::btrfsSize2num(QString &str, Byte_Value ptn_bytes, bool scale_up)
{
    qDebug() << "Btrfs::btrfsSize2num BEGIN";
    Byte_Value size_bytes = qRound64( btrfsSize2Double(str )) ;
    double delta         = btrfsSizeMaxDelta( str) ;
    Byte_Value upper_size = size_bytes + ceil(delta) ;
    Byte_Value lower_size = size_bytes - floor(delta) ;

    if ( size_bytes > ptn_bytes && lower_size <= ptn_bytes )
    {
        qDebug() << "size_bytes > ptn_bytes && lower_size <= ptn_bytes";
        size_bytes = ptn_bytes ;
    }
    else if ( scale_up && size_bytes < ptn_bytes && upper_size > ptn_bytes )
    {
        qDebug() << "scale_up && size_bytes < ptn_bytes && upper_size > ptn_bytes";
        size_bytes = ptn_bytes ;
    }

    qDebug() << "Btrfs::btrfsSize2num END";
    return size_bytes ;
}

//Return maximum delta for which num +/- delta would be rounded by btrfs
//  tools to str.  E.g. btrfs_size_max_delta("2.00GB") -> 5368709.12
double Btrfs::btrfsSizeMaxDelta(QString &str)
{
    qDebug() << "Btrfs::btrfsSizeMaxDelta BEGIN";
    QString limit_str ;
    //Create limit_str.  E.g. str = "2.00GB" -> limit_str = "0.005GB"
    for (int i = 0; i < str.length(); ++i) {
        if (str.at(i).isDigit()) {
            limit_str.append("0");
        } else if (str.at(i).toLatin1() == '.') {
            limit_str.append(".");
        } else {
            limit_str .append( "5" );
            limit_str .append(str.mid(i));
            break ;
        }
    }
    double max_delta = btrfsSize2Double(limit_str) ;
    qDebug() << "Btrfs::btrfsSizeMaxDelta END";
    return max_delta ;
}

FS_Limits Btrfs::getFilesystemLimits(const Partition &partition)
{
    qDebug() << "Btrfs::getFilesystemLimits BEGIN";
    //m_fsLimits.min_size = 256 * MEBIBYTE;
    m_fsLimits.min_size = -1;
    m_fsLimits.max_size = -1;
    qDebug() << "Btrfs::getFilesystemLimits END";
    return m_fsLimits;
}

}
