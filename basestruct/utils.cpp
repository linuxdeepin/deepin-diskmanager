/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file utils.cpp
 *
 * @brief 公共方法类
 *
 * @date 2020-09-21 14:41
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

#include "utils.h"

#include <sys/statvfs.h>

#include <QProcess>
#include <QRegularExpression>
#include <QUuid>
#include <QDebug>

Utils::Utils()
{
}

QString Utils::findProgramInPath(const QString &proName)
{
    qDebug() << "Utils::findProgramInPath----" << proName;
    if (proName.isEmpty())
        return QString();
    QStringList strArg;
    strArg << proName;
    QString strOut, strerr;
    executeCmdWithArtList("which", strArg, strOut, strerr);
    return strOut;
}

int Utils::executeCmdWithArtList(const QString &strCmd, const QStringList &strArg, QString &outPut, QString &error)
{
    qDebug() << "Utils::executeCmdWithArtList cmd:  " << strCmd;
    qDebug() << "Utils::executeCmdWithArtList argList:  " << strArg;
    QProcess proc;
    // proc.open(QIODevice::ReadWrite);
    //proc.start(strcmd, strarg);
    proc.setProgram(strCmd);
    proc.setArguments(strArg);
    proc.start(QIODevice::ReadWrite);
    proc.waitForFinished(-1);
    outPut = proc.readAllStandardOutput().data();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    proc.close();
    qDebug() << "Utils::executeCmdWithArtList----outPut: " << outPut;
    return exitcode;
}

int Utils::executCmd(const QString &strCmd, QString &outPut, QString &error)
{
    qDebug() << "Utils::executCmd*******--------" << strCmd;
    QStringList cmdList = strCmd.split(" ");
    QStringList argList;
    for (int i = 1; i < cmdList.size(); i++) {
        argList.append(cmdList[i]);
    }
    int exitcode = executeCmdWithArtList(cmdList[0],argList, outPut, error);
    return exitcode;
}

int Utils::executWithPipeCmd(const QString &strCmd, QString &outPut, QString &error)
{
    QStringList argList;
    argList<< "-c" << strCmd;

    return executeCmdWithArtList("/bin/bash", argList,outPut, error);
}

int Utils::executWithErrorCmd(const QString &strCmd, const QStringList &strArg ,QString &outPut, QString &outPutError, QString &error)
{
    qDebug() << "Utils::executWithErrorCmd cmd:  " << strCmd;
    qDebug() << "Utils::executWithErrorCmd argList:  " << strArg;
    QProcess proc;

    proc.setProgram(strCmd);
    proc.setArguments(strArg);
    proc.start(QIODevice::ReadWrite);

    proc.waitForFinished(-1);
    outPut = proc.readAllStandardOutput().data();
    outPutError = proc.readAllStandardError();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    proc.close();
    return exitcode;
}

