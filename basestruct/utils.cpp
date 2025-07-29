// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "lvmstruct.h"
#include "utils.h"

#include <sys/statvfs.h>

#include <QProcess>
#include <QRegularExpression>
#include <QUuid>
#include <QDebug>
#include <algorithm>
#include <math.h>
#include <QFile>
#include <QDir>

Utils::Utils()
{
    qDebug() << "Utils constructor";
}

static QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

QString Utils::findProgramInPath(const QString &proName)
{
    qDebug() << "Utils::findProgramInPath----" << proName;
    if (proName.isEmpty()) {
        // qDebug() << "Program name is empty";
        return QString();
    }
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
    return exitcode;
}

int Utils::executCmd(const QString &strCmd, QString &outPut, QString &error)
{
    qDebug() << "Utils::executCmd" << strCmd;

    QStringList args = parseCombinedArgString(strCmd);
    if (args.isEmpty()) {
        error = "args is empty";
        return -1;
    }
    const QString prog = args.takeFirst();
    int exitcode = executeCmdWithArtList(prog, args, outPut, error);

    qDebug() << "Utils::executCmd exitcode:  " << exitcode;
    return exitcode;
}

int Utils::executWithInputOutputCmd(const QString &strCmdArg, const QString *inPut, QString &outPut, QString &error)
{
    qDebug() << "Entering Utils::executWithInputOutputCmd with command:" << strCmdArg;
    QProcess proc;
    int exitCode;

    QStringList args = parseCombinedArgString(strCmdArg);
    if (args.isEmpty()) {
        error = "args is empty";
        return -1;
    }
    const QString prog = args.takeFirst();
    proc.start(prog, args);

    if (inPut) {
        qDebug() << "Writing input to process:" << *inPut;
        proc.waitForStarted(-1);
        proc.write(inPut->toLocal8Bit());
        proc.closeWriteChannel();
    }

    proc.waitForFinished(-1);
    outPut = proc.readAllStandardOutput().data();
    error = proc.errorString();
    exitCode = proc.exitCode();
    proc.close();

    qDebug() << "Utils::executWithInputOutputCmd exitCode:  " << exitCode;
    return exitCode;
}

int Utils::executWithErrorCmd(const QString &strCmd, const QStringList &strArg, QString &outPut, QString &outPutError, QString &error)
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
    qDebug() << "Utils::regexpLabel strText:  " << strText << "strPatter:  " << strPatter;
    QString strSource = strText;
    QString result;
    QRegularExpression re(strPatter, QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(strSource);
    if (match.isValid() && match.hasMatch()) {
        qDebug() << "Regexp match found";
        for (int i = 0; i <= match.lastCapturedIndex(); i++) {
            result = match.captured(i);
            qDebug() << "Captured result:" << result;
            break;
        }
    }
    qDebug() << "Utils::regexpLabel result:  " << result;
    return result;
}

const QString Utils::getPartitionTypeString(PartitionType type)
{
    qDebug() << "Entering Utils::getPartitionTypeString for type:" << type;
    QString strType;
    switch (type) {
    case TYPE_PRIMARY:
        qDebug() << "Partition type is Primary";
        strType = "Primary";
        break;
    case TYPE_LOGICAL:
        qDebug() << "Partition type is Logical";
        strType = "Logical";
        break;
    case TYPE_EXTENDED:
        qDebug() << "Partition type is Extended";
        strType = "Extended";
        break;
    case TYPE_UNALLOCATED:
        qDebug() << "Partition type is Unallocated";
        strType = "Unallocated";
        break;
    case TYPE_UNPARTITIONED:
        qDebug() << "Partition type is Unpartitioned";
        strType = "Unpartitioned";
        break;
    }
    return strType;
}

