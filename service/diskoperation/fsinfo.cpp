// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

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
    qDebug() << "FsInfo::loadCache called.";
    setCommandsFound();
    loadFileSystemInfoCache();
    m_fsInfoCacheInitialized = true;
}

QString FsInfo::getFileSystemType(const QString &path)
{
    qDebug() << "FsInfo::getFileSystemType called for path:" << path;
    initializeIfRequired();
    const fileSystemEntry &fsEntry = getCacheEntryByPath(path);
    QString fsType = fsEntry.m_type;
    QString fsSecType = fsEntry.m_secType;

    // If vfat, decide whether fat16 or fat32
    if (fsType == "vfat") {
        qDebug() << "fsType is vfat, determining if fat16 or fat32.";
        if (m_needBlkidVfatCacheUpdateWorkaround) {
            qDebug() << "Using blkid vfat cache update workaround.";
            // Blkid cache does not correctly add and remove SEC_TYPE when
            // overwriting FAT16 and FAT32 file systems with each other, so
            // prevents correct identification.  Run blkid command again,
            // bypassing the the cache to get the correct results.
            QString output;
            QString error;
            if (!Utils::executCmd("blkid -c /dev/null " + path, output, error)) {
                fsSecType = Utils::regexpLabel(output, " SEC_TYPE=\"([^\"]*)\"");
                qDebug() << "Bypassed cache, got fsSecType:" << fsSecType;
            }
        }
        if (fsSecType == "msdos") {
            qDebug() << "fsSecType is msdos, setting fsType to fat16.";
            fsType = "fat16";
        }
        else {
            qDebug() << "fsSecType is not msdos, setting fsType to fat32.";
            fsType = "fat32";
        }
    }

    qDebug() << "Returning fsType:" << fsType;
    return fsType;
}

QString FsInfo::getPathByUuid(const QString &uuid)
{
    qDebug() << "FsInfo::getPathByUuid called with uuid:" << uuid;
    initializeIfRequired();
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++) {
        if (uuid == m_fileSystemInfoCache[i].m_uuid) {
            qDebug() << "Found path:" << m_fileSystemInfoCache[i].m_path.m_name << "for uuid:" << uuid;
            return m_fileSystemInfoCache[i].m_path.m_name;
        }
    }

    qDebug() << "No path found for uuid:" << uuid;
    return "";
}

QString FsInfo::getPathByLabel(const QString &label)
{
    qDebug() << "FsInfo::getPathByLabel called with label:" << label;
    initializeIfRequired();
    updateFileSystemInfoCacheAllLabels();
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++) {
        if (label == m_fileSystemInfoCache[i].m_label) {
            qDebug() << "Found path:" << m_fileSystemInfoCache[i].m_path.m_name << "for label:" << label;
            return m_fileSystemInfoCache[i].m_path.m_name;
        }
    }

    qDebug() << "No path found for label:" << label;
    return "";
}

QString FsInfo::getLabel(const QString &path, bool &found)
{
    qDebug() << "FsInfo::getLabel called for path:" << path;
    initializeIfRequired();
    BlockSpecial bs = BlockSpecial(path);
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++) {
        if (bs == m_fileSystemInfoCache[i].m_path) {
            qDebug() << "Found cache entry for path:" << path;
            if (m_fileSystemInfoCache[i].m_haveLabel || m_fileSystemInfoCache[i].m_type == "") {
                // Already have the label or this is a blank cache entry
                // for a whole disk device containing a partition table,
                // so no label (as created by
                // load_fs_info_cache_extra_for_path()).
                found = m_fileSystemInfoCache[i].m_haveLabel;
                qDebug() << "Label already in cache or blank entry. Found:" << found << "Label:" << m_fileSystemInfoCache[i].m_label;
                return m_fileSystemInfoCache[i].m_label;
            }

            // Run blkid to get the label for this one partition, update the
            // cache and return the found label.
            qDebug() << "Label not in cache, running blkid to get it.";
            found = runBlkidUpdateCacheOneLabel(m_fileSystemInfoCache[i]);
            qDebug() << "Found:" << found << "Label:" << m_fileSystemInfoCache[i].m_label;
            return m_fileSystemInfoCache[i].m_label;
        }
    }
    qDebug() << "No cache entry found for path:" << path;
    found = false;
    return "";
}

