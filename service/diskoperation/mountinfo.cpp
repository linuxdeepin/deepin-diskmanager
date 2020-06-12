#include "mountinfo.h"
#include "utils.h"
#include "fsinfo.h"
#include <QStringList>
#include <limits.h>
#include <mntent.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace DiskManager {
static MountInfo::MountMapping mount_info;
static MountInfo::MountMapping fstab_info;

void MountInfo::load_cache()
{
    mount_info.clear();
    fstab_info.clear();

    read_mountpoints_from_file("/proc/mounts", mount_info);
    read_mountpoints_from_file_swaps("/proc/swaps", mount_info);

    if (!have_rootfs_dev(mount_info))
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
        read_mountpoints_from_mount_command(mount_info);

    read_mountpoints_from_file("/etc/fstab", fstab_info);

    // Sort the mount points and remove duplicates ... (no need to do this for fstab_info)
    MountMapping::iterator iter_mp;
    for (iter_mp = mount_info.begin(); iter_mp != mount_info.end(); ++iter_mp) {
        std::sort(iter_mp.value().mountpoints.begin(), iter_mp.value().mountpoints.end());

        iter_mp.value().mountpoints.erase(
            std::unique(iter_mp.value().mountpoints.begin(), iter_mp.value().mountpoints.end()),
            iter_mp.value().mountpoints.end());
    }
}

bool MountInfo::is_dev_mounted(const QString &path)
{
    return is_dev_mounted(BlockSpecial(path));
}

bool MountInfo::is_dev_mounted(const BlockSpecial &bs)
{
    MountMapping::const_iterator iter_mp = mount_info.find(bs);
    return iter_mp != mount_info.end();
}

bool MountInfo::is_dev_mounted_readonly(const QString &path)
{
    return is_dev_mounted_readonly(BlockSpecial(path));
}

bool MountInfo::is_dev_mounted_readonly(const BlockSpecial &bs)
{
    MountMapping::const_iterator iter_mp = mount_info.find(bs);
    if (iter_mp == mount_info.end())
        return false;
    return iter_mp.value().readonly;
}

const QVector<QString> &MountInfo::get_mounted_mountpoints(const QString &path)
{
    return find(mount_info, path).mountpoints;
}

const QVector<QString> &MountInfo::get_fstab_mountpoints(const QString &path)
{
    return find(fstab_info, path).mountpoints;
}

void MountInfo::read_mountpoints_from_file(const QString &filename, MountInfo::MountMapping &map)
{
    FILE *fp = setmntent(filename.toStdString().c_str(), "r");
    if (fp == NULL)
        return;

    struct mntent *p = NULL;
    while ((p = getmntent(fp)) != NULL) {
        QString node = p->mnt_fsname;
        QString mountpoint = p->mnt_dir;

        QString uuid = Utils::regexp_label(node, "(?<=UUID\\=).*");
        if (!uuid.isEmpty())
            node = FsInfo::get_path_by_uuid(uuid);

        QString label = Utils::regexp_label(node, "(?<=UUID\\=).*");
        if (!label.isEmpty())
            node = FsInfo::get_path_by_label(label);

        if (!node.isEmpty())
            add_mountpoint_entry(map, node, mountpoint, parse_readonly_flag(p->mnt_opts));
    }

    endmntent(fp);
}

void MountInfo::add_mountpoint_entry(MountInfo::MountMapping &map, QString &node, QString &mountpoint, bool readonly)
{
    // Only add node path if mount point exists
    QFile file(mountpoint);
    if (file.exists()) {
        // Map::operator[] default constructs MountEntry for new keys (nodes).
        MountEntry &mountentry = map[BlockSpecial(node)];
        mountentry.readonly = mountentry.readonly || readonly;
        mountentry.mountpoints.push_back(mountpoint);
    }
}

bool MountInfo::parse_readonly_flag(const QString &str)
{
    QStringList mntopts = str.split(",");
    for (int i = 0; i < mntopts.size(); i++) {
        if (mntopts[i] == "rw")
            return false;
        else if (mntopts[i] == "ro")
            return true;
    }
    return false; // Default is read-write mount
}

void MountInfo::read_mountpoints_from_file_swaps(const QString &filename, MountInfo::MountMapping &map)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        QString node;
        while (!in.atEnd() || !line.isEmpty()) {
            qDebug() << line;
            node = Utils::regexp_label(line, "^(/[^ ]+)");
            if (node.size() > 0)
                map[BlockSpecial(node)].mountpoints.push_back("" /* no mountpoint for swap */);
            line = in.readLine();
        }
    }
}

bool MountInfo::have_rootfs_dev(MountInfo::MountMapping &)
{
    MountMapping::const_iterator iter_mp;
    for (iter_mp = mount_info.begin(); iter_mp != mount_info.end(); iter_mp++) {
        if (!iter_mp.value().mountpoints.isEmpty() && iter_mp.value().mountpoints[0] == "/") {
            if (iter_mp.key().m_name != "rootfs" && iter_mp.key().m_name != "/dev/root")
                return true;
        }
    }
    return false;
}

void MountInfo::read_mountpoints_from_mount_command(MountInfo::MountMapping &map)
{
    QString output;
    QString error;
    if (!Utils::executcmd("mount", output, error)) {
        QStringList lines;
        lines = output.split("\n");
        for (int i = 0; i < lines.size(); i++) {
            // Process line like "/dev/sda3 on / type ext4 (rw)"
            QString node = Utils::regexp_label(lines[i], ".*?(?= )");
            QString mountpoint = Utils::regexp_label(lines[i], "(?<=on ).*?(?= type)");
            QString mntopts = Utils::regexp_label(lines[i], "(?<=\\().*?(?=\\))");
            // qDebug() << node << mountpoint << mntopts;
            if (!node.isEmpty())
                add_mountpoint_entry(map, node, mountpoint, parse_readonly_flag(mntopts));
        }
    }
}

const MountEntry &MountInfo::find(const MountInfo::MountMapping &map, const QString &path)
{
    MountMapping::const_iterator iter_mp = map.find(BlockSpecial(path));
    if (iter_mp != map.end())
        return iter_mp.value();

    static MountEntry not_mounted = MountEntry();
    return not_mounted;
}

} // namespace DiskManager
