// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "mountinfo.h"
#include "utils.h"
#include "fsinfo.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

#include <limits.h>
#include <mntent.h>

namespace DiskManager {
static MountInfo::MountMapping mountInfo;
static MountInfo::MountMapping fstabInfo;

void MountInfo::loadCache(QString &rootFs)
{
    qDebug() << "MountInfo::loadCache called.";
    mountInfo.clear();
    fstabInfo.clear();

    readMountpointsFromFile("/proc/mounts", mountInfo, rootFs);
    readMountpointsFromFileSwaps("/proc/swaps", mountInfo);

    if (!haveRootfsDev(mountInfo)) {
        qDebug() << "Rootfs device not found in /proc/mounts, falling back to mount command.";
        // Old distributions only contain 'rootfs' and '/dev/root' device names
        // for the / (root) file system in /proc/mounts with '/dev/root' being a
        // block device rather than a symlink to the true device.  This prevents
        // identification, and therefore busy detection, of the device containing
        // the / (root) file system.  Used to read /etc/mtab to get the root file
        // system device name, but this contains an out of date device name after
        // the mounting device has been dynamically removed from a multi-device
        // btrfs, thus identifying the wrong device as busy.  Instead fall back
        // to reading mounted file systems from the output of the mount command,
        // but only when required.
        readMountpointsFromMountCommand(mountInfo);
    }

    readMountpointsFromFile("/etc/fstab", fstabInfo, rootFs);

    // Sort the mount points and remove duplicates ... (no need to do this for fstab_info)
    MountMapping::iterator iterMp;
    for (iterMp = mountInfo.begin(); iterMp != mountInfo.end(); ++iterMp) {
        qDebug() << "Sorting and removing duplicate mountpoints for device:" << iterMp.key().m_name;
        std::sort(iterMp.value().mountpoints.begin(), iterMp.value().mountpoints.end());

        iterMp.value().mountpoints.erase(
            std::unique(iterMp.value().mountpoints.begin(), iterMp.value().mountpoints.end()),
            iterMp.value().mountpoints.end());
    }
}

bool MountInfo::isDevMounted(const QString &path)
{
    qDebug() << "MountInfo::isDevMounted called for path:" << path;
    return isDevMounted(BlockSpecial(path));
}

bool MountInfo::isDevMounted(const BlockSpecial &blockSpecial)
{
    qDebug() << "MountInfo::isDevMounted called for BlockSpecial:" << blockSpecial.m_name;
    MountMapping::const_iterator iterMp = mountInfo.find(blockSpecial);
    bool isMounted = iterMp != mountInfo.end();
    qDebug() << "Device" << blockSpecial.m_name << "is mounted:" << isMounted;
    return isMounted;
}

bool MountInfo::isDevMountedReadonly(const QString &path)
{
    qDebug() << "MountInfo::isDevMountedReadonly called for path:" << path;
    return isDevMountedReadonly(BlockSpecial(path));
}

bool MountInfo::isDevMountedReadonly(const BlockSpecial &blockSpecial)
{
    qDebug() << "MountInfo::isDevMountedReadonly called for BlockSpecial:" << blockSpecial.m_name;
    MountMapping::const_iterator iterMp = mountInfo.find(blockSpecial);
    if (iterMp == mountInfo.end()) {
        qDebug() << "Device not found in mount info.";
        return false;
    }
    qDebug() << "Device" << blockSpecial.m_name << "is readonly:" << iterMp.value().readonly;
    return iterMp.value().readonly;
}

const QVector<QString> &MountInfo::getMountedMountpoints(const QString &path)
{
    qDebug() << "MountInfo::getMountedMountpoints called for path:" << path;
    return find(mountInfo, path).mountpoints;
}

const QVector<QString> &MountInfo::getFileSystemTableMountpoints(const QString &path)
{
    qDebug() << "MountInfo::getFileSystemTableMountpoints called for path:" << path;
    return find(fstabInfo, path).mountpoints;
}

void MountInfo::readMountpointsFromFile(const QString &fileName, MountInfo::MountMapping &map, QString &rootFs)
{
    qDebug() << "MountInfo::readMountpointsFromFile called for file:" << fileName;
    FILE *fp = setmntent(fileName.toStdString().c_str(), "r");
    if (fp == nullptr) {
        qDebug() << "Failed to open" << fileName;
        return;
    }
    struct mntent *p = nullptr;
    while ((p = getmntent(fp)) != nullptr) {
        QString node = p->mnt_fsname;
        QString mountpoint = p->mnt_dir;
        qDebug() << "Processing mount entry: dir=" << p->mnt_dir << ", FsName=" << p->mnt_fsname;
        if (0 == rootFs.length() &&
                (0 == strcmp(p->mnt_dir, "/root")
                 || 0 == strcmp(p->mnt_dir, "/home")
                 || 0 == strcmp(p->mnt_dir, "/opt")
                 || 0 == strcmp(p->mnt_dir, "/var"))) {
            rootFs = p->mnt_fsname;
            qDebug() <<"Set RootFS:" << rootFs;
        }

        QString uuid = Utils::regexpLabel(node, "(?<=UUID\\=).*");
        if (!uuid.isEmpty()) {
            qDebug() << "Found UUID in node, resolving path:" << uuid;
            node = FsInfo::getPathByUuid(uuid);
        }

        QString label = Utils::regexpLabel(node, "(?<=LABEL\\=).*");
        if (!label.isEmpty()) {
            qDebug() << "Found LABEL in node, resolving path:" << label;
            node = FsInfo::getPathByLabel(label);
        }

        if (!node.isEmpty()) {
            qDebug() << "Adding mountpoint entry for node:" << node << "mountpoint:" << mountpoint;
            addMountpointEntry(map, node, mountpoint, parseReadonlyFlag(p->mnt_opts));
        }
    }

    endmntent(fp);
}

void MountInfo::addMountpointEntry(MountInfo::MountMapping &map, QString &node, QString &mountPoint, bool readonly)
{
    qDebug() << "MountInfo::addMountpointEntry called for node:" << node << "mountPoint:" << mountPoint << "readonly:" << readonly;
    // Only add node path if mount point exists
    QFile file(mountPoint);
    if (file.exists()) {
        qDebug() << "Mountpoint" << mountPoint << "exists, adding to map.";
        // Map::operator[] default constructs MountEntry for new keys (nodes).
        MountEntry &mountentry = map[BlockSpecial(node)];
        mountentry.readonly = mountentry.readonly || readonly;
        mountentry.mountpoints.push_back(mountPoint);
    }
}

bool MountInfo::parseReadonlyFlag(const QString &str)
{
    qDebug() << "MountInfo::parseReadonlyFlag called for options string:" << str;
    QStringList mntopts = str.split(",");
    for (int i = 0; i < mntopts.size(); i++) {
        if (mntopts[i] == "rw") {
            qDebug() << "Found 'rw' option, returning false (not readonly).";
            return false;
        }
        else if (mntopts[i] == "ro") {
            qDebug() << "Found 'ro' option, returning true (readonly).";
            return true;
        }
    }
    qDebug() << "No 'ro' or 'rw' option found, defaulting to false (not readonly).";
    return false; // Default is read-write mount
}

void MountInfo::readMountpointsFromFileSwaps(const QString &fileName, MountInfo::MountMapping &map)
{
    qDebug() << "MountInfo::readMountpointsFromFileSwaps called for file:" << fileName;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        QString node;
        while (!in.atEnd() || !line.isEmpty()) {
            // qDebug() << "Processing swap line:" << line;
            node = Utils::regexpLabel(line, "^(/[^ ]+)");

            if (node.size() > 0) {
                // qDebug() << "Found swap device:" << node;
                map[BlockSpecial(node)].mountpoints.push_back("" /* no mountpoint for swap */);
            }
            line = in.readLine();
        }
    }
}

