#ifndef PARTEDCORE_H
#define PARTEDCORE_H

#include <QObject>
#include <QVector>
#include <parted/parted.h>
#include <parted/device.h>
#include <QMap>
#include "utils.h"
#include "deviceinfo.h"
namespace DiskManager {

class PartedCore : public QObject
{
    Q_OBJECT
public:
    explicit PartedCore(QObject *parent = nullptr);

    //static
    static void find_supported_core();

public:
    DeviceInfo getDeviceinfo();

private:
    void init();
    bool useable_device(const PedDevice *lp_device);
    void set_device_from_disk(DeviceInfo &device, const QString &device_path);
    bool get_device(const QString &device_path, PedDevice *&lp_device, bool flush);
    void set_device_serial_number(DeviceInfo &device);
    bool flush_device(PedDevice *lp_device);
    void settle_device(std::time_t timeout);

signals:

public slots:


public:
    QVector<QString> m_devicepaths;
    QVector<DeviceInfo> devices;

};

}
#endif // PARTEDCORE_H
