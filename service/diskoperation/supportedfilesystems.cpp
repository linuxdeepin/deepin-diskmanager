#include "supportedfilesystems.h"
#include "utils.h"
#include <QDebug>

namespace DiskManager {

SupportedFileSystems::SupportedFileSystems()
{
    m_fs_objects[FS_UNKNOWN]         = NULL;
    m_fs_objects[FS_OTHER]           = NULL;
//    m_fs_objects[FS_BTRFS]           = new btrfs();
//    m_fs_objects[FS_EXFAT]           = new exfat();
//    m_fs_objects[FS_EXT2]            = new ext2(FS_EXT2);
//    m_fs_objects[FS_EXT3]            = new ext2(FS_EXT3);
//    m_fs_objects[FS_EXT4]            = new ext2(FS_EXT4);
//    m_fs_objects[FS_F2FS]            = new f2fs();
//    m_fs_objects[FS_FAT16]           = new fat16(FS_FAT16);
//    m_fs_objects[FS_FAT32]           = new fat16(FS_FAT32);
//    m_fs_objects[FS_HFS]             = new hfs();
//    m_fs_objects[FS_HFSPLUS]         = new hfsplus();
//    m_fs_objects[FS_JFS]             = new jfs();
//    m_fs_objects[FS_LINUX_SWAP]      = new linux_swap();
//    m_fs_objects[FS_LVM2_PV]         = new lvm2_pv();
//    m_fs_objects[FS_LUKS]            = new luks();
//    m_fs_objects[FS_MINIX]           = new minix();
//    m_fs_objects[FS_NILFS2]          = new nilfs2();
//    m_fs_objects[FS_NTFS]            = new ntfs();
//    m_fs_objects[FS_REISER4]         = new reiser4();
//    m_fs_objects[FS_REISERFS]        = new reiserfs();
//    m_fs_objects[FS_UDF]             = new udf();
//    m_fs_objects[FS_XFS]             = new xfs();
    m_fs_objects[FS_APFS]            = NULL;
    m_fs_objects[FS_ATARAID]         = NULL;
    m_fs_objects[FS_BITLOCKER]       = NULL;
    m_fs_objects[FS_GRUB2_CORE_IMG]  = NULL;
    m_fs_objects[FS_ISO9660]         = NULL;
    m_fs_objects[FS_LINUX_SWRAID]    = NULL;
    m_fs_objects[FS_LINUX_SWSUSPEND] = NULL;
    m_fs_objects[FS_REFS]            = NULL;
    m_fs_objects[FS_UFS]             = NULL;
    m_fs_objects[FS_ZFS]             = NULL;
}

SupportedFileSystems::~SupportedFileSystems()
{
    FSObjectsMap::iterator iter;
    for (iter = m_fs_objects.begin(); iter != m_fs_objects.end(); iter++) {
        auto pvalue = iter.value();
        if (pvalue != NULL)
            delete pvalue;
        iter.value() = NULL;
    }
}

void SupportedFileSystems::find_supported_filesystems()
{
    FSObjectsMap::iterator iter;
    m_fs_support.clear();

    for (iter = m_fs_objects.begin(); iter != m_fs_objects.end(); iter++) {
        if (iter.value()) {
            FileSystem *psys = iter.value();
            m_fs_support.push_back(psys->get_filesystem_support());
        } else {
            FS fs_basicsupp(iter.key());
            fs_basicsupp.move = FS::GPARTED;
            fs_basicsupp.copy = FS::GPARTED;
            m_fs_support.push_back(fs_basicsupp);
        }
    }
}

FileSystem *SupportedFileSystems::get_fs_object(FSType fstype) const
{
    qDebug() << Utils::FSTypeToString(fstype);
    FSObjectsMap::const_iterator iter = m_fs_objects.find(fstype);
    if (iter == m_fs_objects.end())
        return NULL;
    else
        return iter.value();
}

const FS &SupportedFileSystems::get_fs_support(FSType fstype) const
{
    for (unsigned int i = 0; i < m_fs_support.size(); i++) {
        if (m_fs_support[i].fstype == fstype)
            return m_fs_support[i];
    }
    static FS fs_notsupp(FS_UNSUPPORTED);
    return fs_notsupp;
}

const QVector<FS> &SupportedFileSystems::get_all_fs_support() const
{
    return m_fs_support;
}

bool SupportedFileSystems::supported_filesystem(FSType fstype) const
{
    return get_fs_object(fstype) != NULL;
}

}
