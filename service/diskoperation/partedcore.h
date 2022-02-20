/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partedcore.h
 *
 * @brief 磁盘操作类
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
#include "sigtype.h"
#include "log.h"
#include "device.h"
#include "supportedfilesystems.h"
#include "thread.h"
#include "DeviceStorage.h"
#include "lvmoperator/lvmoperator.h"

#include <QObject>
#include <QVector>
#include <QMap>
#include <QStringList>
#include <QFile>

#include <parted/parted.h>
#include <parted/device.h>

namespace DiskManager {

/**
 * @class PartedCore
 * @brief 磁盘操作类
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
     * @brief 获取LVM设备信息
     * @return 返回LVM设备信息
     */
    LVMInfo getAllLVMinfo();

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
    bool mountAndWriteFstab(const QString &mountpath);

    /**
     * @brief 临时挂载分区
     * @param mountpath：挂载路径
     * @return true成功false失败
     */
    bool mountTemp(const QString &mountpath, const QString devPath = "",const FSType&fsType=FS_UNKNOWN);

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
     * @brief 清除磁盘
     * @param fstype：格式化格式
     * @param path：分区别名
     * @param name: 劵标名
     * @param diskType : 0为分区，1为磁盘，2为空闲
     * @param clearType: 清除标准， 0为快速，1为安全（NIST），2为DoD标准， 3为古德曼标准
     * @return true成功false失败
     */
    bool clear(const QString &fstype, const QString &path,const QString &name,const QString &user ,const int &diskType , const int &clearType);

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
    bool deletePartition();

    /**
     * @brief 隐藏分区
     * @param devicePath：设备路径
     * @param parttitionPath：分区路径
     * @return true成功false失败
     */
    bool hidePartition();

    /**
     * @brief 显示分区
     * @param devicePath：设备路径
     * @param parttitionPath：分区路径
     * @return true成功false失败
     */
    bool showPartition();

    /**
     * @brief 创建分区表
     * @param devicepath：设备信息路径
     * @param length:设备大小
     * @param sectorSize:扇区大小
     * @param diskLabel:分区表格式
     * @return true成功false失败
     */
    bool createPartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel);

    /**
     * @brief 分区表错误检测
     * @param devicePath：设备路径
     * @return true错误false正常
     */
    bool detectionPartitionTableError(const QString &devicePath);

    /**
     * @brief USB刷新(插入)
     */
    bool updateUsb();

    /**
     * @brief USB刷新(拔出)
     */
    bool updateUsbRemove();

    /**
     * @brief 获取隐藏分区是否隐藏属性
     * @return 0成功-1失败
     */
    int getPartitionHiddenFlag();

    /**
     * @brief 坏道检测（检测次数）
     * @param devicePath：设备信息路径
     * @param blockStart：开始柱面
     * @param blockEnd：结束柱面
     * @param checkConut：检测次数
     * @param checkSize：检测柱面大小
     * @return true错误false正常
     */
    bool checkBadBlocks(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize,int flag);

    /**
     * @brief 坏道检测（超时时间）
     * @param devicePath：设备信息路径
     * @param blockStart：开始柱面
     * @param blockEnd：结束柱面
     * @param checkTime: 检测超时时间
     * @param checkSize：检测柱面大小
     * @return true错误false正常
     */
    bool checkBadBlocks(const QString &devicePath, int blockStart, int blockEnd, QString checkTime, int checkSize, int flag);

    /**
     * @brief 坏道修复
     * @param devicePath：设备信息路径
     * @param blockStart：开始修复柱面(为0则全部修复)
     * @param checkSize：检测柱面大小
     * @return true错误false正常
     */
    bool fixBadBlocks(const QString &devicePath, QStringList badBlocksList, int checkSize, int flag);

    /**
     * @brief 刷新按钮
     */
    void refreshFunc();

    /**
     * @brief 个人测试
     */
    int test();

    /**
     * @brief 确定是否是真正的设备
     * @param lpDisk：设备信息
     * @return true确定false不确定
     */
    static bool useableDevice(const PedDevice *lpDevice);

    /**
     * @brief 设置设备信息根据磁盘
     * @param device：设备信息
     * @param devicePath：设备路径
     */
    void setDeviceFromDisk(Device &device, const QString &devicePath);

    /**
     * @brief 删除临时挂载文件
     */
    bool delTempMountFile();

     /**
     * @brief 判断设备有否经历过gpt分区表扩容
     * @param devicePath：设备路径
     * @return true扩展后gpt分区表状态与实际不一致, false分区表状态正常
     */
    bool gptIsExpanded(const QString &devicePath);

    /**
     * @brief 创建vg
     * @param vgName:待创建vg名称
     * @param devList: pv设备集合
     * @param size:vg总大小
     * @return true 成功 false 失败
     */
    bool createVG(QString vgName, QList<PVData>devList, long long size);

    /**
     * @brief 创建lv
     * @param vgName:vg名称
     * @param lvList: 待创建lv列表
     * @return true 成功 false 失败
     */
    bool createLV(QString vgName, QList<CreateLVInfo>lvList);

    /**
     * @brief 删除vg
     * @param vglist: 待删除vg列表
     * @return true 成功 false 失败
     */
    bool deleteVG(QStringList vglist);

    /**
     * @brief 删除lv
     * @param lvlist: 待删除lv列表
     * @return true 成功 false 失败
     */
    bool deleteLV(QStringList lvlist);

    /**
     * @brief vg空间调整
     * @param vgName:vg名称
     * @param devList: pv设备集合
     * @param size:调整后vg总大小
     * @return true 成功 false 失败
     */
    bool resizeVG(QString vgName, QList<PVData>devList, long long size);

    /**
     * @brief lv空间调整
     * @param lvPath:lv路径
     * @param size: 调整后lv总大小
     * @return true 成功 false 失败
     */
    bool resizeLV(QString lvPath, QString size);

