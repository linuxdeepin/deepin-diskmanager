/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partedcore.h
 *
 * @brief 磁盘分区具体实现类
 *
 * @date 2020-09-01 16:47
 *
 * Author: liweigang  <liweigang@uniontech.com>
 *
 * Maintainer: liweigang  <liweigang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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

/**
 * @class DiskManagerService
 * @brief 磁盘分区具体实现类
 */

class PartedCore : public QObject
{
    Q_OBJECT
public:
    explicit PartedCore(QObject *parent = nullptr);
    ~PartedCore();
    /**
     * @brief 获取设备信息
     * @return 返回设备信息
     */
    DeviceInfo getDeviceinfo();

    /**
     * @brief 获取所有设备信息
     * @return 返回所有设备信息
     */
    DeviceInfoMap getAllDeviceinfo();

    /**
     * @brief 获取当前页面选择分区信息
     * @param info：当前选择分区信息
     */
    void setCurSelect(const PartitionInfo &info);

    //operationstuff...
    /**
     * @brief 挂载分区
     * @param mountpath：挂载路径
     * @return true成功false失败
     */
    bool mount(const QString &mountpath);

    /**
     * @brief 卸载分区
     * @return true成功false失败
     */
    bool unmount();

    /**
     * @brief 挂载分区
     * @param infovec：要创建的分区信息列表
     * @return true成功false失败
     */
    bool create(const PartitionVec &infovec);

    /**
     * @brief 格式化分区
     * @param fstype：格式化格式
     * @param name：分区别名
     * @return true成功false失败
     */
    bool format(const QString &fstype, const QString &name);

    /**
     * @brief 扩容分区
     * @param info：扩容分区信息
     * @return true成功false失败
     */
    bool resize(const PartitionInfo &info);

    /**
     * @brief 获取支持格式列表
     * @return 返回支持格式列表
     */
    QStringList getallsupportfs();

    /**
     * @brief 获取硬盘硬件信息
     * @param devicepath：设备路径
     * @return 返回硬盘硬件信息
     */
    HardDiskInfo getDeviceHardInfo(const QString &devicepath);

    /**
     * @brief 获取硬盘状态信息
     * @param devicepath：设备路径
     * @return 返回硬盘状态信息
     */
    QString getDeviceHardStatus(const QString &devicepath);

    /**
     * @brief 获取硬盘健康信息
     * @param devicepath：设备路径
     * @return 返回硬盘健康信息
     */
    HardDiskStatusInfoList getDeviceHardStatusInfo(const QString &devicepath);

    /**
     * @brief 删除分区实际操作
     * @param devicePath：设备路径
     * @param parttitionPath：分区路径
     * @return true成功false失败
     */
    bool deletePartition(const QString &devicePath, const QString &parttitionPath);

    /**
     * @brief 隐藏分区
     * @param devicePath：设备路径
     * @param parttitionPath：分区路径
     * @return true成功false失败
     */
    bool hidePartition(const QString &devicePath, const QString &parttitionPath);

    /**
     * @brief 显示分区
     * @param devicePath：设备路径
     * @param parttitionPath：分区路径
     * @return true成功false失败
     */
    bool showPartition(const QString &devicePath, const QString &parttitionPath);

    /**
     * @brief 获取隐藏分区是否隐藏属性
     * @param devicePath：设备路径
     * @param parttitionPath：分区路径
     * @return 1隐藏0没隐藏
     */
    int getPartitionHiddenFlag(const QString &devicePath, const QString &parttitionPath);

    /**
     * @brief 分区表错误检测
     * @param devicePath：设备路径
     * @return true错误false正常
     */
    bool detectionPartitionTableError(const QString &devicePath);

    /**
     * @brief USB设备拔出，自动卸载
     */
    void autoUmount();

    /**
     * @brief USB设备插入，自动挂载
     */
    void autoMount();

    /**
     * @brief 个人测试
     */
    int test();

    /**
     * @brief USB刷新(插入)
     */
    void updateUsb();

    /**
     * @brief USB刷新(拔出)
     */
    void updateUsbRemove();

public:
    //static
    /**
     * @brief 获取hdparm和udevadm的是否支持状态
     */
    static void findSupportedCore();

    /**
     * @brief 获取是否支持文件系统格式
     * @param fstype：文件系统格式
     * @return true支持false不支持
     */
    static bool supportedFileSystem(FSType fstype);

    /**
     * @brief 获取支持文件系统格式
     * @param fstype：文件系统格式
     * @return 支持的文件系统格式
     */
    const FS &getFileSystem(FSType fstype) const;

