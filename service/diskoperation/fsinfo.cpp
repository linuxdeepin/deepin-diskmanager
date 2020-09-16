/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file fsinfo.cpp
 *
 * @brief 文件系统信息类
 *
 * @date 2020-09-09 11:04
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

#include "fsinfo.h"
#include "utils.h"
#include "procpartitionsinfo.h"

#include <QStringList>
#include <QDebug>

namespace DiskManager {

bool FsInfo::m_fsInfoCacheInitialized = false;
bool FsInfo::m_blkidFound = false;
bool FsInfo::m_needBlkidVfatCacheUpdateWorkaround = false;
QVector<fileSystemEntry> FsInfo::m_fileSystemInfoCache;

void FsInfo::loadCache()
{
    setCommandsFound();
    loadFileSystemInfoCache();
    m_fsInfoCacheInitialized = true;
}

QString FsInfo::getFileSystemType(const QString &path)
{
    initializeIfRequired();
    const fileSystemEntry &fsEntry = getCacheEntryByPath(path);
    QString fsType = fsEntry.m_type;
    QString fsSecType = fsEntry.m_secType;

    // If vfat, decide whether fat16 or fat32
    if (fsType == "vfat") {
        if (m_needBlkidVfatCacheUpdateWorkaround) {
            // Blkid cache does not correctly add and remove SEC_TYPE when
            // overwriting FAT16 and FAT32 file systems with each other, so
            // prevents correct identification.  Run blkid command again,
            // bypassing the the cache to get the correct results.
            QString output;
            QString error;
            if (!Utils::executcmd("blkid -c /dev/null " + path, output, error))
                fsSecType = Utils::regexp_label(output, " SEC_TYPE=\"([^\"]*)\"");
        }
        if (fsSecType == "msdos")
            fsType = "fat16";
        else
            fsType = "fat32";
    }

    return fsType;
}

QString FsInfo::getPathByUuid(const QString &uuid)
{
    initializeIfRequired();
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++)
        if (uuid == m_fileSystemInfoCache[i].m_uuid)
            return m_fileSystemInfoCache[i].m_path.m_name;

    return "";
}

QString FsInfo::getPathByLabel(const QString &label)
{
    initializeIfRequired();
    updateFileSystemInfoCacheAllLabels();
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++)
        if (label == m_fileSystemInfoCache[i].m_label)
            return m_fileSystemInfoCache[i].m_path.m_name;

    return "";
}

QString FsInfo::getLabel(const QString &path, bool &found)
{
    initializeIfRequired();
    BlockSpecial bs = BlockSpecial(path);
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++)
        if (bs == m_fileSystemInfoCache[i].m_path) {
            if (m_fileSystemInfoCache[i].m_haveLabel || m_fileSystemInfoCache[i].m_type == "") {
                // Already have the label or this is a blank cache entry
                // for a whole disk device containing a partition table,
                // so no label (as created by
                // load_fs_info_cache_extra_for_path()).
                found = m_fileSystemInfoCache[i].m_haveLabel;
                return m_fileSystemInfoCache[i].m_label;
            }

            // Run blkid to get the label for this one partition, update the
            // cache and return the found label.
            found = runBlkidUpdateCacheOneLabel(m_fileSystemInfoCache[i]);
            return m_fileSystemInfoCache[i].m_label;
        }
    found = false;
    return "";
}

QString FsInfo::getUuid(const QString &path)
{
    initializeIfRequired();
    const fileSystemEntry &fsEntry = getCacheEntryByPath(path);
    return fsEntry.m_uuid;
}

void FsInfo::initializeIfRequired()
{
    if (!m_fsInfoCacheInitialized) {
        setCommandsFound();
        loadFileSystemInfoCache();
        m_fsInfoCacheInitialized = true;
    }
}

void FsInfo::setCommandsFound()
{
    m_blkidFound = (!Utils::find_program_in_path("blkid").isEmpty());
    if (m_blkidFound) {
        // Blkid from util-linux before 2.23 has a cache update bug which prevents
        // correct identification between FAT16 and FAT32 when overwriting one
        // with the other.  Detect the need for a workaround.
        QString output, error;
        Utils::executcmd("blkid -v", output, error);
        QString blkidVersion = Utils::regexp_label(output, "blkid.* ([0-9\\.]+) ");
        int blkidMajorVer = 0;
        int blkidMinorVer = 0;
        if (sscanf(blkidVersion.toStdString().c_str(), "%d.%d", &blkidMajorVer, &blkidMinorVer) == 2) {
            m_needBlkidVfatCacheUpdateWorkaround =
                (blkidMajorVer < 2 || (blkidMajorVer == 2 && blkidMinorVer < 23));
        }
    }
}

