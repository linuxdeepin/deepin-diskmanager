#include "partitioninfo.h"
#include <QDBusArgument>

PartitionInfo::PartitionInfo()
{
    // inside_extended = busy = fs_readonly = false;
    device_path = uuid = name = path = filesystem_label = "";
    partition_number = type = status = alignment = fstype = 0;
    sector_start = sector_end = sectors_used = sectors_unused = sectors_unallocated = significant_threshold = free_space_before = 0;
    // mountpoints.clear();
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
