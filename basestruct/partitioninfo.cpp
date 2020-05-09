#include "partitioninfo.h"
#include <QDBusArgument>

PartitionInfo::PartitionInfo()
{

}

QDBusArgument &operator<<(QDBusArgument &argument, const PartitionInfo &info)
{
    argument.beginStructure();
    argument << info.device_path;

    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, PartitionInfo &info)
{
    argument.beginStructure();
    argument >> info.device_path;

    argument.endStructure();

    return argument;
}