const QString Utils::fileSystemTypeToString(FSType type)
{
    qDebug() << "Entering Utils::fileSystemTypeToString for type:" << type;
    QString strFileSystemType;
    switch (type) {
    case FS_UNSUPPORTED:
        qDebug() << "Filesystem type is Unsupported";
        strFileSystemType = "unsupported";
        break;
    case FS_UNALLOCATED:
        qDebug() << "Filesystem type is Unallocated";
        /* TO TRANSLATORS:  unallocated
         * means that this space on the disk device is
         * outside any partition, so is in other words
         * unallocated.
         */
        strFileSystemType = "unallocated";
        break;
    case FS_UNKNOWN:
        qDebug() << "Filesystem type is Unknown";
        /* TO TRANSLATORS:  unknown
         * means that this space within this partition does
         * not contain a file system known to GParted, and
         * is in other words unknown.
         */
        strFileSystemType = "unknown";
        break;
    case FS_UNFORMATTED:
        qDebug() << "Filesystem type is Unformatted";
        /* TO TRANSLATORS:  unformatted
         * means that when the new partition is created by
         * GParted the space within it will not be formatted
         * with a file system.
         */
        strFileSystemType = "unformatted";
        break;
    case FS_OTHER:
        qDebug() << "Filesystem type is Other";
        /* TO TRANSLATORS:  other
         * name shown in the File System Support dialog to list
         * actions which can be performed on other file systems
         * not specifically listed as supported.
         */
        strFileSystemType = "other";
        break;
    case FS_CLEARED:
        qDebug() << "Filesystem type is Cleared";
        /* TO TRANSLATORS:  cleared
         * means that all file system signatures in the partition
         * will be cleared by GParted.
         */
        strFileSystemType = "cleared";
        break;
    case FS_EXTENDED:
        qDebug() << "Filesystem type is Extended";
        strFileSystemType = "extended";
        break;
    case FS_BTRFS:
        qDebug() << "Filesystem type is BTRFS";
        strFileSystemType = "btrfs";
        break;
    case FS_EXFAT:
        qDebug() << "Filesystem type is exFAT";
        strFileSystemType = "exfat";
        break;
    case FS_EXT2:
        qDebug() << "Filesystem type is ext2";
        strFileSystemType = "ext2";
        break;
    case FS_EXT3:
        qDebug() << "Filesystem type is ext3";
        strFileSystemType = "ext3";
        break;
    case FS_EXT4:
        qDebug() << "Filesystem type is ext4";
        strFileSystemType = "ext4";
        break;
    case FS_F2FS:
        qDebug() << "Filesystem type is f2fs";
        strFileSystemType = "f2fs";
        break;
    case FS_FAT16:
        qDebug() << "Filesystem type is FAT16";
        strFileSystemType = "fat16";
        break;
    case FS_FAT32:
        qDebug() << "Filesystem type is FAT32";
        strFileSystemType = "fat32";
        break;
    case FS_HFS:
        qDebug() << "Filesystem type is HFS";
        strFileSystemType = "hfs";
        break;
    case FS_HFSPLUS:
        qDebug() << "Filesystem type is HFS+";
        strFileSystemType = "hfs+";
        break;
    case FS_JFS:
        qDebug() << "Filesystem type is JFS";
        strFileSystemType = "jfs";
        break;
    case FS_LINUX_SWAP:
        qDebug() << "Filesystem type is Linux Swap";
        strFileSystemType = "linux-swap";
        break;
    case FS_LUKS:
        qDebug() << "Filesystem type is LUKS";
        strFileSystemType = "luks";
        break;
    case FS_LVM2_PV:
        qDebug() << "Filesystem type is LVM2 PV";
        strFileSystemType = "lvm2 pv";
        break;
    case FS_MINIX:
        qDebug() << "Filesystem type is MINIX";
        strFileSystemType = "minix";
        break;
    case FS_NILFS2:
        qDebug() << "Filesystem type is NILFS2";
        strFileSystemType = "nilfs2";
        break;
    case FS_NTFS:
        qDebug() << "Filesystem type is NTFS";
        strFileSystemType = "ntfs";
        break;
    case FS_REISER4:
        qDebug() << "Filesystem type is Reiser4";
        strFileSystemType = "reiser4";
        break;
    case FS_REISERFS:
        qDebug() << "Filesystem type is ReiserFS";
        strFileSystemType = "reiserfs";
        break;
    case FS_UDF:
        qDebug() << "Filesystem type is UDF";
        strFileSystemType = "udf";
        break;
    case FS_XFS:
        qDebug() << "Filesystem type is XFS";
        strFileSystemType = "xfs";
        break;
    case FS_APFS:
        qDebug() << "Filesystem type is APFS";
        strFileSystemType = "apfs";
        break;
    case FS_ATARAID:
        qDebug() << "Filesystem type is ATARAID";
        strFileSystemType = "ataraid";
        break;
    case FS_BITLOCKER:
        qDebug() << "Filesystem type is BITLOCKER";
        strFileSystemType = "bitlocker";
        break;
    case FS_GRUB2_CORE_IMG:
        qDebug() << "Filesystem type is GRUB2_CORE_IMG";
        strFileSystemType = "grub2 core.img";
        break;
    case FS_ISO9660:
        qDebug() << "Filesystem type is ISO9660";
        strFileSystemType = "iso9660";
        break;
    case FS_LINUX_SWRAID:
        qDebug() << "Filesystem type is Linux SWRAID";
        strFileSystemType = "linux-raid";
        break;
    case FS_LINUX_SWSUSPEND:
        qDebug() << "Filesystem type is Linux SWSUSPEND";
        strFileSystemType = "linux-suspend";
        break;
    case FS_REFS:
        qDebug() << "Filesystem type is REFS";
        strFileSystemType = "refs";
        break;
    case FS_UFS:
        qDebug() << "Filesystem type is UFS";
        strFileSystemType = "ufs";
        break;
    case FS_ZFS:
        qDebug() << "Filesystem type is ZFS";
        strFileSystemType = "zfs";
        break;
    case FS_USED:
        qDebug() << "Filesystem type is Used";
        strFileSystemType = "used";
        break;
    case FS_UNUSED:
        qDebug() << "Filesystem type is Unused";
        strFileSystemType = "unused";
        break;
    default:
        qDebug() << "Filesystem type is Unknown";
        strFileSystemType = "";
    }

    qDebug() << "Converted filesystem type:" << strFileSystemType;
    return strFileSystemType;
}

