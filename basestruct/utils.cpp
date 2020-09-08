#include "utils.h"
#include <sys/statvfs.h>
#include <QProcess>
#include <QRegularExpression>
#include <QUuid>
#include <QDebug>

Utils::Utils()
{
}

QString Utils::find_program_in_path(const QString &proname)
{
    qDebug() << "Utils::find_program_in_path----" << proname;
    if (proname.isEmpty())
        return QString();
    QStringList strarg;
    strarg << proname;
    QString strout, strerr;
    executecmdwithartlist("which", strarg, strout, strerr);
    return strout;
}

int Utils::executecmdwithartlist(const QString &strcmd, const QStringList &strarg, QString &output, QString &error)
{
    //qDebug() << "Utils::executecmdwithartlist----+=" << strcmd;
    QProcess proc;
    // proc.open(QIODevice::ReadWrite);
    //proc.start(strcmd, strarg);
    proc.setProgram(strcmd);
    proc.setArguments(strarg);
    proc.start(QIODevice::ReadWrite);

    proc.waitForFinished(-1);
    output = proc.readAllStandardOutput().data();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    //qDebug() << "Utils::executecmdwithartlist----" << output << error;
    proc.close();
    return exitcode;
}

int Utils::executcmd(const QString &strcmd, QString &output, QString &error)
{
    qDebug() << "Utils::executcmd*******--------" << strcmd;
    QProcess proc;
    // proc.open(QIODevice::ReadWrite);
    proc.start(strcmd);
    proc.waitForFinished(-1);
    output = proc.readAllStandardOutput();
    QString stderror = proc.readAllStandardError();
    error = proc.errorString();
    int exitcode = proc.exitCode();
    // qDebug()<<output<<error<<stderror;
    //mkfs.ext4 -V have no output,stderror include useful info
    if (output.isEmpty() && !stderror.isEmpty()) {
        output = stderror;
    }
    proc.close();
    return exitcode;
}

