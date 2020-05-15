#ifndef MOUNTINFO_H
#define MOUNTINFO_H
#include "blockspecial.h"
#include "commondef.h"
#include <QMap>

namespace DiskManager {

class MountInfo
{
public:
    typedef QMap<BlockSpecial, MountEntry> MountMapping;
    static void load_cache();
    static bool is_dev_mounted(const QString &path);
    static bool is_dev_mounted(const BlockSpecial &bs);
    static bool is_dev_mounted_readonly(const QString &path);
    static bool is_dev_mounted_readonly(const BlockSpecial &bs);
    static const QVector<QString> &get_mounted_mountpoints(const QString &path);
    static const QVector<QString> &get_fstab_mountpoints(const QString &path);

private:
    static void read_mountpoints_from_file(const QString &filename, MountMapping &map);
    static void add_mountpoint_entry(MountMapping &map, QString &node, QString &mountpoint, bool readonly);
    static bool parse_readonly_flag(const QString &str);
    static void read_mountpoints_from_file_swaps(const QString &filename, MountMapping &map);
    static bool have_rootfs_dev(MountMapping &map);
    static void read_mountpoints_from_mount_command(MountMapping &map);
    static const MountEntry &find(const MountMapping &map, const QString &path);
};

}
#endif // MOUNTINFO_H