FSType Utils::stringToFileSystemType(const QString &fileSystemName)
{
    qDebug() << "Utils::stringToFileSystemType - Converting:" << fileSystemName;
    qDebug() << "Input filesystem name:" << fileSystemName;
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

    qDebug() << "Converted filesystem type:" << type;
    return type;
}

int Utils::getMountedFileSystemUsage(const QString &mountpoint, Byte_Value &fileSystemSize, Byte_Value &fileSystemFree)
{
    qDebug() << "Utils::getMountedFileSystemUsage - Checking:" << mountpoint;
    struct statvfs sfs;
    int ret;
    ret = statvfs(mountpoint.toStdString().c_str(), &sfs);
    if (ret == 0) {
        fileSystemSize = static_cast<Byte_Value>(sfs.f_blocks) * sfs.f_frsize;
        fileSystemFree = static_cast<Byte_Value>(sfs.f_bfree) * sfs.f_bsize;
    } else {
        qWarning() << "Utils::getMountedFileSystemUsage - Failed for:" << mountpoint << "Error:" << errno;
        QString errorMessage("statvfs(\"%1\"):%2 "); // = "statvfs(\"" + mountpoint + "\"): " + Glib::strerror(errno) ;
        errorMessage = errorMessage.arg(mountpoint).arg(errno);
//        qDebug() << errorMessage;
    }

    return ret;
}