QString FsInfo::getUuid(const QString &path)
{
    qDebug() << "FsInfo::getUuid called for path:" << path;
    initializeIfRequired();
    const fileSystemEntry &fsEntry = getCacheEntryByPath(path);
    qDebug() << "Returning uuid:" << fsEntry.m_uuid;
    return fsEntry.m_uuid;
}

void FsInfo::initializeIfRequired()
{
    if (!m_fsInfoCacheInitialized) {
        qDebug() << "Cache not initialized, initializing now.";
        setCommandsFound();
        loadFileSystemInfoCache();
        m_fsInfoCacheInitialized = true;
    }
}

void FsInfo::setCommandsFound()
{
    qDebug() << "FsInfo::setCommandsFound called.";
    m_blkidFound = (!Utils::findProgramInPath("blkid").isEmpty());
    qDebug() << "blkid found:" << m_blkidFound;
    if (m_blkidFound) {
        // Blkid from util-linux before 2.23 has a cache update bug which prevents
        // correct identification between FAT16 and FAT32 when overwriting one
        // with the other.  Detect the need for a workaround.
        QString output, error;
        Utils::executCmd("blkid -v", output, error);
        QString blkidVersion = Utils::regexpLabel(output, "blkid.* ([0-9\\.]+) ");
        int blkidMajorVer = 0;
        int blkidMinorVer = 0;
        if (sscanf(blkidVersion.toStdString().c_str(), "%d.%d", &blkidMajorVer, &blkidMinorVer) == 2) {
            m_needBlkidVfatCacheUpdateWorkaround =
                (blkidMajorVer < 2 || (blkidMajorVer == 2 && blkidMinorVer < 23));
            qDebug() << "blkid version:" << blkidVersion << "Need workaround:" << m_needBlkidVfatCacheUpdateWorkaround;
        }
    }
}

void FsInfo::loadFileSystemInfoCache()
{
    qDebug() << "FsInfo::loadFileSystemInfoCache called.";
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
            qDebug() << "Cache entry missing for whole disk device:" << allDevices[i] << ", loading it.";
            // Run "blkid PATH" and load entry into cache for missing entries.
            loadFileSystemInfoCacheExtraForPath(allDevices[i]);
        }
    }
}

bool FsInfo::runBlkidLoadCache(const QString &path)
{
    qDebug() << "FsInfo::runBlkidLoadCache called for path:" << (path.isEmpty() ? "all" : path);
    QString cmd = "blkid";
    if (path.size())
        cmd = cmd + " " + path; //Glib::shell_quote(path);
    QString output;
    QString error;
    bool loadedEntries = false;
    if (m_blkidFound && !Utils::executCmd(cmd, output, error)) {
        qDebug() << "blkid command successful.";
        QStringList strlist = output.split("\n");
        for (int i = 0; i < strlist.size(); i++) {
            fileSystemEntry fsEntry = {BlockSpecial(), "", "", "", false, ""};
            QString entryPath = Utils::regexpLabel(strlist[i], "(?<=/).*?(?=: )");
            if (entryPath.length() > 0) {
                entryPath = "/" + entryPath;
                // qDebug() << "Parsing blkid output for path:" << entryPath;
                fsEntry.m_path = BlockSpecial(entryPath);
                fsEntry.m_type = Utils::regexpLabel(strlist[i], "(?<= TYPE=\").*?(?=\")");
                fsEntry.m_secType = Utils::regexpLabel(strlist[i], " SEC_TYPE=\"([^\"]*)\"");
                fsEntry.m_uuid = Utils::regexpLabel(strlist[i], "(?<=UUID=\").*?(?=\")");
                fsEntry.m_label = Utils::regexpLabel(strlist[i], "(?<=LABEL=\").*?(?=\" )");

                m_fileSystemInfoCache.push_back(fsEntry);
                loadedEntries = true;
//                qDebug() << fsEntry.m_path.m_name << fsEntry.m_type << fsEntry.m_secType << fsEntry.m_uuid << fsEntry.m_label;
            }
        }
    } else {
        qDebug() << "blkid command failed or blkid not found.";
    }
    return loadedEntries;
}

