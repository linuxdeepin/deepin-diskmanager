#include "partedcore.h"
#include "utils.h"

#include <unistd.h>
#include <fcntl.h>
#include <QDebug>

namespace DiskManager {
//hdparm可检测，显示与设定IDE或SCSI硬盘的参数。
//udevadm可检测设备热插拔
static bool udevadm_found = false;
static bool hdparm_found = false;
const std::time_t SETTLE_DEVICE_PROBE_MAX_WAIT_SECONDS = 1;
const std::time_t SETTLE_DEVICE_APPLY_MAX_WAIT_SECONDS = 10;

PartedCore::PartedCore(QObject *parent) : QObject(parent)
{
    find_supported_core();
    init();
}

void PartedCore::find_supported_core()
{
    udevadm_found = ! Utils::find_program_in_path("udevadm").isEmpty();
    hdparm_found = ! Utils::find_program_in_path("hdparm").isEmpty();
}

void PartedCore::init()
{
    m_devicepaths.clear();
    ped_device_probe_all();

    PedDevice *lp_device = ped_device_get_next(NULL) ;
    while (lp_device) {
        /* TO TRANSLATORS: looks like   Confirming /dev/sda */
        qDebug() << QString("Confirming %1").arg(lp_device->path);

        //only add this device if we can read the first sector (which means it's a real device)
        if (useable_device(lp_device))
            m_devicepaths.push_back(lp_device->path);
        qDebug() << lp_device->path;
        lp_device = ped_device_get_next(lp_device) ;
    }
    qDebug() << __FUNCTION__ << "devicepaths size=" << m_devicepaths.size();
    std::sort(m_devicepaths .begin(), m_devicepaths .end()) ;

    for (unsigned int t = 0 ; t < m_devicepaths .size() ; t++) {
        /*TO TRANSLATORS: looks like Searching /dev/sda partitions */
        DeviceInfo temp_device;
        set_device_from_disk(temp_device, m_devicepaths[t]);
        devices.push_back(temp_device);
    }
}

bool PartedCore::useable_device(const PedDevice *lp_device)
{
    Q_ASSERT(nullptr != lp_device);
    char *buf = static_cast<char *>(malloc(lp_device->sector_size));
    if (! buf)
        return false;

    // Must be able to read from the first sector before the disk device is considered
    // useable in GParted.
    bool success = false;
    int fd = open(lp_device->path, O_RDONLY | O_NONBLOCK);
    if (fd >= 0) {
        ssize_t bytes_read = read(fd, buf, lp_device->sector_size);
        success = (bytes_read == lp_device->sector_size);
        close(fd);
    }
    free(buf);
    return success;
}

void PartedCore::set_device_from_disk(DeviceInfo &device, const QString &device_path)
{
    PedDevice *lp_device = NULL;
    PedDisk *lp_disk = NULL;
    if (get_device(device_path, lp_device, true)) {
        device.heads       = lp_device->bios_geom.heads;
        device.length      = lp_device->length;
        device.m_path      = device_path;
//        device.model       = lp_device->model;
//        device.sector_size = lp_device->sector_size;
//        device.sectors     = lp_device->bios_geom.sectors;
//        device.cylinders   = lp_device->bios_geom.cylinders;
//        device.cylsize     = device.heads * device.sectors;
//        set_device_serial_number(device);
//        if (device.cylsize < (MEBIBYTE / device.sector_size))
//            device.cylsize = MEBIBYTE / device.sector_size;
    }
}

bool PartedCore::get_device(const QString &device_path, PedDevice *&lp_device, bool flush)
{
    lp_device = ped_device_get(device_path.toStdString().c_str());
    if (lp_device) {
        if (flush)
            // Force cache coherency before going on to read the partition
            // table so that libparted reading the whole disk device and the
            // file system tools reading the partition devices read the same
            // data.
            flush_device(lp_device);
        return true;
    }
    return false;
}

void PartedCore::set_device_serial_number(DeviceInfo &device)
{
    if (! hdparm_found)
        // Serial number left blank when the hdparm command is not installed.
        return;

    QString output, error;
    Utils::executcmd(QString("hdparm -I %1").arg(device.m_path), output, error);
    if (! error.isEmpty()) {
        // hdparm reported an error message to stderr.  Assume it's a device
        // without a hard drive serial number.
        //
        // Using hdparm -I to query Linux software RAID arrays and BIOS fake RAID
        // arrays, both devices without their own hard drive serial numbers,
        // produce this error:
        //     HDIO_DRIVE_CMD(identify) failed: Inappropriate ioctl for device
        //
        // And querying USB flash drives, also a device type without their own
        // hard drive serial numbers, generates this error:
        //     SG_IO: bad/missing sense data, sb[]:  70 00 05 00 00 00 00 0a ...
        device.serial_number = "none";
    } else {
        QString serial_number = Utils::regexp_label(output, "^[[:blank:]]*Serial Number:[[:blank:]]*(.*)[[:blank:]]*$");
        if (! serial_number.isEmpty())
            device.serial_number = serial_number;
    }
    // Otherwise serial number left blank when not found in the hdparm output.
}

bool PartedCore::flush_device(PedDevice *lp_device)
{
    bool success = false ;
    if (ped_device_open(lp_device)) {
        success = ped_device_sync(lp_device) ;
        ped_device_close(lp_device) ;
        // (!46) Wait for udev rules to complete after this ped_device_open() and
        // ped_device_close() pair to avoid busy /dev/DISK entry when running file
        // system specific querying commands on the whole disk device in the call
        // sequence after get_device() in set_device_from_disk().
        settle_device(SETTLE_DEVICE_PROBE_MAX_WAIT_SECONDS);
    }
    return success ;
}

void PartedCore::settle_device(std::time_t timeout)
{
    //如果支持udevadm
    //udevadm settle [options]　　查看udev事件队列，如果所有事件全部处理完就退出。timeout超时时间
    if (udevadm_found) {
        QString out, err;
        Utils::executcmd(QString("udevadm settle --timeout=%1").arg(timeout), out, err) ;
    } else
        sleep(timeout) ;
}

DeviceInfo PartedCore::getDeviceinfo()
{
    qDebug() << __FUNCTION__ << devices.size() << "^^^^^^^";
    DeviceInfo info = devices.at(0);
    qDebug() << __FUNCTION__ << "#########";
    qDebug() << info.m_path << info.heads << info.cylinders << info.serial_number << info.max_prims;
    return  info;
}

}//end namespace
