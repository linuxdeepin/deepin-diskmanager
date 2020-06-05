#ifndef SUPPORTEDFILESYSTEMS_H
#define SUPPORTEDFILESYSTEMS_H
#include "filesystems/filesystem.h"
#include "commondef.h"
#include <QVector>
#include <QMap>

namespace DiskManager {

class SupportedFileSystems
{
public:
    SupportedFileSystems();
    ~SupportedFileSystems();
    void find_supported_filesystems();
    FileSystem *get_fs_object(FSType fstype) const;
    const FS &get_fs_support(FSType fstype) const;
    const QVector<FS> &get_all_fs_support() const;
    bool supported_filesystem(FSType fstype) const;
    const QStringList &get_all_fsname();

private:
    typedef QMap<FSType, FileSystem *> FSObjectsMap;

    QVector<FS> m_fs_support;
    FSObjectsMap m_fs_objects;
    QStringList m_effectivefs;
};

} // namespace DiskManager
#endif // SUPPORTEDFILESYSTEMS_H