QString Utils::getFileSystemSoftWare(FSType fileSystemType)
{
    qDebug() << "Utils::getFileSystemSoftWare - Checking:" << fileSystemType;
    switch (fileSystemType) {
    case FS_BTRFS:
        return "btrfs.h-progs / btrfs.h-tools";
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
    qDebug() << "Utils::getFileSystemSoftWare - return empty";
    return "";
}

QString Utils::formatSize(Sector sectors, Byte_Value sectorSize)
{
    qDebug() << "Utils::formatSize - Formatting size for sectors:" << sectors << "sectorSize:" << sectorSize;
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
    qDebug() << "Utils::formatSize - Formatted size:" << res;
    return res;
}

double Utils::sectorToUnit(Sector sectors, Byte_Value sectorSize, SIZE_UNIT sizeUnit)
{
    qDebug() << "Utils::sectorToUnit - Converting sectors:" << sectors << "sectorSize:" << sectorSize << "sizeUnit:" << sizeUnit;
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
    qDebug() << "Utils::sectorToUnit - Converted value:" << res;
    return res;
}

int Utils::getMaxPartitionNameLength(QString &tableType)
{
    qDebug() << "Utils::getMaxPartitionNameLength - Checking tableType:" << tableType;
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

    qDebug() << "Utils::getMaxPartitionNameLength - Returning 0";
    return 0;
}

Byte_Value Utils::floorSize(Byte_Value value, Byte_Value roundingSize)
{
    // qInfo() << "Utils::floorSize - value:" << value << "roundingSize:" << roundingSize;
    return value / roundingSize * roundingSize;
}

Byte_Value Utils::ceilSize(Byte_Value value, Byte_Value roundingSize)
{
    // qInfo() << "Utils::ceilSize - value:" << value << "roundingSize:" << roundingSize;
    return (value + roundingSize - 1LL) / roundingSize * roundingSize;
}

QString Utils::createUuid()
{
    qInfo() << "Utils::createUuid - Creating UUID";
    QUuid uuid;
    return uuid.createUuid().toString(QUuid::WithoutBraces);
}


QString Utils::LVMFormatSize(long long lvmSize)
{
    qInfo() << "Utils::LVMFormatSize - Formatting size for lvmSize:" << lvmSize;
    QString res;
    if (lvmSize  < KIBIBYTE) {
        res = res.setNum(LVMSizeToUnit(lvmSize, UNIT_BYTE), 'f', 2);
        res.append(" B");
    } else if (lvmSize < MEBIBYTE) {
        res = res.setNum(LVMSizeToUnit(lvmSize, UNIT_KIB), 'f', 2);
        res.append(" KiB");
    } else if (lvmSize < GIBIBYTE) {
        res = res.setNum(LVMSizeToUnit(lvmSize, UNIT_MIB), 'f', 2);
        res.append(" MiB");
    } else if (lvmSize < TEBIBYTE) {
        res = res.setNum(LVMSizeToUnit(lvmSize, UNIT_GIB), 'f', 2);
        res.append(" GiB");
    } else {
        res = res.setNum(LVMSizeToUnit(lvmSize, UNIT_TIB), 'f', 2);
        res.append(" TiB");
    }
    qInfo() << "Utils::LVMFormatSize - Formatted size:" << res;
    return res;
}

double Utils::LVMSizeToUnit(long long lvmSize, SIZE_UNIT sizeUnit)
{
    qInfo() << "Utils::LVMSizeToUnit - Converting lvmSize:" << lvmSize << "sizeUnit:" << sizeUnit;
    double res = 0.0;
    switch (sizeUnit) {
    case UNIT_BYTE:
        res = lvmSize ;
        break;
    case UNIT_KIB:
        res = lvmSize / static_cast<double>(KIBIBYTE) ;
        break;
    case UNIT_MIB:
        res = lvmSize / static_cast<double>(MEBIBYTE);
        break;
    case UNIT_GIB:
        res = lvmSize / static_cast<double>(GIBIBYTE);
        break;
    case UNIT_TIB:
        res = lvmSize / static_cast<double>(TEBIBYTE);
        break;
    default:
        res = lvmSize;
    }
    qInfo() << "Utils::LVMSizeToUnit - Converted value:" << res;
    return res;
}

bool Utils::adjudicationPVDelete(LVMInfo lvmInfo, const set<QString> &pvStrList, bool &bigDataMove, QStringList &realMovePvList)
{
    qDebug() << "Entering Utils::adjudicationPVDelete";
    bigDataMove = false;
    //获取所有存在于vg中 待删除的pv
    QMap<QString, QVector<QString>>t_map;
    foreach (auto pvPath, pvStrList) {
        if (!lvmInfo.pvExists(pvPath)) { //pv不存在
            return false;
        }

        PVInfo pv =  lvmInfo.getPV(pvPath);
        if (pv.noJoinVG()) {            //只获取加入vg的pv路径
            continue;
        }

        if (!lvmInfo.pvOfVg(pv)) {      //vg不存在或pv不在vg中 说明数据有错误
            qWarning() << "pv not join vg";
            return false;
        }
        t_map[pv.m_vgName].push_back(pv.m_pvPath);
    }

    //判断pv是否可以删除
    long long remvoeAllSize = 0;
    for (QMap<QString, QVector<QString>>::iterator it = t_map.begin(); it != t_map.end(); ++it) {
        long long remvoePE = 0;
        long long unusedPE = 0;
        VGInfo vgInfo = lvmInfo.getVG(it.key());
        foreach (const QString &path, it.value()) { //需要删除的pv
            auto pvIt = vgInfo.m_pvInfo.find(path);
            if (pvIt != vgInfo.m_pvInfo.end()) {
                if (pvIt->m_pvUsedPE > 0) {
                    realMovePvList.push_back(pvIt->m_pvPath);
                    remvoePE += pvIt->m_pvUsedPE;
                }
                unusedPE += pvIt->m_pvUnusedPE;
            }
        }
        if (vgInfo.m_peUnused - unusedPE < remvoePE) { //判断是否允许删除
            qWarning() << "pv used pe more than vg unused pe";
            return false;
        }

        if (!bigDataMove) { //判断是否存在大量数据需要移动
            qInfo() << "Utils::adjudicationPVDelete - Checking if large data move is needed";
            remvoeAllSize += (remvoePE * vgInfo.m_PESize);
            bigDataMove = ((remvoeAllSize / GIBIBYTE) >= 1);
        }
    }
    qInfo() << "Utils::adjudicationPVDelete return true";
    return true;
}

QString Utils::getCipherStr(CRYPT_CIPHER cipher)
{
    qDebug() << "Entering Utils::getCipherStr for cipher:" << cipher;
    switch (cipher) {
    case CRYPT_CIPHER::NOT_CRYPT:
        return QString("not_crypt");
    case CRYPT_CIPHER::AES_XTS_PLAIN64:
        return QString("aes-xts-plain64");
    case CRYPT_CIPHER::SM4_XTS_PLAIN64:
        return QString("sm4-xts-plain64");
    default:
        return QString("crypt_unknow");
    }
}

CRYPT_CIPHER Utils::getCipher(QString cipher)
{
    qDebug() << "Entering Utils::getCipher for cipher:" << cipher;
    if (cipher.contains("not_crypt")) {
        return  CRYPT_CIPHER::NOT_CRYPT;
    } else if (cipher.contains("aes-xts-plain64")) {
        return  CRYPT_CIPHER::AES_XTS_PLAIN64;
    } else if (cipher.contains("sm4-xts-plain64")) {
        return  CRYPT_CIPHER::SM4_XTS_PLAIN64;
    } else {
        return  CRYPT_CIPHER::CRYPT_UNkNOW;
    }

}

bool Utils::kernelSupportFS(const QString &fsType)
{
    qDebug() << "Utils::kernelSupportFS - Checking support for:" << fsType;
    bool supported = false;
    QFile file("/proc/filesystems");
    if (file.open(QIODevice::ReadOnly)) {
        QString info = file.readAll();
        if (info.contains(fsType)) {
            qDebug() << "Utils::kernelSupportFS - Supported:" << fsType;
            supported = true;
        }
        file.close();
    }
    return supported;
}

QString Utils::mkTempDir(const QString &infix)
{
    qDebug() << "Utils::mkTempDir - Creating temp dir with infix:" << infix;
    // Construct template like "/var/tmp/diskmanager-XXXXXX" or "/var/tmp/diskmanager-INFIX-XXXXXX"
    QString dirTemplate = "/var/tmp/";

    if (!infix.isEmpty()) {
        dirTemplate += infix + "-" ;
    }
    dirTemplate += "XXXXXX" ;
    QDir dir(dirTemplate);
    if (!dir.exists() && dir.mkpath(dirTemplate)) {
        qDebug() << "Utils::mkTempDir - Created temp dir:" << dirTemplate;
        return dirTemplate;
    }
    qDebug() << "Utils::mkTempDir - Using existing temp dir:" << dirTemplate;
    return dirTemplate;
}

void Utils::rmTempDir(QString &dirName)
{
    qDebug() << "Utils::rmTempDir - Removing temp dir:" << dirName;
    QDir dir(dirName);
    if (dir.exists()) {
        dir.remove(dirName);
        qDebug() << "Utils::rmTempDir - Removed temp dir:" << dirName;
    } else {
        qDebug() << "Utils::rmTempDir - Temp dir does not exist:" << dirName;
    }
}

QString Utils::readContent(const QString &filename)
{
    qDebug() << "Utils::readContent - Reading file:" << filename;
    QString ret;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Utils::readContent - Successfully read:" << filename;
        ret = file.readAll();
        file.close();
    }
    return ret;
}

int Utils::executCmd(const QString &strCmd)
{
    QString outPut, error;
    return executCmd(strCmd, outPut, error);
}
