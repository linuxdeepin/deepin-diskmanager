#ifndef PARTEDCORE_H
#define PARTEDCORE_H
#include "device.h"
#include "supportedfilesystems.h"
#include <QObject>
#include <QVector>
#include <parted/parted.h>
#include <parted/device.h>
#include <QMap>


namespace DiskManager {

class PartedCore : public QObject
{
    Q_OBJECT
public:
    explicit PartedCore(QObject *parent = nullptr);

    DeviceInfo getDeviceinfo();
    DeviceInfoMap getAllDeviceinfo();
    void curSelectedChanged(const PartitionInfo &info);
public:

    //static
    static void find_supported_core();
    static bool supported_filesystem(FSType fstype);
    const FS &get_fs(FSType fstype) const;
    static FileSystem *get_filesystem_object(FSType fstype);
    static bool filesystem_resize_disallowed(const Partition &partition) ;
    static void insert_unallocated(const QString &device_path,
                                   QVector<Partition *> &partitions,
                                   Sector start,
                                   Sector end,
                                   Byte_Value sector_size,
                                   bool inside_extended);
    void set_flags(Partition &partition, PedPartition *lp_partition) ;

private:
    //general..
    static bool flush_device(PedDevice *lp_device);
    static void settle_device(std::time_t timeout);
    static bool commit_to_os(PedDisk *lp_disk, std::time_t timeout);
    static bool useable_device(const PedDevice *lp_device);
    static bool get_device(const QString &device_path, PedDevice *&lp_device, bool flush);
    static bool get_disk(PedDevice *&lp_device, PedDisk *&lp_disk, bool strict = true);
    static void destroy_device_and_disk(PedDevice *&lp_device, PedDisk *&lp_disk);
    bool infoBelongToPartition(const Partition &partition, const PartitionInfo info);

    //detectionstuff..
    void probedeviceinfo(const QString &path = QString());
    void set_device_from_disk(Device &device, const QString &device_path);
    void set_device_serial_number(Device &device);
    void set_device_one_partition(Device &device, PedDevice *lp_device, FSType fstype);
    void set_partition_label_and_uuid(Partition &partition);
    bool is_busy(FSType fstype, const QString &path, const PedPartition *lp_partition = nullptr);
    void read_label(Partition &partition) ;
    void read_uuid(Partition &partition) ;
    void set_mountpoints(Partition &partition);
    bool set_mountpoints_helper(Partition &partition, const QString &path);
    void set_used_sectors(Partition &partition, PedDisk *lp_disk);
    void mounted_fs_set_used_sectors(Partition &partition);
    void set_device_partitions(Device &device, PedDevice *lp_device, PedDisk *lp_disk) ;

    static FSType detect_filesystem(PedDevice *lp_device, PedPartition *lp_partition);
    static FSType detect_filesystem_internal(const QString &path, Byte_Value sector_size);
    static QString get_partition_path(PedPartition *lp_partition);

    //operationstuff...
    bool mount(const QString &partitionpath, const QString &mountpath);
    bool unmount(const QString &mountpath);
signals:

public slots:


private:
    QVector<PedPartitionFlag> flags;
    QMap<QString, Device> devicemap;
    DeviceInfoMap inforesult;
    Partition curpartition;
    static SupportedFileSystems *supported_filesystems;

};

}
#endif // PARTEDCORE_H
