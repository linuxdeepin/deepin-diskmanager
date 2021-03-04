#include "deviceinfo.h"
#include <QDBusArgument>

QDBusArgument &operator<<(QDBusArgument &argument, const DeviceInfo &info)
{
    argument.beginStructure();
    argument << info.length
             << info.heads
             << info.m_path
             << info.sectors
             << info.cylinders
             << info.cylsize
             << info.model
             << info.serial_number
             << info.disktype
             << info.sector_size
             << info.max_prims
             << info.highest_busy
             << info.readonly
             << info.max_partition_name_length
             << info.partition;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DeviceInfo &info)
{
    argument.beginStructure();
    argument >> info.length
        >> info.heads
        >> info.m_path
        >> info.sectors
        >> info.cylinders
        >> info.cylsize
        >> info.model
        >> info.serial_number
        >> info.disktype
        >> info.sector_size
        >> info.max_prims
        >> info.highest_busy
        >> info.readonly
        >> info.max_partition_name_length
        >> info.partition;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const stCustest &stcus)
{
    argument.beginStructure();
    argument << stcus.length
             << stcus.heads
             << stcus.m_path;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, stCustest &stcus)
{
    argument.beginStructure();
    argument >> stcus.length
        >> stcus.heads
        >> stcus.m_path;

    argument.endStructure();
    return argument;
}
QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskInfo &inhdinfo)
{
    argument.beginStructure();
    argument << inhdinfo.m_deviceModel
             << inhdinfo.m_serialNumber
             << inhdinfo.m_deviceId
             << inhdinfo.m_firmwareVersion
             << inhdinfo.m_userCapacity
             << inhdinfo.m_sectorSize
             << inhdinfo.m_rotationRate
             << inhdinfo.m_formFactor
             << inhdinfo.m_deviceis
             << inhdinfo.m_ataVersionIs
             << inhdinfo.m_sataVersionIs
             << inhdinfo.m_localTime
             << inhdinfo.m_smartSupport
             << inhdinfo.m_smartSupportOn_Off
             << inhdinfo.m_Model
             << inhdinfo.m_Vendor
             << inhdinfo.m_MediaType
             << inhdinfo.m_Size
             << inhdinfo.m_RotationRate
             << inhdinfo.m_Interface
             << inhdinfo.m_SerialNumber
             << inhdinfo.m_Version
             << inhdinfo.m_Capabilities
             << inhdinfo.m_Description
             << inhdinfo.m_PowerOnHours
             << inhdinfo.m_PowerCycleCount
             << inhdinfo.m_FirmwareVersion
             << inhdinfo.m_Speed;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskInfo &inhdinfo)
{
    argument.beginStructure();
    argument >> inhdinfo.m_deviceModel
            >> inhdinfo.m_serialNumber
            >> inhdinfo.m_deviceId
            >> inhdinfo.m_firmwareVersion
            >> inhdinfo.m_userCapacity
            >> inhdinfo.m_sectorSize
            >> inhdinfo.m_rotationRate
            >> inhdinfo.m_formFactor
            >> inhdinfo.m_deviceis
            >> inhdinfo.m_ataVersionIs
            >> inhdinfo.m_sataVersionIs
            >> inhdinfo.m_localTime
            >> inhdinfo.m_smartSupport
            >> inhdinfo.m_smartSupportOn_Off
            >> inhdinfo.m_Model
            >> inhdinfo.m_Vendor
            >> inhdinfo.m_MediaType
            >> inhdinfo.m_Size
            >> inhdinfo.m_RotationRate
            >> inhdinfo.m_Interface
            >> inhdinfo.m_SerialNumber
            >> inhdinfo.m_Version
            >> inhdinfo.m_Capabilities
            >> inhdinfo.m_Description
            >> inhdinfo.m_PowerOnHours
            >> inhdinfo.m_PowerCycleCount
            >> inhdinfo.m_FirmwareVersion
            >> inhdinfo.m_Speed;
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
    : readonly(false)
{
    length = heads = sectors = cylinders = cylsize = sector_size = max_prims = highest_busy = max_partition_name_length = 0;
    m_path = model = serial_number = disktype = QString("");
    partition.clear();
}
