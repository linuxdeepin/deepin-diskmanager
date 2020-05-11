#ifndef DEVICE_H
#define DEVICE_H
#include "deviceinfo.h"
#include "partitionvector.h"
#include <QtDBus/QtDBus>

namespace DiskManager {

class Device
{
public:
    Device();

public:
    Sector length;
    Sector heads ;
    QString m_path;
    Sector sectors ;
    Sector cylinders ;
    Sector cylsize ;
    QString model;
    QString serial_number;
    QString disktype;
    int sector_size ;
    int max_prims ;
    int highest_busy ;
    bool readonly ;
    int max_partition_name_length;
    PartitionVector partitions;

};

}//namespace DiskManager
#endif // DEVICE_H

