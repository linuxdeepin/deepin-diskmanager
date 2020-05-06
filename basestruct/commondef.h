#ifndef COMMONDEF_H
#define COMMONDEF_H
#include <QObject>

typedef long long Sector;
typedef long long Byte_Value;

//Size units defined in bytes
const Byte_Value KIBIBYTE = 1024;
const Byte_Value MEBIBYTE = (KIBIBYTE *KIBIBYTE);
const Byte_Value GIBIBYTE = (MEBIBYTE *KIBIBYTE);
const Byte_Value TEBIBYTE = (GIBIBYTE *KIBIBYTE);
const Byte_Value PEBIBYTE = (TEBIBYTE *KIBIBYTE);
const Byte_Value EXBIBYTE = (PEBIBYTE *KIBIBYTE);

enum FSType {
    // Special partition types and functions
    FS_UNSUPPORTED     = 0,  // Type with no supported actions
    FS_UNALLOCATED     = 1,  // Unallocated space on a partitioned drive
    FS_UNKNOWN         = 2,  // Unrecognised content in a drive or partition
    FS_UNFORMATTED     = 3,  // Create a partition without a file system
    FS_CLEARED         = 4,  // Clear existing file system signatures
    FS_OTHER           = 5,  // Just for showing in the File System Support dialog
    FS_EXTENDED        = 6,

    // Fully supported file system types
    FS_BTRFS           = 7,
    FS_EXFAT           = 8, /* Also known as fat64 */
    FS_EXT2            = 9,
    FS_EXT3            = 10,
    FS_EXT4            = 11,
    FS_F2FS            = 12,
    FS_FAT16           = 13,
    FS_FAT32           = 14,
    FS_HFS             = 15,
    FS_HFSPLUS         = 16,
    FS_JFS             = 17,
    FS_LINUX_SWAP      = 18,
    FS_LUKS            = 19,
    FS_LVM2_PV         = 20,
    FS_MINIX           = 21,
    FS_NILFS2          = 22,
    FS_NTFS            = 23,
    FS_REISER4         = 24,
    FS_REISERFS        = 25,
    FS_UDF             = 26,
    FS_XFS             = 27,

    // Other recognised file system types
    FS_APFS            = 28,
    FS_ATARAID         = 29,
    FS_BITLOCKER       = 30,
    FS_GRUB2_CORE_IMG  = 31,
    FS_ISO9660         = 32,
    FS_LINUX_SWRAID    = 33,
    FS_LINUX_SWSUSPEND = 34,
    FS_REFS            = 35,
    FS_UFS             = 36,
    FS_ZFS             = 37,

    // Partition space usage colours
    FS_USED            = 38,
    FS_UNUSED          = 39
} ;

enum SIZE_UNIT {
    UNIT_SECTOR = 0,
    UNIT_BYTE   = 1,

    UNIT_KIB    = 2,
    UNIT_MIB    = 3,
    UNIT_GIB    = 4,
    UNIT_TIB    = 5
} ;

#endif // COMMONDEF_H