void FsInfo::loadFileSystemInfoCache()
{
    m_fileSystemInfoCache.clear();
    // Run "blkid" and load entries into the cache.
    runBlkidLoadCache();

    // (#771244) Ensure the cache has entries for all whole disk devices, even if
    // those entries are blank.  Needed so that an ISO9660 image stored on a whole
    // disk device is detected before any embedded partitions within the image.
    const BlockSpecial emptyBs = BlockSpecial();
    QVector<QString> allDevices = ProcPartitionsInfo::getDevicePaths();
    for (int i = 0; i < allDevices.size(); i++) {
        const fileSystemEntry &fsEntry = getCacheEntryByPath(allDevices[i]);
        if (fsEntry.m_path == emptyBs) {
            // Run "blkid PATH" and load entry into cache for missing entries.
            loadFileSystemInfoCacheExtraForPath(allDevices[i]);
        }
    }
}

bool FsInfo::runBlkidLoadCache(const QString &path)
{
    QString cmd = "blkid";
    if (path.size())
        cmd = cmd + " " + path; //Glib::shell_quote(path);
    QString output;
    QString error;
    bool loadedEntries = false;
    if (m_blkidFound && !Utils::executcmd(cmd, output, error)) {
//        qDebug() << output;
        QStringList strlist = output.split("\n");
        for (int i = 0; i < strlist.size(); i++) {
            fileSystemEntry fsEntry = {BlockSpecial(), "", "", "", false, ""};
            QString entryPath = Utils::regexp_label(strlist[i], "(?<=/).*?(?=: )");
            if (entryPath.length() > 0) {
                entryPath = "/" + entryPath;
                fsEntry.m_path = BlockSpecial(entryPath);
                fsEntry.m_type = Utils::regexp_label(strlist[i], "(?<= TYPE=\").*?(?=\")");
                fsEntry.m_secType = Utils::regexp_label(strlist[i], " SEC_TYPE=\"([^\"]*)\"");
                fsEntry.m_uuid = Utils::regexp_label(strlist[i], "(?<=UUID=\").*?(?=\")");
                fsEntry.m_label = Utils::regexp_label(strlist[i], "(?<=LABEL=\").*?(?=\" )");

                m_fileSystemInfoCache.push_back(fsEntry);
                loadedEntries = true;
                qDebug() << fsEntry.m_path.m_name << fsEntry.m_type << fsEntry.m_secType << fsEntry.m_uuid << fsEntry.m_label;
            }
        }
    }
    return loadedEntries;
}

const fileSystemEntry &FsInfo::getCacheEntryByPath(const QString &path)
{
    BlockSpecial bs = BlockSpecial(path);
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++)
        if (bs == m_fileSystemInfoCache[i].m_path)
            return m_fileSystemInfoCache[i];

    static fileSystemEntry notFound = {BlockSpecial(), "", "", "", false, ""};
    return notFound;
}

void FsInfo::loadFileSystemInfoCacheExtraForPath(const QString &path)
{
    bool entryAdded = runBlkidLoadCache(path);
    if (!entryAdded) {
        // Ran "blkid PATH" but didn't find details suitable for loading as a
        // cache entry so add a blank entry for PATH name here.
        fileSystemEntry fsEntry = {BlockSpecial(path), "", "", "", false, ""};
        m_fileSystemInfoCache.push_back(fsEntry);
    }
}

void FsInfo::updateFileSystemInfoCacheAllLabels()
{
    if (!m_blkidFound)
        return;

    // For all cache entries which are file systems but don't yet have a label load it
    // now.
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++)
        if (m_fileSystemInfoCache[i].m_type != "" && !m_fileSystemInfoCache[i].m_haveLabel)
            runBlkidUpdateCacheOneLabel(m_fileSystemInfoCache[i]);
}

bool FsInfo::runBlkidUpdateCacheOneLabel(fileSystemEntry &fsEntry)
{
    if (!m_blkidFound)
        return false;

    // (#786502) Run a separate blkid execution for a single partition to get the
    // label without blkid's default non-reversible encoding.
    QString output;
    QString error;
    bool success = !Utils::executcmd("blkid -o value -s LABEL " + fsEntry.m_path.m_name, output, error);
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
    fsEntry.m_haveLabel = true;
    fsEntry.m_label = output;
    return true;
}

} // namespace DiskManager
