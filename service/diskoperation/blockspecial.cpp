/* modified by Uniontech Software Technology Co., Ltd. */
/* Copyright (C) 2016 Mike Fleetwood
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "blockspecial.h"

#include <QMap>
#include <QDebug>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

namespace DiskManager {

struct MmNumber {
    unsigned long m_major;
    unsigned long m_minor;
};

typedef QMap<QString, MmNumber> MMNumberMapping;
// Associative array caching name to major, minor number pairs
// E.g.
//     mm_number_cache["/dev/sda"]  = {8, 0}
//     mm_number_cache["/dev/sda1"] = {8, 1}
//     mm_number_cache["proc"]      = {0, 0}
//     mm_number_cache["sysfs"]     = {0, 0}
static MMNumberMapping mmNumberCache;

BlockSpecial::BlockSpecial()
    : m_name("")
    , m_major(0UL)
    , m_minor(0UL)
{
    qDebug() << "BlockSpecial object created with empty name";
}

BlockSpecial::BlockSpecial(const QString &name)
    : m_name(name)
    , m_major(0UL)
    , m_minor(0UL)
{
    qDebug() << "BlockSpecial object created for device:" << name;
    MMNumberMapping::const_iterator mmNumIter = mmNumberCache.find(name);
    if (mmNumIter != mmNumberCache.end()) {
        qDebug() << "Found device in cache:" << name;
        // Use already cached major, minor pair

        m_major = mmNumIter.value().m_major;
        m_minor = mmNumIter.value().m_minor;
        return;
    }
    qDebug() << "Device not in cache, calling stat:" << name;

    MmNumber pair = {0UL, 0UL};
    // Call stat(name, ...) to get the major, minor pair
    struct stat sb;
    if (stat(name.toStdString().c_str(), &sb) == 0 && S_ISBLK(sb.st_mode)) {
        qDebug() << "stat successful and is a block device:" << name;
        m_major = major(sb.st_rdev);
        m_minor = minor(sb.st_rdev);
        pair.m_major = m_major;
        pair.m_minor = m_minor;
    } else {
        qDebug() << "stat failed or is not a block device:" << name;
    }
    // Add new cache entry for name to major, minor pair
    qDebug() << "Adding new cache entry for" << name << "with major" << pair.m_major << "minor" << pair.m_minor;
    mmNumberCache[name] = pair;
}

BlockSpecial::~BlockSpecial()
{
    // Do not log in destructors of global variables, which can cause crashes
    // qDebug() << "BlockSpecial object destroyed for device:" << m_name;
}

void BlockSpecial::clearCache()
{
    qDebug() << "Clearing block device number cache";
    mmNumberCache.clear();
}

void BlockSpecial::registerBlockSpecial(const QString &name, unsigned long major, unsigned long minor)
{
    qDebug() << "Registering block device:" << name << "with major:" << major << "minor:" << minor;
    MmNumber pair;
    pair.m_major = major;
    pair.m_minor = minor;
    // Add new, or update existing, cache entry for name to major, minor pair
    mmNumberCache[name] = pair;
}

bool operator==(const BlockSpecial &lhs, const BlockSpecial &rhs)
{
    // qDebug() << "Entering operator== for BlockSpecial";
    if (lhs.m_major > 0UL || lhs.m_minor > 0UL) {
        // qDebug() << "Comparing by major/minor number for" << lhs.m_name << "and" << rhs.m_name;
        // Match block special files by major, minor device numbers.
        return lhs.m_major == rhs.m_major && lhs.m_minor == rhs.m_minor;
    }
    else {
        // qDebug() << "Comparing by name for" << lhs.m_name << "and" << rhs.m_name;
        // For non-block special files fall back to name string compare.
        return lhs.m_name == rhs.m_name;
    }
}

bool operator<(const BlockSpecial &lhs, const BlockSpecial &rhs)
{
    // qDebug() << "Entering operator< for BlockSpecial";
    if (lhs.m_major == 0 && rhs.m_major == 0 && lhs.m_minor == 0 && rhs.m_minor == 0) {
        // qDebug() << "Comparing non-block special files by name for" << lhs.m_name << "and" << rhs.m_name;
        // Two non-block special files are ordered by name.
        return lhs.m_name < rhs.m_name;
    }
    else {
        // qDebug() << "Comparing block special files by major/minor number for" << lhs.m_name << "and" << rhs.m_name;
        // Block special files are ordered by major, minor device numbers.
        return lhs.m_major < rhs.m_major || (lhs.m_major == rhs.m_major && lhs.m_minor < rhs.m_minor);
    }
}

} // namespace DiskManager
