#ifndef FSINFO_H
#define FSINFO_H
#include "blockspecial.h"
#include <QVector>
namespace DiskManager {

struct FS_Entry {
    BlockSpecial path;
    QString type;
    QString sec_type;
    QString uuid;
    bool have_label;
    QString label;
};

class FsInfo
{
public:
    static void load_cache();
    static QString get_fs_type(const QString &path);
    static QString get_path_by_uuid(const QString &uuid);
    static QString get_path_by_label(const QString &label);
    static QString get_label(const QString &path, bool &found);
    static QString get_uuid(const QString &path);

private:
    static void initialize_if_required();
    static void set_commands_found();
    static void load_fs_info_cache();
    static bool run_blkid_load_cache(const QString &path = "");
    static const FS_Entry &get_cache_entry_by_path(const QString &path);
    static void load_fs_info_cache_extra_for_path(const QString &path);
    static void update_fs_info_cache_all_labels();
    static bool run_blkid_update_cache_one_label(FS_Entry &fs_entry);
    static bool fs_info_cache_initialized;
    static bool blkid_found;
    static bool need_blkid_vfat_cache_update_workaround;
    static QVector<FS_Entry> fs_info_cache;
};

} // namespace DiskManager
#endif // FSINFO_H
