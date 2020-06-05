#include "blockspecial.h"
#include <QMap>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

namespace DiskManager {

struct MM_Number {
    unsigned long m_major;
    unsigned long m_minor;
};

typedef QMap<QString, MM_Number> MMNumberMapping;
// Associative array caching name to major, minor number pairs
// E.g.
//     mm_number_cache["/dev/sda"]  = {8, 0}
//     mm_number_cache["/dev/sda1"] = {8, 1}
//     mm_number_cache["proc"]      = {0, 0}
//     mm_number_cache["sysfs"]     = {0, 0}
static MMNumberMapping mm_number_cache;

BlockSpecial::BlockSpecial()
    : m_name("")
    , m_major(0UL)
    , m_minor(0UL)
{
}

BlockSpecial::BlockSpecial(const QString &name)
    : m_name(name)
    , m_major(0UL)
    , m_minor(0UL)
{
    MMNumberMapping::const_iterator mm_num_iter = mm_number_cache.find(name);
    if (mm_num_iter != mm_number_cache.end()) {
        // Use already cached major, minor pair

        m_major = mm_num_iter.value().m_major;
        m_minor = mm_num_iter.value().m_minor;
        return;
    }

    MM_Number pair = {0UL, 0UL};
    // Call stat(name, ...) to get the major, minor pair
    struct stat sb;
    if (stat(name.toStdString().c_str(), &sb) == 0 && S_ISBLK(sb.st_mode)) {
        m_major = major(sb.st_rdev);
        m_minor = minor(sb.st_rdev);
        pair.m_major = m_major;
        pair.m_minor = m_minor;
    }
    // Add new cache entry for name to major, minor pair
    mm_number_cache[name] = pair;
}

BlockSpecial::~BlockSpecial()
{
}

void BlockSpecial::clear_cache()
{
    mm_number_cache.clear();
}

void BlockSpecial::register_block_special(const QString &name, unsigned long major, unsigned long minor)
{
    MM_Number pair;
    pair.m_major = major;
    pair.m_minor = minor;
    // Add new, or update existing, cache entry for name to major, minor pair
    mm_number_cache[name] = pair;
}

bool operator==(const BlockSpecial &lhs, const BlockSpecial &rhs)
{
    if (lhs.m_major > 0UL || lhs.m_minor > 0UL)
        // Match block special files by major, minor device numbers.
        return lhs.m_major == rhs.m_major && lhs.m_minor == rhs.m_minor;
    else
        // For non-block special files fall back to name string compare.
        return lhs.m_name == rhs.m_name;
}

bool operator<(const BlockSpecial &lhs, const BlockSpecial &rhs)
{
    if (lhs.m_major == 0 && rhs.m_major == 0 && lhs.m_minor == 0 && rhs.m_minor == 0)
        // Two non-block special files are ordered by name.
        return lhs.m_name < rhs.m_name;
    else
        // Block special files are ordered by major, minor device numbers.
        return lhs.m_major < rhs.m_major || (lhs.m_major == rhs.m_major && lhs.m_minor < rhs.m_minor);
}

} // namespace DiskManager
