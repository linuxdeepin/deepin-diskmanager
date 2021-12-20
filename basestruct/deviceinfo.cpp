#include "deviceinfo.h"
#include <QDBusArgument>

QDBusArgument &operator<<(QDBusArgument &argument, const DeviceInfo &info)
{
    argument.beginStructure();
    argument << info.m_length
             << info.m_heads
             << info.m_path
             << info.m_sectors
             << info.m_cylinders
             << info.m_cylsize
             << info.m_model
             << info.m_serialNumber
             << info.m_disktype
             << info.m_sectorSize
             << info.m_maxPrims
             << info.m_highestBusy
             << info.m_readonly
             << info.m_maxPartitionNameLength
             << info.m_partition
             << info.m_mediaType
             << info.m_interface;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DeviceInfo &info)
{
    argument.beginStructure();
    argument >> info.m_length
        >> info.m_heads
        >> info.m_path
        >> info.m_sectors
        >> info.m_cylinders
        >> info.m_cylsize
        >> info.m_model
        >> info.m_serialNumber
        >> info.m_disktype
        >> info.m_sectorSize
        >> info.m_maxPrims
        >> info.m_highestBusy
        >> info.m_readonly
        >> info.m_maxPartitionNameLength
        >> info.m_partition
        >> info.m_mediaType
        >> info.m_interface;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const stCustest &stcus)
{
    argument.beginStructure();
    argument << stcus.m_length
             << stcus.m_heads
             << stcus.m_path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, stCustest &stcus)
{
    argument.beginStructure();
    argument >> stcus.m_length
        >> stcus.m_heads
        >> stcus.m_path;

    argument.endStructure();
    return argument;
}
QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskInfo &inhdinfo)
{
    argument.beginStructure();
    argument << inhdinfo.m_model
             << inhdinfo.m_vendor
             << inhdinfo.m_mediaType
             << inhdinfo.m_size
             << inhdinfo.m_rotationRate
             << inhdinfo.m_interface
             << inhdinfo.m_serialNumber
             << inhdinfo.m_version
             << inhdinfo.m_capabilities
             << inhdinfo.m_description
             << inhdinfo.m_powerOnHours
             << inhdinfo.m_powerCycleCount
             << inhdinfo.m_firmwareVersion
             << inhdinfo.m_speed;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskInfo &inhdinfo)
{
    argument.beginStructure();
    argument >> inhdinfo.m_model
            >> inhdinfo.m_vendor
            >> inhdinfo.m_mediaType
            >> inhdinfo.m_size
            >> inhdinfo.m_rotationRate
            >> inhdinfo.m_interface
            >> inhdinfo.m_serialNumber
            >> inhdinfo.m_version
            >> inhdinfo.m_capabilities
            >> inhdinfo.m_description
            >> inhdinfo.m_powerOnHours
            >> inhdinfo.m_powerCycleCount
            >> inhdinfo.m_firmwareVersion
            >> inhdinfo.m_speed;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskStatusInfo &inhdinfo)
{
    argument.beginStructure();
    argument << inhdinfo.m_id
             << inhdinfo.m_attributeName
             << inhdinfo.m_flag
             << inhdinfo.m_value
             << inhdinfo.m_worst
             << inhdinfo.m_thresh
             << inhdinfo.m_type
             << inhdinfo.m_updated
             << inhdinfo.m_whenFailed
             << inhdinfo.m_rawValue;
    argument.endStructure();
    return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskStatusInfo &inhdinfo)
{
    argument.beginStructure();
    argument >> inhdinfo.m_id
             >> inhdinfo.m_attributeName
             >> inhdinfo.m_flag
             >> inhdinfo.m_value
             >> inhdinfo.m_worst
             >> inhdinfo.m_thresh
             >> inhdinfo.m_type
             >> inhdinfo.m_updated
             >> inhdinfo.m_whenFailed
             >> inhdinfo.m_rawValue;
    argument.endStructure();
    return argument;
}

DeviceInfo::DeviceInfo()
    : m_readonly(false)
{
    m_length = m_heads = m_sectors = m_cylinders = m_cylsize = m_sectorSize = m_maxPrims = m_highestBusy = m_maxPartitionNameLength = 0;
    m_path = m_model = m_serialNumber = m_disktype = m_mediaType = m_interface = QString("");
    m_partition.clear();
}
