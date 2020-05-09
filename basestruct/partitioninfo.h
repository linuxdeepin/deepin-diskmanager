#ifndef PARTITIONINFO_H
#define PARTITIONINFO_H
#include "commondef.h"
#include <QString>

class QDBusArgument;
class PartitionInfo
{
public:
    PartitionInfo();

public:
    QString device_path ;
    int partition_number;
    PartitionType type;// UNALLOCATED, PRIMARY, LOGICAL, etc...
    PartitionStatus status; //STAT_REAL, STAT_NEW, etc..
    PartitionAlignment alignment;   //ALIGN_CYLINDER, ALIGN_STRICT, etc
    FSType fstype;
    QString uuid ;
    QString name;
    Sector sector_start;
    Sector sector_end;
    Sector sectors_used;
    Sector sectors_unused;
    Sector sectors_unallocated;  //Difference between the size of the partition and the file system
    Sector significant_threshold;  //Threshold from intrinsic to significant unallocated sectors
//    bool inside_extended;
//    bool busy;
//    bool fs_readonly;
};
Q_DECLARE_METATYPE(PartitionInfo)
QDBusArgument &operator<<(QDBusArgument &argument, const PartitionInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, PartitionInfo &info);

#endif // PARTITIONINFO_H
