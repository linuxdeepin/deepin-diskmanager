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

    //static
    static void find_supported_core();
    static bool supported_filesystem(FSType fstype);
    const FS &get_fs(FSType fstype) const;
    static FileSystem *get_filesystem_object(FSType fstype);

public:
    DeviceInfo getDeviceinfo();

private:
    //general..
    bool flush_device(PedDevice *lp_device);
    void settle_device(std::time_t timeout);
    bool useable_device(const PedDevice *lp_device);
    bool get_device(const QString &device_path, PedDevice *&lp_device, bool flush);

    //detectionstuff..
    void probedeviceinfo(const QString &path = QString());
    void set_device_from_disk(Device &device, const QString &device_path);
    void set_device_serial_number(Device &device);
    void set_device_one_partition(Device &device, PedDevice *lp_device, FSType fstype);
    void set_partition_label_and_uuid(Partition &partition);
    bool is_busy(FSType fstype, const QString &path);
    void read_label(Partition &partition) ;
    void read_uuid(Partition &partition) ;
    void set_mountpoints(Partition &partition);
    void set_used_sectors(Partition &partition, PedDisk *lp_disk);
    void mounted_fs_set_used_sectors(Partition &partition);

    static FSType detect_filesystem(PedDevice *lp_device, PedPartition *lp_partition);
    static FSType detect_filesystem_internal(const QString &path, Byte_Value sector_size);
    static QString get_partition_path(PedPartition *lp_partition);

    //operationstuff...
signals:

public slots:


public:
    QVector<QString> m_devicepaths;
    QVector<DeviceInfo> devices;
    QMap<QString, Device> devicemap;
    static SupportedFileSystems *supported_filesystems;

};

}
#endif // PARTEDCORE_H