private:

    /**
     * @brief USB设备拔出，自动卸载
     */
    void autoUmount();

    /**
     * @brief USB设备插入，自动挂载
     */
    void autoMount();

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

    /**
     * @brief 安全擦除算法
     * @param start: 启始地址
     * @param end: 结束地址
     * @param size: 扇区大小
     * @param fstype: 文件类型
     * @param name : 劵标名
     * @param count: 循环次数
     * @return : 执行结果
     */
    bool secuClear(const QString &path, const Sector &start, const Sector &end, const Byte_Value &size, const QString &fstype,const QString &name=" ", const int &count=1);

private:
    //general..
    /**
     * @brief 初始化信号槽链接
     */
    void initConnection();

    /**
     * @brief 创建分区表
     * @param devicePath:设备路径
     * @param diskLabel:分区表格式
     * @return true成功false失败
     */
    bool newDiskLabel(const QString &devicePath, const QString &diskLabel);

    /**
     * @brief 刷新设备设置
     * @param lpDevice：设备信息
     * @return true成功false失败
     */
    static bool flushDevice(PedDevice *lpDevice);

//    /**
//     * @brief 刷新udev事件队列
//     * @param timeout：超时时间
//     */
//    static void settleDevice(std::time_t timeout);

    /**
     * @brief 等待udev时间处理，启动分区内核更新
     * @param lpDisk：设备信息
     * @param timeout：超时时间
     * @return true成功false失败
     */
    static bool commitToOs(PedDisk *lpDisk);

    /**
     * @brief 确定是否是真正的设备
     * @param lpDisk：设备信息
     * @return true确定false不确定
     */
    //static bool useableDevice(const PedDevice *lpDevice);

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
    bool infoBelongToPartition(const Partition &partition, const PartitionInfo &info);

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

    /**
     * @brief 获取分区详细信息
     * @param path：默认空
     */
    void probeDeviceInfo(const QString &path = QString());

    /**
     * @brief 设置设备信息根据磁盘
     * @param device：设备信息
     * @param devicePath：设备路径
     */
    //void setDeviceFromDisk(Device &device, const QString &devicePath);

    /**
     * @brief 设置设备序列号
     * @param device：设备信息
     */
    void setDeviceSerialNumber(Device &device);

    /**
     * @brief 设置设备分区信息
     * @param device：设备信息
     * @param lpDevice：分区详细信息
     * @param fstype：文件系统格式
     */
    void setDeviceOnePartition(Device &device, PedDevice *lpDevice, FSType fstype);

    /**
     * @brief 设置分区表和分区UUID
     * @param partition：分区信息
     */
    void setPartitionLabelAndUuid(Partition &partition);

    /**
     * @brief 分区是否挂载状态
     * @param fstype：文件系统格式
     * @param path：挂载路径
     * @param lp_partition：分区详细信息
     * @return true挂载false没挂载
     */
    bool isBusy(FSType fstype, const QString &path, const PedPartition *lpPartition = nullptr);

    /**
     * @brief 读取分区表
     * @param partition：分区信息
     */
    void readLabel(Partition &partition);

    /**
     * @brief 读取UUid
     * @param partition：分区信息
     */
    void readUuid(Partition &partition);

    /**
     * @brief 设置分区挂载点信息
     * @param partition：分区信息
     */
    void setMountPoints(Partition &partition);

    /**
     * @brief 设置分区挂载点信息帮助
     * @param partition：分区信息
     * @param path：挂载路径
     * @return true成功false失败
     */
    bool setMountPointsHelper(Partition &partition, const QString &path);

    /**
     * @brief 设置分区已用空间
     * @param partition：分区信息
     * @param lpDisk：设备信息
     */
    void setUsedSectors(Partition &partition, PedDisk *lpDisk);

    /**
     * @brief 设置挂载状态分区已用空间
     * @param partition：分区信息
     */
    void mountedFileSystemSetUsedSectors(Partition &partition);

    /**
     * @brief 设置设备分区信息
     * @param device：设备信息
     * @param lpDevice:设备详细信息
     * @param lpDisk：磁盘信息
     */
    void setDevicePartitions(Device &device, PedDevice *lpDevice, PedDisk *lpDisk);

    /**
     * @brief 检查文件系统
     * @param lpDevice：设备详细信息
     * @param lpPartition:分区详细信息
     * @return 文件系统格式
     */
    static FSType detectFilesystem(PedDevice *lpDevice, PedPartition *lpPartition);

    /**
     * @brief 检查内部文件系统
     * @param path：路径
     * @param sectorSize:扇区大小
     * @return 文件系统格式
     */
    static FSType detectFilesystemInternal(const QString &path, Byte_Value sectorSize);

    /**
     * @brief 获取分区路径
     * @param lpPartition：分区详细信息
     * @return 分区路径
     */
    static QString getPartitionPath(PedPartition *lpPartition);

    /**
     * @brief 设置分区已用空间
     * @param partition：分区信息
     * @param lpDisk：磁盘信息
     */
    void LpSetUsedSectors(Partition &partition, PedDisk *lpDisk);

    //operationstuff...
    /**
     * @brief 设置分区别名
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool namePartition(const Partition &partition);

    /**
     * @brief 抹除分区原来的文件系统签名
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool eraseFilesystemSignatures(const Partition &partition);

    /**
     * @brief 设置分区格式
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool setPartitionType(const Partition &partition);

    /**
     * @brief 创建分区文件系统
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool createFileSystem(const Partition &partition);

    /**
     * @brief 格式化分区
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool formatPartition(const Partition &partition);

    /**
     * @brief 扩容分区
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool resize(const Partition &partitionNew);

    /**
     * @brief 检查修复文件系统
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool checkRepairFileSystem(const Partition &partition);

    /**
     * @brief 调整移动分区大小
     * @param partitionOld：旧分区信息
     * @param partitionNew：新分区信息
     * @param rollbackOnFail：回滚标志
     * @return true成功false失败
     */
    bool resizeMovePartition(const Partition &partitionOld, const Partition &partitionNew, bool rollbackOnFail);


    /**
     * @brief 调整移动分区实现大小
     * @param partitionOld：旧分区信息
     * @param partitionNew：新分区信息
     * @param newStart：新分区扇区开始
     * @param newEnd：新分区扇区结束
     * @return true成功false失败
     */
    bool resizeMovePartitionImplement(const Partition &partitionOld, const Partition &partitionNew, Sector &newStart, Sector &newEnd);

    /**
     * @brief 最大化文件系统
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool maxImizeFileSystem(const Partition &partition);

    /**
     * @brief 调整文件系统大小
     * @param partitionOld：旧分区信息
     * @param partitionNew：新分区信息
     * @return true成功false失败
     */
    bool resizeFileSystemImplement(const Partition &partitionOld, const Partition &partitionNew);

    /**
     * @brief 使用libparted调整移动文件系统大小
     * @param partitionOld：旧分区信息
     * @param partitionNew：新分区信息
     * @return true成功false失败
     */
    bool resizeMoveFileSystemUsingLibparted(const Partition &partitionOld, const Partition &partitionNew);

    /**
     * @brief 创建分区
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool create(Partition &newPartition);

    /**
     * @brief 创建分区
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool createPartition(Partition &newPartition, Sector minSize = 0);

    /**
     * @brief 重写分区表
     * @param devicePath：设备名称
     */
    void reWritePartition(const QString &devicePath);

    /**
     * @brief 发送刷新信号并且返回bool值
     * @param flag：设置的返回值
     * @return true false 与flag相同
     */
    inline bool sendRefSigAndReturn(bool flag)
    {
        emit refreshDeviceInfo();
        return flag;
    }

    /**
     * @brief 获取创建VG的pvdata 列表
     * @param devList: 设备列表
     * @param size: 总大小
     * @return true false 与flag相同
     */
    QList<PVData> getCreatePVList(const QList<PVData>& devList, const long long &totalSize);



    /**
     * @brief 获取创建PV的磁盘
     * @param devPath: 磁盘路径
     * @param dev: 磁盘设备 传入参数
     * @return true 获取成功 false 获取失败
     */
    bool getPVDevice(const QString &devPath,Device&dev);


    /**
     * @brief 获取创建PV的磁盘
     * @param pv: pv数据结构体
     * @param flag: pv需要创建分区表 标志位
     * @return 大于0为pv长度 单位byte  小于0 为错误
     */
    long long getPVSize(const PVData&pv,bool flag = false);


    /**
     * @brief 获取创建PV的分区结尾
     * @param pv: pv数据结构体
     * @param unallocaSize: 未分配空间
     * @return true 获取成功 false 获取失败
     */
    bool getPVStartEndSector(PVData & pv, const long long & unallocaSize);


    /**
     * @brief 创建pv分区
     * @param pv: pv数据结构体
     * @return true 获取成功 false 获取失败
     */
    bool createPVPartition(PVData & pv);