QString Utils::regexpLabel(const QString &strText, const QString &strPatter)
{
    QString strSource = strText;
    QString result;
    QRegularExpression re(strPatter, QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(strSource);
    if (match.isValid() && match.hasMatch()) {
        for (int i = 0; i <= match.lastCapturedIndex(); i++) {
            result = match.captured(i);
//            qDebug() << __FUNCTION__ << "-------****" << result;
            break;
        }
    }
    return result;
}

const QString Utils::getPartitionTypeString(PartitionType type)
{
    QString strType;
    switch (type) {
    case TYPE_PRIMARY:
        strType = "Primary";
        break;
    case TYPE_LOGICAL:
        strType = "Logical";
        break;
    case TYPE_EXTENDED:
        strType = "Extended";
        break;
    case TYPE_UNALLOCATED:
        strType = "Unallocated";
        break;
    case TYPE_UNPARTITIONED:
        strType = "Unpartitioned";
        break;
    }
    return strType;
}

const QString Utils::fileSystemTypeToString(FSType type)
{
    QString strFileSystemType;
    switch (type) {
    case FS_UNSUPPORTED:
        strFileSystemType = "unsupported";
        break;
    case FS_UNALLOCATED:
        /* TO TRANSLATORS:  unallocated
         * means that this space on the disk device is
         * outside any partition, so is in other words
         * unallocated.
         */
        strFileSystemType = "unallocated";
        break;
    case FS_UNKNOWN:
        /* TO TRANSLATORS:  unknown
         * means that this space within this partition does
         * not contain a file system known to GParted, and
         * is in other words unknown.
         */
        strFileSystemType = "unknown";
        break;
    case FS_UNFORMATTED:
        /* TO TRANSLATORS:  unformatted
         * means that when the new partition is created by
         * GParted the space within it will not be formatted
         * with a file system.
         */
        strFileSystemType = "unformatted";
        break;
    case FS_OTHER:
        /* TO TRANSLATORS:  other
         * name shown in the File System Support dialog to list
         * actions which can be performed on other file systems
         * not specifically listed as supported.
         */
        strFileSystemType = "other";
        break;
    case FS_CLEARED:
        /* TO TRANSLATORS:  cleared
         * means that all file system signatures in the partition
         * will be cleared by GParted.
         */
        strFileSystemType = "cleared";
        break;
    case FS_EXTENDED:
        strFileSystemType = "extended";
        break;
    case FS_BTRFS:
        strFileSystemType = "btrfs";
        break;
    case FS_EXFAT:
        strFileSystemType = "exfat";
        break;
    case FS_EXT2:
        strFileSystemType = "ext2";
        break;
    case FS_EXT3:
        strFileSystemType = "ext3";
        break;
    case FS_EXT4:
        strFileSystemType = "ext4";
        break;
    case FS_F2FS:
        strFileSystemType = "f2fs";
        break;
    case FS_FAT16:
        strFileSystemType = "fat16";
        break;
    case FS_FAT32:
        strFileSystemType = "fat32";
        break;
    case FS_HFS:
        strFileSystemType = "hfs";
        break;
    case FS_HFSPLUS:
        strFileSystemType = "hfs+";
        break;
    case FS_JFS:
        strFileSystemType = "jfs";
        break;
    case FS_LINUX_SWAP:
        strFileSystemType = "linux-swap";
        break;
    case FS_LUKS:
        strFileSystemType = "luks";
        break;
    case FS_LVM2_PV:
        strFileSystemType = "lvm2 pv";
        break;
    case FS_MINIX:
        strFileSystemType = "minix";
        break;
    case FS_NILFS2:
        strFileSystemType = "nilfs2";
        break;
    case FS_NTFS:
        strFileSystemType = "ntfs";
        break;
    case FS_REISER4:
        strFileSystemType = "reiser4";
        break;
    case FS_REISERFS:
        strFileSystemType = "reiserfs";
        break;
    case FS_UDF:
        strFileSystemType = "udf";
        break;
    case FS_XFS:
        strFileSystemType = "xfs";
        break;
    case FS_APFS:
        strFileSystemType = "apfs";
        break;
    case FS_ATARAID:
        strFileSystemType = "ataraid";
        break;
    case FS_BITLOCKER:
        strFileSystemType = "bitlocker";
        break;
    case FS_GRUB2_CORE_IMG:
        strFileSystemType = "grub2 core.img";
        break;
    case FS_ISO9660:
        strFileSystemType = "iso9660";
        break;
    case FS_LINUX_SWRAID:
        strFileSystemType = "linux-raid";
        break;
    case FS_LINUX_SWSUSPEND:
        strFileSystemType = "linux-suspend";
        break;
    case FS_REFS:
        strFileSystemType = "refs";
        break;
    case FS_UFS:
        strFileSystemType = "ufs";
        break;
    case FS_ZFS:
        strFileSystemType = "zfs";
        break;
    case FS_USED:
        strFileSystemType = "used";
        break;
    case FS_UNUSED:
        strFileSystemType = "unused";
        break;
    default:
        strFileSystemType = "";
    }

    return strFileSystemType;
}

FSType Utils::stringToFileSystemType(const QString &fileSystemName)
{
    FSType type = FS_UNKNOWN;
    if (fileSystemName == "extended")
        type = FS_EXTENDED;
    else if (fileSystemName == "btrfs")
        type = FS_BTRFS;
    else if (fileSystemName == "exfat")
        type = FS_EXFAT;
    else if (fileSystemName == "ext2")
        type = FS_EXT2;
    else if (fileSystemName == "ext3")
        type = FS_EXT3;
    else if (fileSystemName == "ext4" || fileSystemName == "ext4dev")
        type = FS_EXT4;
    else if (fileSystemName == "linux-swap(v0)" || fileSystemName == "linux-swap(v1)" || fileSystemName == "swap")
        type = FS_LINUX_SWAP;
    else if (fileSystemName == "crypto_LUKS")
        type = FS_LUKS;
    else if (fileSystemName == "LVM2_member")
        type = FS_LVM2_PV;
    else if (fileSystemName == "f2fs")
        type = FS_F2FS;
    else if (fileSystemName == "fat16")
        type = FS_FAT16;
    else if (fileSystemName == "fat32")
        type = FS_FAT32;
    else if (fileSystemName == "minix")
        type = FS_MINIX;
    else if (fileSystemName == "nilfs2")
        type = FS_NILFS2;
    else if (fileSystemName == "ntfs")
        type = FS_NTFS;
    else if (fileSystemName == "reiserfs")
        type = FS_REISERFS;
    else if (fileSystemName == "xfs")
        type = FS_XFS;
    else if (fileSystemName == "jfs")
        type = FS_JFS;
    else if (fileSystemName == "hfs")
        type = FS_HFS;
    else if (fileSystemName == "hfs+" || fileSystemName == "hfsx" || fileSystemName == "hfsplus")
        type = FS_HFSPLUS;
    else if (fileSystemName == "udf")
        type = FS_UDF;
    else if (fileSystemName == "ufs")
        type = FS_UFS;
    else if (fileSystemName == "apfs")
        type = FS_APFS;
    else if (fileSystemName == "BitLocker")
        type = FS_BITLOCKER;
    else if (fileSystemName == "iso9660")
        type = FS_ISO9660;
    else if (fileSystemName == "linux_raid_member")
        return FS_LINUX_SWRAID;
    else if (fileSystemName == "swsusp" || fileSystemName == "swsuspend")
        type = FS_LINUX_SWSUSPEND;
    else if (fileSystemName == "ReFS")
        type = FS_REFS;
    else if (fileSystemName == "zfs_member")
        type = FS_ZFS;
    else if (fileSystemName == "adaptec_raid_member" || fileSystemName == "ddf_raid_member" || fileSystemName == "hpt45x_raid_member" || fileSystemName == "hpt37x_raid_member" || fileSystemName == "isw_raid_member" || fileSystemName == "jmicron_raid_member" || fileSystemName == "lsi_mega_raid_member" || fileSystemName == "nvidia_raid_member" || fileSystemName == "promise_fasttrack_raid_member" || fileSystemName == "silicon_medley_raid_member" || fileSystemName == "via_raid_member")
        type = FS_ATARAID;

    return type;
}

int Utils::getMountedFileSystemUsage(const QString &mountpoint, Byte_Value &fileSystemSize, Byte_Value &fileSystemFree)
{
    struct statvfs sfs;
    int ret;
    ret = statvfs(mountpoint.toStdString().c_str(), &sfs);
    if (ret == 0) {
        fileSystemSize = static_cast<Byte_Value>(sfs.f_blocks) * sfs.f_frsize;
        fileSystemFree = static_cast<Byte_Value>(sfs.f_bfree) * sfs.f_bsize;
    } else {
        QString errorMessage("statvfs(\"%1\"):%2 "); // = "statvfs(\"" + mountpoint + "\"): " + Glib::strerror(errno) ;
        errorMessage = errorMessage.arg(mountpoint).arg(errno);
//        qDebug() << errorMessage;
    }

    return ret;
}

QString Utils::getFileSystemSoftWare(FSType fileSystemType)
{
    switch (fileSystemType) {
    case FS_BTRFS:
        return "btrfs-progs / btrfs-tools";
    case FS_EXT2:
        return "e2fsprogs";
    case FS_EXT3:
        return "e2fsprogs";
    case FS_EXT4:
        return "e2fsprogs v1.41+";
    case FS_F2FS:
        return "f2fs-tools";
    case FS_FAT16:
        return "dosfstools, mtools";
    case FS_FAT32:
        return "dosfstools, mtools";
    case FS_HFS:
        return "hfsutils";
    case FS_HFSPLUS:
        return "hfsprogs";
    case FS_JFS:
        return "jfsutils";
    case FS_LINUX_SWAP:
        return "util-linux";
    case FS_LVM2_PV:
        return "lvm2";
    case FS_LUKS:
        return "cryptsetup, dmsetup";
    case FS_MINIX:
        return "util-linux";
    case FS_NILFS2:
        return "nilfs-utils / nilfs-tools";
    case FS_NTFS:
        return "ntfs-3g / ntfsprogs";
    case FS_REISER4:
        return "reiser4progs";
    case FS_REISERFS:
        return "reiserfsprogs / reiserfs-utils";
    case FS_UDF:
        return "udftools";
    case FS_XFS:
        return "xfsprogs, xfsdump";

    default:
        return "";
    }
    return "";
}

QString Utils::formatSize(Sector sectors, Byte_Value sectorSize)
{
    QString res;
    if ((sectors * sectorSize) < KIBIBYTE) {
        res = res.setNum(sectorToUnit(sectors, sectorSize, UNIT_BYTE), 'f', 2);
        res.append(" B");
    } else if ((sectors * sectorSize) < MEBIBYTE) {
        res = res.setNum(sectorToUnit(sectors, sectorSize, UNIT_KIB), 'f', 2);
        res.append(" KiB");
    } else if ((sectors * sectorSize) < GIBIBYTE) {
        res = res.setNum(sectorToUnit(sectors, sectorSize, UNIT_MIB), 'f', 2);
        res.append(" MiB");
    } else if ((sectors * sectorSize) < TEBIBYTE) {
        res = res.setNum(sectorToUnit(sectors, sectorSize, UNIT_GIB), 'f', 2);
        res.append(" GiB");
    } else {
        res = res.setNum(sectorToUnit(sectors, sectorSize, UNIT_TIB), 'f', 2);
        res.append(" TiB");
    }
    return res;
}

double Utils::sectorToUnit(Sector sectors, Byte_Value sectorSize, SIZE_UNIT sizeUnit)
{
    double res = 0.0;
    switch (sizeUnit) {
    case UNIT_BYTE:
        res = sectors * sectorSize;
        break;
    case UNIT_KIB:
        res = sectors / (static_cast<double>(KIBIBYTE) / sectorSize);
        break;
    case UNIT_MIB:
        res = sectors / (static_cast<double>(MEBIBYTE) / sectorSize);
        break;
    case UNIT_GIB:
        res = sectors / (static_cast<double>(GIBIBYTE) / sectorSize);
        break;
    case UNIT_TIB:
        res = sectors / (static_cast<double>(TEBIBYTE) / sectorSize);
        break;
    default:
        res = sectors;
    }
    return res;
}

int Utils::getMaxPartitionNameLength(QString &tableType)
{
    if (tableType == "amiga")
        return 0; // Disabled
    else if (tableType == "dvh")
        return 0; // Disabled
    else if (tableType == "gpt")
        return 36;
    else if (tableType == "mac")
        return 0; // Disabled
    else if (tableType == "pc98")
        return 0; // Disabled

    return 0;
}

Byte_Value Utils::floorSize(Byte_Value value, Byte_Value roundingSize)
{
    return value / roundingSize * roundingSize;
}

Byte_Value Utils::ceilSize(Byte_Value value, Byte_Value roundingSize)
{
    return (value + roundingSize - 1LL) / roundingSize * roundingSize;
}

QString Utils::createUuid()
{
    QUuid uuid;
    return uuid.createUuid().toString(QUuid::WithoutBraces);
}