QString Utils::regexp_label(const QString &strtext, const QString &strpatter)
{
    QString strsource = strtext;
    QString result;
    QRegularExpression re(strpatter, QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(strsource);
    if (match.isValid() && match.hasMatch()) {
        for (int i = 0; i <= match.lastCapturedIndex(); i++) {
            result = match.captured(i);
//            qDebug() << __FUNCTION__ << "-------****" << result;
            break;
        }
    }
    return result;
}

const QString Utils::get_partition_type_string(PartitionType type)
{
    QString strtype;
    switch (type) {
    case TYPE_PRIMARY:
        strtype = "Primary";
        break;
    case TYPE_LOGICAL:
        strtype = "Logical";
        break;
    case TYPE_EXTENDED:
        strtype = "Extended";
        break;
    case TYPE_UNALLOCATED:
        strtype = "Unallocated";
        break;
    case TYPE_UNPARTITIONED:
        strtype = "Unpartitioned";
        break;
    }
    return strtype;
}

const QString Utils::FSTypeToString(FSType fstype)
{
    QString strfstype;
    switch (fstype) {
    case FS_UNSUPPORTED:
        strfstype = "unsupported";
        break;
    case FS_UNALLOCATED:
        /* TO TRANSLATORS:  unallocated
         * means that this space on the disk device is
         * outside any partition, so is in other words
         * unallocated.
         */
        strfstype = "unallocated";
        break;
    case FS_UNKNOWN:
        /* TO TRANSLATORS:  unknown
         * means that this space within this partition does
         * not contain a file system known to GParted, and
         * is in other words unknown.
         */
        strfstype = "unknown";
        break;
    case FS_UNFORMATTED:
        /* TO TRANSLATORS:  unformatted
         * means that when the new partition is created by
         * GParted the space within it will not be formatted
         * with a file system.
         */
        strfstype = "unformatted";
        break;
    case FS_OTHER:
        /* TO TRANSLATORS:  other
         * name shown in the File System Support dialog to list
         * actions which can be performed on other file systems
         * not specifically listed as supported.
         */
        strfstype = "other";
        break;
    case FS_CLEARED:
        /* TO TRANSLATORS:  cleared
         * means that all file system signatures in the partition
         * will be cleared by GParted.
         */
        strfstype = "cleared";
        break;
    case FS_EXTENDED:
        strfstype = "extended";
        break;
    case FS_BTRFS:
        strfstype = "btrfs";
        break;
    case FS_EXFAT:
        strfstype = "exfat";
        break;
    case FS_EXT2:
        strfstype = "ext2";
        break;
    case FS_EXT3:
        strfstype = "ext3";
        break;
    case FS_EXT4:
        strfstype = "ext4";
        break;
    case FS_F2FS:
        strfstype = "f2fs";
        break;
    case FS_FAT16:
        strfstype = "fat16";
        break;
    case FS_FAT32:
        strfstype = "fat32";
        break;
    case FS_HFS:
        strfstype = "hfs";
        break;
    case FS_HFSPLUS:
        strfstype = "hfs+";
        break;
    case FS_JFS:
        strfstype = "jfs";
        break;
    case FS_LINUX_SWAP:
        strfstype = "linux-swap";
        break;
    case FS_LUKS:
        strfstype = "luks";
        break;
    case FS_LVM2_PV:
        strfstype = "lvm2 pv";
        break;
    case FS_MINIX:
        strfstype = "minix";
        break;
    case FS_NILFS2:
        strfstype = "nilfs2";
        break;
    case FS_NTFS:
        strfstype = "ntfs";
        break;
    case FS_REISER4:
        strfstype = "reiser4";
        break;
    case FS_REISERFS:
        strfstype = "reiserfs";
        break;
    case FS_UDF:
        strfstype = "udf";
        break;
    case FS_XFS:
        strfstype = "xfs";
        break;
    case FS_APFS:
        strfstype = "apfs";
        break;
    case FS_ATARAID:
        strfstype = "ataraid";
        break;
    case FS_BITLOCKER:
        strfstype = "bitlocker";
        break;
    case FS_GRUB2_CORE_IMG:
        strfstype = "grub2 core.img";
        break;
    case FS_ISO9660:
        strfstype = "iso9660";
        break;
    case FS_LINUX_SWRAID:
        strfstype = "linux-raid";
        break;
    case FS_LINUX_SWSUSPEND:
        strfstype = "linux-suspend";
        break;
    case FS_REFS:
        strfstype = "refs";
        break;
    case FS_UFS:
        strfstype = "ufs";
        break;
    case FS_ZFS:
        strfstype = "zfs";
        break;
    case FS_USED:
        strfstype = "used";
        break;
    case FS_UNUSED:
        strfstype = "unused";
        break;
    default:
        strfstype = "";
    }

    return strfstype;
}

FSType Utils::StringToFSType(const QString &fsname)
{
    FSType type = FS_UNKNOWN;
    if (fsname == "extended")
        type = FS_EXTENDED;
    else if (fsname == "btrfs")
        type = FS_BTRFS;
    else if (fsname == "exfat")
        type = FS_EXFAT;
    else if (fsname == "ext2")
        type = FS_EXT2;
    else if (fsname == "ext3")
        type = FS_EXT3;
    else if (fsname == "ext4" || fsname == "ext4dev")
        type = FS_EXT4;
    else if (fsname == "linux-swap(v0)" || fsname == "linux-swap(v1)" || fsname == "swap")
        type = FS_LINUX_SWAP;
    else if (fsname == "crypto_LUKS")
        type = FS_LUKS;
    else if (fsname == "LVM2_member")
        type = FS_LVM2_PV;
    else if (fsname == "f2fs")
        type = FS_F2FS;
    else if (fsname == "fat16")
        type = FS_FAT16;
    else if (fsname == "fat32")
        type = FS_FAT32;
    else if (fsname == "minix")
        type = FS_MINIX;
    else if (fsname == "nilfs2")
        type = FS_NILFS2;
    else if (fsname == "ntfs")
        type = FS_NTFS;
    else if (fsname == "reiserfs")
        type = FS_REISERFS;
    else if (fsname == "xfs")
        type = FS_XFS;
    else if (fsname == "jfs")
        type = FS_JFS;
    else if (fsname == "hfs")
        type = FS_HFS;
    else if (fsname == "hfs+" || fsname == "hfsx" || fsname == "hfsplus")
        type = FS_HFSPLUS;
    else if (fsname == "udf")
        type = FS_UDF;
    else if (fsname == "ufs")
        type = FS_UFS;
    else if (fsname == "apfs")
        type = FS_APFS;
    else if (fsname == "BitLocker")
        type = FS_BITLOCKER;
    else if (fsname == "iso9660")
        type = FS_ISO9660;
    else if (fsname == "linux_raid_member")
        return FS_LINUX_SWRAID;
    else if (fsname == "swsusp" || fsname == "swsuspend")
        type = FS_LINUX_SWSUSPEND;
    else if (fsname == "ReFS")
        type = FS_REFS;
    else if (fsname == "zfs_member")
        type = FS_ZFS;
    else if (fsname == "adaptec_raid_member" || fsname == "ddf_raid_member" || fsname == "hpt45x_raid_member" || fsname == "hpt37x_raid_member" || fsname == "isw_raid_member" || fsname == "jmicron_raid_member" || fsname == "lsi_mega_raid_member" || fsname == "nvidia_raid_member" || fsname == "promise_fasttrack_raid_member" || fsname == "silicon_medley_raid_member" || fsname == "via_raid_member")
        type = FS_ATARAID;

    return type;
}

int Utils::get_mounted_filesystem_usage(const QString &mountpoint, Byte_Value &fs_size, Byte_Value &fs_free)
{
    struct statvfs sfs;
    int ret;
    ret = statvfs(mountpoint.toStdString().c_str(), &sfs);
    if (ret == 0) {
        fs_size = static_cast<Byte_Value>(sfs.f_blocks) * sfs.f_frsize;
        fs_free = static_cast<Byte_Value>(sfs.f_bfree) * sfs.f_bsize;
    } else {
        QString error_message("statvfs(\"%1\"):%2 "); // = "statvfs(\"" + mountpoint + "\"): " + Glib::strerror(errno) ;
        error_message = error_message.arg(mountpoint).arg(errno);
        qDebug() << error_message;
    }

    return ret;
}

QString Utils::get_filesystem_software(FSType fstype)
{
    switch (fstype) {
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

QString Utils::format_size(Sector sectors, Byte_Value sector_size)
{
    QString res;
    if ((sectors * sector_size) < KIBIBYTE) {
        res = res.setNum(sector_to_unit(sectors, sector_size, UNIT_BYTE), 'f', 2);
        res.append(" B");
    } else if ((sectors * sector_size) < MEBIBYTE) {
        res = res.setNum(sector_to_unit(sectors, sector_size, UNIT_KIB), 'f', 2);
        res.append(" KiB");
    } else if ((sectors * sector_size) < GIBIBYTE) {
        res = res.setNum(sector_to_unit(sectors, sector_size, UNIT_MIB), 'f', 2);
        res.append(" MiB");
    } else if ((sectors * sector_size) < TEBIBYTE) {
        res = res.setNum(sector_to_unit(sectors, sector_size, UNIT_GIB), 'f', 2);
        res.append(" GiB");
    } else {
        res = res.setNum(sector_to_unit(sectors, sector_size, UNIT_TIB), 'f', 2);
        res.append(" TiB");
    }
    return res;
}

double Utils::sector_to_unit(Sector sectors, Byte_Value sector_size, SIZE_UNIT size_unit)
{
    double res = 0.0;
    switch (size_unit) {
    case UNIT_BYTE:
        res = sectors * sector_size;
        break;
    case UNIT_KIB:
        res = sectors / (static_cast<double>(KIBIBYTE) / sector_size);
        break;
    case UNIT_MIB:
        res = sectors / (static_cast<double>(MEBIBYTE) / sector_size);
        break;
    case UNIT_GIB:
        res = sectors / (static_cast<double>(GIBIBYTE) / sector_size);
        break;
    case UNIT_TIB:
        res = sectors / (static_cast<double>(TEBIBYTE) / sector_size);
        break;
    default:
        res = sectors;
    }
    return res;
}

int Utils::get_max_partition_name_length(QString &tabletype)
{
    if (tabletype == "amiga")
        return 0; // Disabled
    else if (tabletype == "dvh")
        return 0; // Disabled
    else if (tabletype == "gpt")
        return 36;
    else if (tabletype == "mac")
        return 0; // Disabled
    else if (tabletype == "pc98")
        return 0; // Disabled

    return 0;
}

Byte_Value Utils::floor_size(Byte_Value value, Byte_Value rounding_size)
{
    return value / rounding_size * rounding_size;
}

Byte_Value Utils::ceil_size(Byte_Value value, Byte_Value rounding_size)
{
    return (value + rounding_size - 1LL) / rounding_size * rounding_size;
}

QString Utils::CreateUUid()
{
    QUuid uuid;
    return uuid.createUuid().toString(QUuid::WithoutBraces);
}