const fileSystemEntry &FsInfo::getCacheEntryByPath(const QString &path)
{
    qDebug() << "FsInfo::getCacheEntryByPath called for path:" << path;
    BlockSpecial bs = BlockSpecial(path);
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++) {
        if (bs == m_fileSystemInfoCache[i].m_path) {
            qDebug() << "Found cache entry for path:" << path;
            return m_fileSystemInfoCache[i];
        }
    }

    qDebug() << "No cache entry found for path:" << path << ", returning static notFound.";
    static fileSystemEntry notFound = {BlockSpecial(), "", "", "", false, ""};
    return notFound;
}

void FsInfo::loadFileSystemInfoCacheExtraForPath(const QString &path)
{
    qDebug() << "FsInfo::loadFileSystemInfoCacheExtraForPath called for path:" << path;
    bool entryAdded = runBlkidLoadCache(path);
    if (!entryAdded) {
        qDebug() << "No entry added by runBlkidLoadCache, adding a blank entry for path:" << path;
        // Ran "blkid PATH" but didn't find details suitable for loading as a
        // cache entry so add a blank entry for PATH name here.
        fileSystemEntry fsEntry = {BlockSpecial(path), "", "", "", false, ""};
        m_fileSystemInfoCache.push_back(fsEntry);
    }
}

void FsInfo::updateFileSystemInfoCacheAllLabels()
{
    qDebug() << "FsInfo::updateFileSystemInfoCacheAllLabels called.";
    if (!m_blkidFound) {
        qDebug() << "blkid not found, returning.";
        return;
    }
    // For all cache entries which are file systems but don't yet have a label load it
    // now.
    for (int i = 0; i < m_fileSystemInfoCache.size(); i++) {
        if (m_fileSystemInfoCache[i].m_type != "" && !m_fileSystemInfoCache[i].m_haveLabel) {
            qDebug() << "Updating label for cache entry:" << m_fileSystemInfoCache[i].m_path.m_name;
            runBlkidUpdateCacheOneLabel(m_fileSystemInfoCache[i]);
        }
    }
}

bool FsInfo::runBlkidUpdateCacheOneLabel(fileSystemEntry &fsEntry)
{
    qDebug() << "FsInfo::runBlkidUpdateCacheOneLabel called for path:" << fsEntry.m_path.m_name;
    if (!m_blkidFound) {
        qDebug() << "blkid not found, returning false.";
        return false;
    }

    // (#786502) Run a separate blkid execution for a single partition to get the
    // label without blkid's default non-reversible encoding.
    QString output;
    QString error;
    bool success = !Utils::executCmd("blkid -o value -s LABEL " + fsEntry.m_path.m_name, output, error);
    if (!success) {
        qDebug() << "blkid command to get label failed.";
        return false;
    }

    size_t len = output.length();
    if (len > 0 && output.back() == '\n') {
        qDebug() << "Stripping trailing newline from blkid output.";
        // Output is either the label with a terminating new line or zero bytes
        // when the file system has no label.  Strip optional trailing new line
        // from blkid output.
        output.resize(len - 1);
    }
    // Update cache entry with the read label.
    fsEntry.m_haveLabel = true;
    fsEntry.m_label = output;
    qDebug() << "Updated label:" << fsEntry.m_label;
    return true;
}

} // namespace DiskManager