bool MountInfo::haveRootfsDev(MountInfo::MountMapping &)
{
    qDebug() << "MountInfo::haveRootfsDev called.";
    MountMapping::const_iterator iterMp;
    for (iterMp = mountInfo.begin(); iterMp != mountInfo.end(); iterMp++) {
        if (!iterMp.value().mountpoints.isEmpty() && iterMp.value().mountpoints[0] == "/") {
            // qDebug() << "Found root mountpoint for device:" << iterMp.key().m_name;
            if (iterMp.key().m_name != "rootfs" && iterMp.key().m_name != "/dev/root") {
                // qDebug() << "Device is not 'rootfs' or '/dev/root', returning true.";
                return true;
            }
        }
    }
    qDebug() << "No suitable rootfs device found, returning false.";
    return false;
}

void MountInfo::readMountpointsFromMountCommand(MountInfo::MountMapping &map)
{
    qDebug() << "MountInfo::readMountpointsFromMountCommand called.";
    QString output;
    QString error;
    if (!Utils::executCmd("mount", output, error)) {
        QStringList lines;
        lines = output.split("\n");
        for (int i = 0; i < lines.size(); i++) {
            // qDebug() << "Processing mount command output line:" << lines[i];
            // Process line like "/dev/sda3 on / type ext4 (rw)"
            QString node = Utils::regexpLabel(lines[i], ".*?(?= )");
            QString mountpoint = Utils::regexpLabel(lines[i], "(?<=on ).*?(?= type)");
            QString mntopts = Utils::regexpLabel(lines[i], "(?<=\\().*?(?=\\))");
            // qDebug() << node << mountpoint << mntopts;
            if (!node.isEmpty()) {
                addMountpointEntry(map, node, mountpoint, parseReadonlyFlag(mntopts));
            }
        }
    }
}

const MountEntry &MountInfo::find(const MountInfo::MountMapping &map, const QString &path)
{
    // qDebug() << "MountInfo::find called for path:" << path;
    MountMapping::const_iterator iterMp = map.find(BlockSpecial(path));

    if (iterMp != map.end()) {
        // qDebug() << "Found entry for path:" << path;
        return iterMp.value();
    }

    // qDebug() << "No entry found for path:" << path << ", returning static not_mounted.";
    static MountEntry not_mounted = MountEntry();
    return not_mounted;
}

} // namespace DiskManager
