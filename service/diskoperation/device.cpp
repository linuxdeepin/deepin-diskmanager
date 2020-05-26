#include "device.h"
#include <QDebug>

namespace DiskManager {

Device::Device()
{
    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
    qDBusRegisterMetaType<PartitionVec>();
    qDBusRegisterMetaType<stCustest>();
}

void Device::enable_partition_naming(int length)
{
    if (length > 0)
        max_partition_name_length = length;
    else
        max_partition_name_length = 0;
}

bool Device::partition_naming_supported() const
{
    return max_partition_name_length > 0;
}

int Device::get_max_partition_name_length() const
{
    return max_partition_name_length;
}

DeviceInfo Device::getDeviceInfo()
{
    DeviceInfo info;
    info.length = length;
    info.heads = heads;
    info.m_path = m_path;
    info.sectors = sectors;
    info.cylinders = cylinders;
    info.cylsize = cylsize;
    info.model = model;
    info.serial_number = serial_number;
    info.disktype = disktype;
    info.sector_size = sector_size;
    info.max_prims = max_prims;
    info.highest_busy = highest_busy;
    info.max_partition_name_length = max_partition_name_length;
//    qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
//             << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
//             << info.sector_size << info.max_prims << info.highest_busy << info.readonly
//             << info.max_partition_name_length;
    return info;
}

}