signals:
    void probeAllInfo();

    /**
     * @brief 创建分区表信号
     * @param flag:true成功false失败
     */
    void createTableMessage(const bool &flag);

    /**
     * @brief 更新信息信号
     * @param infomap：硬盘信息
     */
    void updateDeviceInfo(const DeviceInfoMap &infomap, const LVMInfo &lvmInfo);

    /**
     * @brief 刷新信息信号
     */
    void refreshDeviceInfo(int type = 0, bool arg1 = true, QString arg2 = "");

    /**
     * @brief 卸载状态信号
     * @param umountMessage:卸载信息
     */
    void unmountPartition(const QString &unmountMessage);

    /**
     * @brief 删除分区信号
     * @param deleteMessage：删除结果
     */
    void deletePartitionMessage(const QString &deleteMessage);

    /**
     * @brief 隐藏分区信号
     * @param hideMessage：隐藏结果
     */
    void hidePartitionInfo(const QString &hideMessage);

    /**
     * @brief 显示分区信号
     * @param hideMessage：显示结果
     */
    void showPartitionInfo(const QString &showMessage);

    /**
     * @brief USB信号
     */
    void usbUpdated();

    /**
     * @brief 坏道检测检测信息信号(次数检测)
     * @param cylinderNumber：检测柱面号
     * @param cylinderTimeConsuming：柱面耗时
     * @param cylinderStatus：柱面状态
     * @param cylinderErrorInfo：柱面错误信息
     */
    void checkBadBlocksCountInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);

    /**
     * @brief 坏道检测完成信号
     */
    void checkBadBlocksFinished();

    /**
     * @brief 坏道修复完成信号
     */
    void fixBadBlocksFinished();

    /**
     * @brief 坏道修复信息信号
     * @param cylinderNumber：检测柱面号
     * @param cylinderStatus：柱面状态
     */
    void fixBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming);

    /**
     * @brief 坏道检查线程启动信号(次数)
     */
    void checkBadBlocksRunCountStart();

    /**
     * @brief 坏道检查线程启动信号(时间)
     */
    void checkBadBlocksRunTimeStart();

    /**
     * @brief 坏道修复线程启动信号
     */
    void fixBadBlocksStart();
    /**
     * @brief 清除信号
     * @param clearMessage：清除结果
     */
    void clearMessage(const QString &clearMessage);



