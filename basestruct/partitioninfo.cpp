#include "partitioninfo.h"
#include <QDBusArgument>

PartitionInfo::PartitionInfo():
    inside_extended(false),
    busy(false),
    fs_readonly(false)
{
    //inside_extended = busy = fs_readonly = false;
    device_path = uuid = name = path = filesystem_label = "";
    partition_number = type = status = alignment = fstype = 0;
    sector_start = sector_end = sectors_used = sectors_unused = sectors_unallocated = significant_threshold = free_space_before = 0;
    // mountpoints.clear();
}

Sector PartitionInfo::get_sector_length() const
{
    if (sector_start >= 0 && sector_end >= 0)
        return sector_end - sector_start + 1 ;
    else
        return -1 ;
}

Byte_Value PartitionInfo::get_byte_length() const
{
    if (get_sector_length() >= 0)
        return get_sector_length() * sector_size ;
    else
        return -1 ;
}

bool PartitionInfo::operator==(const PartitionInfo &info) const
{
    return device_path == info .device_path &&
           partition_number == info .partition_number &&
           sector_start == info .sector_start &&
           type == info .type ;
}
QDBusArgument &operator<<(QDBusArgument &argument, const PartitionInfo &info)
{
    argument.beginStructure();
    argument << info.device_path
             << info.partition_number
             << info.type
             << info.status
             << info.alignment
             << info.fstype
             << info.uuid
             << info.name
             << info.sector_start
             << info.sector_end
             << info.sectors_used
             << info.sectors_unused
             << info.sectors_unallocated
             << info.significant_threshold
             << info.free_space_before
             << info.sector_size
             << info.fs_block_size
             << info.path
             << info.filesystem_label
             << info.inside_extended
             << info.busy
             << info.fs_readonly
             << info.mountpoints;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, PartitionInfo &info)
{
    argument.beginStructure();

    argument >> info.device_path
             >> info.partition_number
             >> info.type
             >> info.status
             >> info.alignment
             >> info.fstype
             >> info.uuid
             >> info.name
             >> info.sector_start
             >> info.sector_end
             >> info.sectors_used
             >> info.sectors_unused
             >> info.sectors_unallocated
             >> info.significant_threshold
             >> info.free_space_before
             >> info.sector_size
             >> info.fs_block_size
             >> info.path
             >> info.filesystem_label
             >> info.inside_extended
             >> info.busy
             >> info.fs_readonly
             >> info.mountpoints;
    argument.endStructure();

    return argument;
}
