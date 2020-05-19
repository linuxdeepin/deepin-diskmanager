#include "ext2.h"
#include "utils.h"
#include <QDebug>

namespace DiskManager {

EXT2::EXT2(FSType type): specific_type(type), force_auto_64bit(false)
{

}

FS EXT2::get_filesystem_support()
{
    FS fs(specific_type);

    fs .busy = FS::GPARTED ;
    QString output, error;
    mkfs_cmd = "mkfs." + Utils::FSTypeToString(specific_type);
    bool have_64bit_feature = false;
    if (!Utils::find_program_in_path(mkfs_cmd).isEmpty()) {
        fs .create = FS::EXTERNAL ;
        fs .create_with_label = FS::EXTERNAL ;

        // Determine mkfs.ext4 version specific capabilities.
        force_auto_64bit = false;
        if (specific_type == FS_EXT4) {
            Utils::executcmd(QString("%1%2").arg(mkfs_cmd).arg(" -V"), output, error);
            int mke2fs_major_ver = 0;
            int mke2fs_minor_ver = 0;
            int mke2fs_patch_ver = 0;
            if (sscanf(error.toLatin1().data(), "mke2fs %d.%d.%d",
                       &mke2fs_major_ver, &mke2fs_minor_ver, &mke2fs_patch_ver) >= 2) {
                // Ext4 64bit feature was added in e2fsprogs 1.42, but
                // only enable large volumes from 1.42.9 when a large
                // number of 64bit bugs were fixed.
                // *   Release notes, E2fsprogs 1.42 (November 29, 2011)
                //     http://e2fsprogs.sourceforge.net/e2fsprogs-release.html#1.42
                // *   Release notes, E2fsprogs 1.42.9 (December 28, 2013)
                //     http://e2fsprogs.sourceforge.net/e2fsprogs-release.html#1.42.9
                have_64bit_feature = (mke2fs_major_ver > 1)
                                     || (mke2fs_major_ver == 1 && mke2fs_minor_ver > 42)
                                     || (mke2fs_major_ver == 1 && mke2fs_minor_ver == 42 && mke2fs_patch_ver >= 9);

                // (#766910) E2fsprogs 1.43 creates 64bit ext4 file
                // systems by default.  RHEL/CentOS 7 configured e2fsprogs
                // 1.42.9 to create 64bit ext4 file systems by default.
                // Theoretically this can be done when 64bit feature was
                // added in e2fsprogs 1.42.  GParted will re-implement the
                // removed mke2fs.conf(5) auto_64-bit_support option to
                // avoid the issues with multiple boot loaders not working
                // with 64bit ext4 file systems.
                force_auto_64bit = (mke2fs_major_ver > 1)
                                   || (mke2fs_major_ver == 1 && mke2fs_minor_ver >= 42);
            }
        }
    }

    if (! Utils::find_program_in_path("dumpe2fs").isEmpty()) {
        // Resize2fs is preferred, but not required, to determine the minimum FS
        // size.  Can fall back to using dumpe2fs instead.  Dumpe2fs is required
        // for reading FS size and FS block size.  See ext2::set_used_sectors()
        // implementation for details.
        fs.read = FS::EXTERNAL;
        fs.online_read = FS::EXTERNAL;
    }

    if (! Utils::find_program_in_path("tune2fs").isEmpty()) {
        fs.read_uuid = FS::EXTERNAL;
        fs.write_uuid = FS::EXTERNAL;
    }

    if (! Utils::find_program_in_path("e2label").isEmpty()) {
        fs.read_label = FS::EXTERNAL;
        fs.write_label = FS::EXTERNAL;
    }

    if (! Utils::find_program_in_path("e2fsck").isEmpty())
        fs.check = FS::EXTERNAL;

    if (! Utils::find_program_in_path("resize2fs").isEmpty()) {
        fs.grow = FS::EXTERNAL;

        if (fs.read)    // Needed to determine a min file system size..
            fs.shrink = FS::EXTERNAL;
    }

    if (fs.check) {
        fs.copy = fs.move = FS::GPARTED;

        // If supported, use e2image to copy/move the file system as it only
        // copies used blocks, skipping unused blocks.  This is more efficient
        // than copying all blocks used by GParted's internal method.
        if (! Utils::find_program_in_path("e2image").isEmpty()) {
            Utils::executcmd("e2image", output, error);
            if (Utils::regexp_label(error, "(-o src_offset)") == "-o src_offset")
                fs.copy = fs.move = FS::EXTERNAL;
        }
    }
    // Maximum size of an ext2/3/4 volume is 2^32 - 1 blocks, except for ext4 with
    // 64bit feature.  That is just under 16 TiB with a 4K block size.
    // *   Ext4 Disk Layout, Blocks
    //     https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout#Blocks
    // FIXME: Rounding down to whole MiB here should not be necessary.  The Copy, New
    // and Resize/Move dialogs should limit FS correctly without this.  See bug
    // #766910 comment #12 onwards for further discussion.
    //     https://bugzilla.gnome.org/show_bug.cgi?id=766910#c12
    if (specific_type == FS_EXT2                             ||
            specific_type == FS_EXT3                             ||
            (specific_type == FS_EXT4 && ! have_64bit_feature))
        fs_limits.max_size = Utils::floor_size(16 * TEBIBYTE - 4 * KIBIBYTE, MEBIBYTE);

    return fs ;
}

bool EXT2::create(const Partition &new_partition)
{
    QString features, output, error, cmd;
    if (force_auto_64bit) {
        // (#766910) Manually implement mke2fs.conf(5) auto_64-bit_support option
        // by setting or clearing the 64bit feature on the command line depending
        // of the partition size.
        if (new_partition.get_byte_length() >= 16 * TEBIBYTE)
            features = " -O 64bit";
        else
            features = " -O ^64bit";
    }
    cmd = QString("%1%2%3%4%5%6%7").arg(mkfs_cmd).arg(" -F").arg(features).arg(" -L ").
          arg(new_partition.get_filesystem_label()).arg(" ").arg(new_partition.get_path());
    qDebug() << __FUNCTION__ << cmd;
    Utils::executcmd(cmd, output, error);

    return true;

}

}

