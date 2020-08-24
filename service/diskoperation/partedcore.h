#ifndef PARTEDCORE_H
#define PARTEDCORE_H
#include "log.h"
#include "device.h"
#include "supportedfilesystems.h"
#include <QObject>
#include <QVector>
#include <parted/parted.h>
#include <parted/device.h>
#include <QMap>
#include <QStringList>
#include <QFile>
#include <QDir>

namespace DiskManager {

class PartedCore : public QObject
{
    Q_OBJECT
public:
    explicit PartedCore(QObject *parent = nullptr);
    ~PartedCore();

    DeviceInfo getDeviceinfo();
    DeviceInfoMap getAllDeviceinfo();
    void setCurSelect(const PartitionInfo &info);

    //operationstuff...
    bool mount(const QString &mountpath);
    bool unmount();
    bool create(const PartitionVec &infovec);
    bool format(const QString &fstype, const QString &name);
    bool resize(const PartitionInfo &info);
//    bool autoMount(const QString &partitionPath, const QString &fstype, const QString &mountPath,const QString &uuid);
    QStringList getallsupportfs();
    //new add by li 2020/7/20
    /**
     * @brief 获取硬盘硬件信息
     * @return 返回硬盘硬件信息
     */
    HardDiskInfo getDeviceHardInfo(const QString &devicepath);
    //HardDiskInfo getDeviceHardInfo();
    /**
     * @brief 获取硬盘状态信息
     * @return 返回硬盘状态信息
     */
    QString getDeviceHardStatus(const QString &devicepath);
    //QString getDeviceHardStatus();
    /**
     * @brief 获取硬盘健康信息
     * @return 返回硬盘健康信息
     */
    HardDiskStatusInfoList getDeviceHardStatusInfo(const QString &devicepath);
    //HardDiskStatusInfoList getDeviceHardStatusInfo();
    /**
     * @brief 删除分区实际操作
     * @param true成功false失败
     * @信号返回值错误信息代表1：设备读取失败2：代表该分区不存在3：执行删除分区操作失败4：报告内核失败（但前几步执行成功，该分区已删除，提示用户重启（系统命令就是这么操作））
     */
    bool deletePartition(const QString &devicePath, const QString &parttitionPath);
    /**
     * @brief 隐藏分区
     * @param true成功false失败
     */
    bool hidePartition(const QString &devicePath, const QString &parttitionPath);
    /**
     * @brief 显示分区
     * @param true成功false失败
     */
    bool showPartition(const QString &devicePath, const QString &parttitionPath);
    /**
     * @brief 获取隐藏分区是否隐藏属性
     * @param 1隐藏0没隐藏
     */
    int getPartitionHiddenFlag(const QString &devicePath, const QString &parttitionPath);
    /**
     * @brief 分区表错误检测
     * @param true错误false正常
     */
    bool detectionPartitionTableError(const QString &devicePath);
    /**
     * @brief USB设备拔出，自动卸载
     * @param 无
     */
    void autoUmount();
    /**
     * @brief 个人测试
     * @param 无
     */
    int test();
    /**
     * @brief USB刷新(插入)
     * @param 无
     */
    void updateUsb();
    /**
     * @brief USB刷新(拔出)
     * @param 无
     */
    void updateUsbRemove();
public:
    //static
    static void find_supported_core();
    static bool supported_filesystem(FSType fstype);
    const FS &get_fs(FSType fstype) const;
    static FileSystem *get_filesystem_object(FSType fstype);
    static bool filesystem_resize_disallowed(const Partition &partition);
    static void insert_unallocated(const QString &device_path,
                                   QVector<Partition *> &partitions,
                                   Sector start,
                                   Sector end,
                                   Byte_Value sector_size,
                                   bool inside_extended);

    void set_flags(Partition &partition, PedPartition *lp_partition);
    static FS_Limits get_filesystem_limits(FSType fstype, const Partition &partition);

private:
    //general..
    static bool flush_device(PedDevice *lp_device);
    //static void settle_device(std::time_t timeout);
    static bool commit_to_os(PedDisk *lp_disk);
    static bool useable_device(const PedDevice *lp_device);
    static bool get_device(const QString &device_path, PedDevice *&lp_device, bool flush = false);
    static bool get_disk(PedDevice *&lp_device, PedDisk *&lp_disk, bool strict = true);
    static void destroy_device_and_disk(PedDevice *&lp_device, PedDisk *&lp_disk);
    bool infoBelongToPartition(const Partition &partition, const PartitionInfo info);
    static bool get_device_and_disk(const QString &device_path, PedDevice *&lp_device,
                                    PedDisk *&lp_disk, bool strict = true, bool flush = false);
    static bool commit(PedDisk *lp_disk);
    static PedPartition *get_lp_partition(const PedDisk *lp_disk, const Partition &partition);
    //detectionstuff..
    void probedeviceinfo(const QString &path = QString());
    void set_device_from_disk(Device &device, const QString &device_path);
    void set_device_serial_number(Device &device);
    void set_device_one_partition(Device &device, PedDevice *lp_device, FSType fstype);
    void set_partition_label_and_uuid(Partition &partition);
    bool is_busy(FSType fstype, const QString &path, const PedPartition *lp_partition = nullptr);
    void read_label(Partition &partition);
    void read_uuid(Partition &partition);
    void set_mountpoints(Partition &partition);
    bool set_mountpoints_helper(Partition &partition, const QString &path);
    void set_used_sectors(Partition &partition, PedDisk *lp_disk);
    void mounted_fs_set_used_sectors(Partition &partition);
    void set_device_partitions(Device &device, PedDevice *lp_device, PedDisk *lp_disk);

    static FSType detect_filesystem(PedDevice *lp_device, PedPartition *lp_partition);
    static FSType detect_filesystem_internal(const QString &path, Byte_Value sector_size);
    static QString get_partition_path(PedPartition *lp_partition);
    void LP_set_used_sectors(Partition &partition, PedDisk *lp_disk);

    //operationstuff...
    bool name_partition(const Partition &partition);
    bool erase_filesystem_signatures(const Partition &partition);
    bool set_partition_type(const Partition &partition);
    bool create_filesystem(const Partition &partition);
    bool formatpartition(const Partition &partition);
    bool resize(const Partition &partition_new);
    bool check_repair_filesystem(const Partition &partition);
    bool resize_move_partition(const Partition &partition_old, const Partition &partition_new, bool rollback_on_fail);
    bool resize_move_partition_implement(const Partition &partition_old, const Partition &partition_new, Sector &new_start, Sector &new_end);
    bool maximize_filesystem(const Partition &partition);
    bool resize_filesystem_implement(const Partition &partition_old, const Partition &partition_new);
    bool resize_move_filesystem_using_libparted(const Partition &partition_old, const Partition &partition_new);
    bool create(Partition &partition);
    bool create_partition(Partition &new_partition, Sector min_size = 0);
signals:
    void sigUpdateDeviceInfo(const DeviceInfoMap &infomap);
    void sigRefreshDeviceInfo();
    void sigDeletePatition(const QString &deleteMessage);
    void sigHidePartition(const QString &hideMessage);
    void sigShowPartition(const QString &showMessage);
    void sigUpdateUsb();
public slots:
    void slotRefreshDeviceInfo();
    void autoMount();

private:
    QVector<PedPartitionFlag> flags;
    QMap<QString, Device> devicemap;
    DeviceInfoMap inforesult;
    Partition curpartition;
    static SupportedFileSystems *supported_filesystems;
};

} // namespace DiskManager
#endif // PARTEDCORE_H