public slots:

    /**
     * @brief 刷新信息槽函数
     */
    void onRefreshDeviceInfo(int type = 0, bool arg1 = true, QString arg2 = "");


    /**
      * @brief 刷新硬件信息
      */
    void syncDeviceInfo(/*const QMap<QString, Device> deviceMap, */const DeviceInfoMap inforesult, const LVMInfo lvmInfo);

private:
    QVector<PedPartitionFlag> m_flags;    //分区标志hidden boot efi等
    QMap<QString, Device> m_deviceMap;    //设备对应信息表
    DeviceInfoMap m_inforesult;           //全部设备分区信息
    Partition m_curpartition;             //当前选中分区信息
    static SupportedFileSystems *m_supportedFileSystems; //支持的文件系统
    QByteArray m_hiddenPartition;         //隐藏分区（规则文件中的隐藏分区）
    QThread *m_workerCheckThread;         //坏道检查线程对象
    QThread *m_workerFixThread;           //坏道修复线程对象
    QThread *m_workerThreadProbe;         //硬件刷新专用
    WorkThread m_checkThread;
    FixThread m_fixthread;
    ProbeThread m_probeThread;
    bool m_isClear;

    LVMInfo m_lvmInfo;                    //lvm 数据集合

};

} // namespace DiskManager
#endif // PARTEDCORE_H
