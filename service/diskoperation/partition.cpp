#include "partition.h"
#include "utils.h"
#include <QDebug>

namespace DiskManager {

Partition::Partition()
{
    Reset();
}

Partition *Partition::clone() const
{
    return new Partition(*this);
}

bool Partition::sector_usage_known() const
{
    return sectors_used >= 0 && sectors_unused >= 0 ;
}

Sector Partition::get_sectors_unallocated() const
{
    if (sectors_unallocated < significant_threshold)
        return 0 ;
    else
        return sectors_unallocated ;
}

void Partition::Reset()
{
    path.clear();
    status = STAT_REAL;
    type = TYPE_UNALLOCATED;
    alignment = ALIGN_STRICT ;
    fstype = FS_UNALLOCATED;
    have_filesystem_label = false;
    uuid .clear() ;
    name.clear();
    partition_number = sector_start = sector_end = sectors_used = sectors_unused = -1;
    sectors_unallocated = 0 ;
    significant_threshold = 1 ;
    free_space_before = -1 ;
    sector_size = 0 ;
    fs_block_size = -1;
    inside_extended = busy = strict_start = false ;
    fs_readonly = false;
    logicals .clear() ;
    flags .clear() ;
    mountpoints .clear() ;
}

void Partition::Set(const QString &device_path, const QString &partition, int partition_number, PartitionType type, FSType fstype, Sector sector_start, Sector sector_end, Byte_Value sector_size, bool inside_extended, bool busy)
{
    this ->device_path = device_path ;
    this->path = partition;
    this ->partition_number = partition_number;
    this ->type = type;
    this->fstype = fstype;
    this ->sector_start = sector_start;
    this ->sector_end = sector_end;
    this ->sector_size = sector_size;
    this ->inside_extended = inside_extended;
    this ->busy = busy;
}

void Partition::set_unpartitioned(const QString &device_path, const QString &partition_path, FSType fstype, Sector length, Byte_Value sector_size, bool busy)
{
    Reset();
    // The path from the parent Device object and this child Partition object
    // spanning the whole device would appear to be the same.  However the former
    // may have come from the command line and the later is the canonicalised
    // name returned from libparted.  (See GParted_Core::set_device_from_disk()
    // "loop" table and GParted_Core::set_device_one_partition() calls to
    // set_unpartitioned()).  Therefore they can be different.
    //
    // For unrecognised whole disk device partitions use "unallocated" as the
    // partition path to match what Set_Unallocated() does when it created such
    // whole disk device partitions.
    Set(device_path,
        (fstype == FS_UNALLOCATED) ? Utils::FSTypeToString(FS_UNALLOCATED) : partition_path,
        1,
        TYPE_UNPARTITIONED,
        fstype,
        0LL,
        length - 1LL,
        sector_size,
        false,
        busy);
}

void Partition::Set_Unallocated(const QString &device_path, Sector sector_start, Sector sector_end, Byte_Value sector_size, bool inside_extended)
{
    Reset() ;

    Set(device_path,
        Utils::FSTypeToString(FS_UNALLOCATED),
        -1,
        TYPE_UNALLOCATED,
        FS_UNALLOCATED,
        sector_start,
        sector_end,
        sector_size,
        inside_extended,
        false);

    status = STAT_REAL;
}

bool Partition::filesystem_label_known() const
{
    return have_filesystem_label;
}

void Partition::set_filesystem_label(const QString &filesystem_label)
{
    if (!filesystem_label.isEmpty()) {
        this->filesystem_label = filesystem_label;
        have_filesystem_label = true;
    }
}

void Partition::add_mountpoint(const QString &mountpoint)
{
    this->mountpoints .push_back(mountpoint) ;
}

void Partition::add_mountpoints(const QVector<QString> &mountpoints)
{
    this->mountpoints = mountpoints;
}

QVector<QString> Partition::get_mountpoints() const
{
    return mountpoints;
}

QString Partition::get_mountpoint() const
{
    if (mountpoints .size() > 0)
        return mountpoints .front() ;

    return "" ;
}

void Partition::set_sector_usage(Sector sectors_fs_size, Sector sectors_fs_unused)
{
    Sector length = get_sector_length() ;
    if (0 <= sectors_fs_size   && sectors_fs_size   <= length
            && 0 <= sectors_fs_unused && sectors_fs_unused <= sectors_fs_size
       ) {
        sectors_used          = sectors_fs_size - sectors_fs_unused ;
        sectors_unused        = sectors_fs_unused ;
        sectors_unallocated   = length - sectors_fs_size ;
        significant_threshold = calc_significant_unallocated_sectors() ;
    } else if (sectors_fs_size == -1) {
        if (0 <= sectors_fs_unused && sectors_fs_unused <= length) {
            sectors_used   = length - sectors_fs_unused ;
            sectors_unused = sectors_fs_unused ;
        } else {
            sectors_used = -1 ;
            sectors_unused = -1 ;
        }
        sectors_unallocated   = 0 ;
        significant_threshold = 1 ;
    }
}

QString Partition::get_path() const
{
    return path;
}

Byte_Value Partition::get_byte_length() const
{
    if (get_sector_length() >= 0)
        return get_sector_length() * sector_size ;
    else
        return -1 ;
}

Sector Partition::get_sector_length() const
{
    if (sector_start >= 0 && sector_end >= 0)
        return sector_end - sector_start + 1 ;
    else
        return -1 ;
}

QString Partition::get_filesystem_label() const
{
    return filesystem_label;
}

PartitionInfo Partition::getPartitionInfo()
{
    PartitionInfo info;
    info.device_path = device_path;
    info.partition_number = partition_number;
    info.type = type;
    info.status = status;
    info.alignment = alignment;
    info.fstype = fstype;
    info.uuid = uuid;
    info.name = name;
    info.sector_start = sector_start;
    info.sector_end = sector_end;
    info.sectors_used = sectors_used;
    info.sectors_unused = sectors_unused;
    info.sectors_unallocated = sectors_unallocated;
    info.significant_threshold = significant_threshold;
    info.free_space_before = free_space_before;
    info.sector_size = sector_size;
    info.fs_block_size = fs_block_size;
    info.path = path;
    info.filesystem_label = filesystem_label;
    info.inside_extended = inside_extended;
    info.busy = busy;
    info.fs_readonly = fs_readonly;
    info.mountpoints = mountpoints;
    qDebug() << info.device_path << info.partition_number << info.type << info.status << info.alignment << info.fstype << info.uuid
             << info.name << info.sector_start << info.sector_end << info.sectors_used << info.sectors_unused
             << info.sectors_unallocated << info.significant_threshold << info.free_space_before
             << info.sector_size << info.fs_block_size << info.path << info.filesystem_label;
    return  info;
}

Sector Partition::calc_significant_unallocated_sectors() const
{
    const double HIGHER_UNALLOCATED_FRACTION = 0.05 ;
    const double LOWER_UNALLOCATED_FRACTION  = 0.02 ;
    Sector     length   = get_sector_length() ;
    Byte_Value byte_len = length * sector_size ;
    Sector     significant ;

    if (byte_len <= 0) {
        significant = 1;
    } else if (byte_len <= 100 * MEBIBYTE) {
        significant = qRound(length * HIGHER_UNALLOCATED_FRACTION) ;
    } else if (byte_len <= 1 * GIBIBYTE) {
        double fraction = (HIGHER_UNALLOCATED_FRACTION - LOWER_UNALLOCATED_FRACTION) -
                          (byte_len - 100 * MEBIBYTE) * (HIGHER_UNALLOCATED_FRACTION - LOWER_UNALLOCATED_FRACTION) /
                          (1 * GIBIBYTE - 100 * MEBIBYTE) +
                          LOWER_UNALLOCATED_FRACTION ;
        significant = qRound(length * fraction) ;
    } else {
        significant = qRound(length * LOWER_UNALLOCATED_FRACTION) ;
    }
    if (significant <= 1)
        significant = 1;
    return significant ;
}

}//namespace DiskManager
