#include "fsinfo.h"
#include "utils.h"
#include "procpartitionsinfo.h"
#include <QStringList>
#include <QDebug>

namespace DiskManager {

bool FsInfo::fs_info_cache_initialized = false;
bool FsInfo::blkid_found = false;
bool FsInfo::need_blkid_vfat_cache_update_workaround = false;
QVector<FS_Entry> FsInfo::fs_info_cache;

void FsInfo::load_cache()
{
    set_commands_found();
    load_fs_info_cache();
    fs_info_cache_initialized = true;
}

QString FsInfo::get_fs_type(const QString &path)
{
    initialize_if_required();
    const FS_Entry &fs_entry = get_cache_entry_by_path(path);
    QString fs_type = fs_entry.type;
    QString fs_sec_type = fs_entry.sec_type;

    // If vfat, decide whether fat16 or fat32
    if (fs_type == "vfat") {
        if (need_blkid_vfat_cache_update_workaround) {
            // Blkid cache does not correctly add and remove SEC_TYPE when
            // overwriting FAT16 and FAT32 file systems with each other, so
            // prevents correct identification.  Run blkid command again,
            // bypassing the the cache to get the correct results.
            QString output;
            QString error;
            if (!Utils::executcmd("blkid -c /dev/null " + path, output, error))
                fs_sec_type = Utils::regexp_label(output, " SEC_TYPE=\"([^\"]*)\"");
        }
        if (fs_sec_type == "msdos")
            fs_type = "fat16";
        else
            fs_type = "fat32";
    }

    return fs_type;
}

QString FsInfo::get_path_by_uuid(const QString &uuid)
{
    initialize_if_required();
    for (unsigned int i = 0; i < fs_info_cache.size(); i++)
        if (uuid == fs_info_cache[i].uuid)
            return fs_info_cache[i].path.m_name;

    return "";
}

QString FsInfo::get_path_by_label(const QString &label)
{
    initialize_if_required();
    update_fs_info_cache_all_labels();
    for (unsigned int i = 0; i < fs_info_cache.size(); i++)
        if (label == fs_info_cache[i].label)
            return fs_info_cache[i].path.m_name;

    return "";
}

QString FsInfo::get_label(const QString &path, bool &found)
{
    initialize_if_required();
    BlockSpecial bs = BlockSpecial(path);
    for (unsigned int i = 0; i < fs_info_cache.size(); i++)
        if (bs == fs_info_cache[i].path) {
            if (fs_info_cache[i].have_label || fs_info_cache[i].type == "") {
                // Already have the label or this is a blank cache entry
                // for a whole disk device containing a partition table,
                // so no label (as created by
                // load_fs_info_cache_extra_for_path()).
                found = fs_info_cache[i].have_label;
                return fs_info_cache[i].label;
            }

            // Run blkid to get the label for this one partition, update the
            // cache and return the found label.
            found = run_blkid_update_cache_one_label(fs_info_cache[i]);
            return fs_info_cache[i].label;
        }
    found = false;
    return "";
}

QString FsInfo::get_uuid(const QString &path)
{
    initialize_if_required();
    const FS_Entry &fs_entry = get_cache_entry_by_path(path);
    return fs_entry.uuid;
}

void FsInfo::initialize_if_required()
{
    if (!fs_info_cache_initialized) {
        set_commands_found();
        load_fs_info_cache();
        fs_info_cache_initialized = true;
    }
}

void FsInfo::set_commands_found()
{
    blkid_found = (!Utils::find_program_in_path("blkid").isEmpty());
    if (blkid_found) {
        // Blkid from util-linux before 2.23 has a cache update bug which prevents
        // correct identification between FAT16 and FAT32 when overwriting one
        // with the other.  Detect the need for a workaround.
        QString output, error;
        Utils::executcmd("blkid -v", output, error);
        QString blkid_version = Utils::regexp_label(output, "blkid.* ([0-9\\.]+) ");
        int blkid_major_ver = 0;
        int blkid_minor_ver = 0;
        if (sscanf(blkid_version.toStdString().c_str(), "%d.%d", &blkid_major_ver, &blkid_minor_ver) == 2) {
            need_blkid_vfat_cache_update_workaround =
                (blkid_major_ver < 2 || (blkid_major_ver == 2 && blkid_minor_ver < 23));
        }
    }
}

void FsInfo::load_fs_info_cache()
{
    fs_info_cache.clear();
    // Run "blkid" and load entries into the cache.
    run_blkid_load_cache();

    // (#771244) Ensure the cache has entries for all whole disk devices, even if
    // those entries are blank.  Needed so that an ISO9660 image stored on a whole
    // disk device is detected before any embedded partitions within the image.
    const BlockSpecial empty_bs = BlockSpecial();
    QVector<QString> all_devices = ProcPartitionsInfo::get_device_paths();
    for (unsigned int i = 0; i < all_devices.size(); i++) {
        const FS_Entry &fs_entry = get_cache_entry_by_path(all_devices[i]);
        if (fs_entry.path == empty_bs) {
            // Run "blkid PATH" and load entry into cache for missing entries.
            load_fs_info_cache_extra_for_path(all_devices[i]);
        }
    }
}

bool FsInfo::run_blkid_load_cache(const QString &path)
{
    QString cmd = "blkid";
    if (path.size())
        cmd = cmd + " " + path; //Glib::shell_quote(path);
    QString output;
    QString error;
    bool loaded_entries = false;
    if (blkid_found && !Utils::executcmd(cmd, output, error)) {
        qDebug() << output;
        QStringList strlist = output.split("\n");
        for (int i = 0; i < strlist.size(); i++) {
            FS_Entry fs_entry = {BlockSpecial(), "", "", "", false, ""};
            QString entry_path = Utils::regexp_label(strlist[i], "(?<=/).*?(?=: )");
            if (entry_path.length() > 0) {
                entry_path = "/" + entry_path;
                fs_entry.path = BlockSpecial(entry_path);
                fs_entry.type = Utils::regexp_label(strlist[i], "(?<= TYPE=\").*?(?=\")");
                fs_entry.sec_type = Utils::regexp_label(strlist[i], " SEC_TYPE=\"([^\"]*)\"");
                fs_entry.uuid = Utils::regexp_label(strlist[i], "(?<=UUID=\").*?(?=\")");
                fs_entry.label = Utils::regexp_label(strlist[i], "(?<=LABEL=\").*?(?=\" )");
                ;
                fs_info_cache.push_back(fs_entry);
                loaded_entries = true;
                qDebug() << fs_entry.path.m_name << fs_entry.type << fs_entry.sec_type << fs_entry.uuid << fs_entry.label;
            }
        }
    }
    return loaded_entries;
}

const FS_Entry &FsInfo::get_cache_entry_by_path(const QString &path)
{
    BlockSpecial bs = BlockSpecial(path);
    for (unsigned int i = 0; i < fs_info_cache.size(); i++)
        if (bs == fs_info_cache[i].path)
            return fs_info_cache[i];

    static FS_Entry not_found = {BlockSpecial(), "", "", "", false, ""};
    return not_found;
}

void FsInfo::load_fs_info_cache_extra_for_path(const QString &path)
{
    bool entry_added = run_blkid_load_cache(path);
    if (!entry_added) {
        // Ran "blkid PATH" but didn't find details suitable for loading as a
        // cache entry so add a blank entry for PATH name here.
        FS_Entry fs_entry = {BlockSpecial(path), "", "", "", false, ""};
        fs_info_cache.push_back(fs_entry);
    }
}

void FsInfo::update_fs_info_cache_all_labels()
{
    if (!blkid_found)
        return;

    // For all cache entries which are file systems but don't yet have a label load it
    // now.
    for (unsigned int i = 0; i < fs_info_cache.size(); i++)
        if (fs_info_cache[i].type != "" && !fs_info_cache[i].have_label)
            run_blkid_update_cache_one_label(fs_info_cache[i]);
}

bool FsInfo::run_blkid_update_cache_one_label(FS_Entry &fs_entry)
{
    if (!blkid_found)
        return false;

    // (#786502) Run a separate blkid execution for a single partition to get the
    // label without blkid's default non-reversible encoding.
    QString output;
    QString error;
    bool success = !Utils::executcmd("blkid -o value -s LABEL " + fs_entry.path.m_name, output, error);
    if (!success)
        return false;

    size_t len = output.length();
    if (len > 0 && output.back() == '\n') {
        // Output is either the label with a terminating new line or zero bytes
        // when the file system has no label.  Strip optional trailing new line
        // from blkid output.
        output.resize(len - 1);
    }
    // Update cache entry with the read label.
    fs_entry.have_label = true;
    fs_entry.label = output;
    return true;
}

} // namespace DiskManager
