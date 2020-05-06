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
             << info.max_partition_name_length;
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
             >> info.max_partition_name_length;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const stCustest &stcus)
{
    argument.beginStructure();
    argument << stcus.length
             << stcus.heads
             << stcus.m_path
             << stcus.btest;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, stCustest &stcus)
{
    argument.beginStructure();
    argument >> stcus.length
             >> stcus.heads
             >> stcus.m_path
             >> stcus.btest;

    argument.endStructure();
    return argument;
}