    /**
     * @brief 获取文件系统对象
     * @param fstype：文件系统格式
     * @return 文件系统对象
     */
    static FileSystem *getFileSystemObject(FSType fstype);
//    static bool filesystem_resize_disallowed(const Partition &partition);

    /**
     * @brief 设置空闲空间
     * @param devicePath：设备路径
     * @param partitions：分区信息列表
     * @param start：扇区开始
     * @param end：扇区结束
     * @param sectorSize：扇区大小
     * @param insideExtended：扩展分区标志
     */
    static void insertUnallocated(const QString &devicePath,
                                   QVector<Partition *> &partitions,
                                   Sector start,
                                   Sector end,
                                   Byte_Value sectorSize,
                                   bool insideExtended);

    /**
     * @brief 设置分区标志
     * @param partition：分区信息
     * @param lpPartition：（库）分区详细信息
     */
    void setFlags(Partition &partition, PedPartition *lpPartition);

    /**
     * @brief 最小和最大文件系统大小限制
     * @param fstype：文件系统格式
     * @param partition：分区信息
     * @return 最小和最大文件系统大小限制结构体
     */
    static FS_Limits getFileSystemLimits(FSType fstype, const Partition &partition);

private:
    //general..
    /**
     * @brief 刷新设备设置
     * @param lpDevice：设备信息
     * @return true成功false失败
     */
    static bool flushDevice(PedDevice *lpDevice);

    /**
     * @brief 刷新udev事件队列
     * @param timeout：超时时间
     */
    static void settleDevice(std::time_t timeout);

    /**
     * @brief 等待udev时间处理，启动分区内核更新
     * @param lpDisk：设备信息
     * @param timeout：超时时间
     * @return true成功false失败
     */
    static bool commitToOs(PedDisk *lpDisk, time_t timeout);

    /**
     * @brief 确定是否是真正的设备
     * @param lpDisk：设备信息
     * @return true确定false不确定
     */
    static bool useableDevice(const PedDevice *lpDevice);

    /**
     * @brief 获取设备
     * @param devicePath：设备路径
     * @param lpDevice：设备信息
     * @param flush：是否先刷新设备更新
     * @return true成功false失败
     */
    static bool getDevice(const QString &devicePath, PedDevice *&lpDevice, bool flush = false);

    /**
     * @brief 获取磁盘
     * @param lpDevice：设备信息
     * @param lpDisk：磁盘信息
     * @param strict：标记位
     * @return true成功false失败
     */
    static bool getDisk(PedDevice *&lpDevice, PedDisk *&lpDisk, bool strict = true);

    /**
     * @brief 销毁磁盘信息和设备信息
     * @param lpDevice：设备信息
     * @param lpDisk：磁盘信息
     */
    static void destroyDeviceAndDisk(PedDevice *&lpDevice, PedDisk *&lpDisk);

    /**
     * @brief 同步页面选择分区信息
     * @param partition：分区信息
     * @param info：分区详细信息
     * @return true成功false失败
     */
    bool infoBelongToPartition(const Partition &partition, const PartitionInfo info);

    /**
     * @brief 获取磁盘和设备信息
     * @param devicePath：设备路径
     * @param lpDevice：设备信息
     * @param lpDisk：磁盘信息
     * @param flush：是否刷新设备标记位
     * @param strict：标记位
     * @return true成功false失败
     */
    static bool getDeviceAndDisk(const QString &devicePath, PedDevice *&lpDevice,
                                    PedDisk *&lpDisk, bool strict = true, bool flush = false);

    /**
     * @brief 更新到磁盘信息
     * @param lpDisk：磁盘信息
     * @return true成功false失败
     */
    static bool commit(PedDisk *lpDisk);

    /**
     * @brief 获取分区详细信息
     * @param lp_disk：磁盘信息
     * @return 磁盘详细信息
     */
    static PedPartition *getLpPartition(const PedDisk *lpDisk, const Partition &partition);
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
    void updateDeviceInfo(const DeviceInfoMap &infomap);
    void refreshDeviceInfo();
    void deletePatition(const QString &deleteMessage);
    void hidePartitionInfo(const QString &hideMessage);
    void showPartitionInfo(const QString &showMessage);
    void usbUpdated();
public slots:
    void onRefreshDeviceInfo();

private:
    QVector<PedPartitionFlag> flags;
    QMap<QString, Device> devicemap;
    DeviceInfoMap inforesult;
    Partition curpartition;
    static SupportedFileSystems *supported_filesystems;
};

} // namespace DiskManager
#endif // PARTEDCORE_H
