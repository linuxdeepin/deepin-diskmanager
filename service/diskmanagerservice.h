/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskmanagerservice.h
 *
 * @brief 磁盘分区操作封装类
 *
 * @date 2020-09-01 10:49
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

#ifndef DISKMANAGERSERVICE_H
#define DISKMANAGERSERVICE_H
#include "diskoperation/partedcore.h"
#include "diskoperation/thread.h"
#include "PolicyKitHelper.h"

#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>

namespace DiskManager {

/**
 * @class DiskManagerService
 * @brief 磁盘分区操作封装类
 */

class DiskManagerService : public QObject
    , protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.diskmanager")
public:
    explicit DiskManagerService(QObject *parent = nullptr);

Q_SIGNALS:
    /**
     * @brief 删除分区信号
     * @param deleteMessage：删除信息
     */
    Q_SCRIPTABLE void rootLogin(const QString &loginMessage);

    /**
     * @brief 信息传递信号
     * @param msg：要传递的信息
     */
    Q_SCRIPTABLE void MessageReport(const QString &msg);

    /**
     * @brief 刷新设备信息信号
     * @param infomap：所有设备分区信息
     */
    Q_SCRIPTABLE void updateDeviceInfo(const DeviceInfoMap &infomap);

    /**
     * @brief 删除分区信号
     * @param deleteMessage：删除信息
     */
    Q_SCRIPTABLE void deletePatition(const QString &deleteMessage);

    /**
     * @brief 隐藏分区信号
     * @param deleteMessage：隐藏信息
     */
    Q_SCRIPTABLE void hidePartitionInfo(const QString &hideMessage);

    /**
     * @brief 显示分区信号
     * @param deleteMessage：显示信息
     */
    Q_SCRIPTABLE void showPartitionInfo(const QString &showMessage);

    /**
     * @brief USB刷新信号
     */
    Q_SCRIPTABLE void usbUpdated();

    /**
     * @brief 坏道检测检测信息信号
     * @param cylinderNumber：检测柱面号
     * @param cylinderTimeConsuming：柱面耗时
     * @param cylinderStatus：柱面状态
     * @param cylinderErrorInfo：柱面错误信息
     */
    Q_SCRIPTABLE void checkBadBlocksCountInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);

    /**
     * @brief 坏道检测完成信号
     */
    Q_SCRIPTABLE void checkBadBlocksFinished();

    /**
     * @brief 坏道修复完成信号
     */
    Q_SCRIPTABLE void fixBadBlocksFinished();

    /**
     * @brief 坏道修复信息信号
     * @param cylinderNumber：检测柱面号
     * @param cylinderStatus：柱面状态
     */
    Q_SCRIPTABLE void fixBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming);

public Q_SLOTS:
    /**
     * @brief 退出服务
     */
    Q_SCRIPTABLE void Quit();

    /**
    *@brief 启动服务
    */
    Q_SCRIPTABLE void Start(qint64 applicationPid);

    /**
     * @brief 获取设备信息
     * @return 返回设备信息表
     */
    Q_SCRIPTABLE DeviceInfo getDeviceinfo();

    /**
     * @brief 获取全部设备信息
     */
    Q_SCRIPTABLE void getalldevice();

    /**
     * @brief 获取页面选择分区信息
     * @param 分区信息
     */
    Q_SCRIPTABLE void setCurSelect(const PartitionInfo &info);

    /**
     * @brief 卸载
     */
    Q_SCRIPTABLE bool unmount();

    /**
     * @brief 挂载
     * @param 挂载点路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool mount(const QString &mountpath);

    /**
     * @brief 获取全部文件系统格式支持
     * @return 返回支持文件系统格式列表
     */
    Q_SCRIPTABLE QStringList getallsupportfs();

    /**
     * @brief 格式化分区
     * @param fstype：文件系统类型
     * @param name：分区别名
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool format(const QString &fstype, const QString &name = QString());

    /**
     * @brief 扩容分区
     * @param info：扩容分区信息
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool resize(const PartitionInfo &info);

    /**
     * @brief 创建分区
     * @param infovec：创建分区信息列表
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool create(const PartitionVec &infovec);

    /**
     * @brief 获取硬盘硬件信息
     * @param devicepath：设备信息路径
     * @return 返回硬盘信息表
     */
    Q_SCRIPTABLE HardDiskInfo onGetDeviceHardInfo(const QString &devicepath);

    /**
     * @brief 获取硬盘健康状态
     * @param devicepath：设备信息路径
     * @return 返回硬盘健康状态
     */
    Q_SCRIPTABLE QString onGetDeviceHardStatus(const QString &devicepath);

    /**
     * @brief 获取硬盘健康信息
     * @param devicepath：设备信息路径
     * @return 返回硬盘健康信息
     */
    Q_SCRIPTABLE HardDiskStatusInfoList onGetDeviceHardStatusInfo(const QString &devicepath);

    /**
     * @brief 删除分区
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool onDeletePartition();

    /**
     * @brief 隐藏分区
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool onHidePartition();

    /**
     * @brief 显示分区
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool onShowPartition();

    /**
     * @brief 分区表错误检测
     * @param devicepath：设备信息路径
     * @return true错误false正常
     */
    Q_SCRIPTABLE bool onDetectionPartitionTableError(const QString &devicePath);

    /**
     * @brief 坏道检测（检测次数）
     * @param devicePath：设备信息路径
     * @param blockStart：开始柱面
     * @param blockEnd：结束柱面
     * @param checkConut：检测次数
     * @param checkSize：检测柱面大小
     * @param flag：暂停，检测，继续标志
     * @return true错误false正常
     */
    Q_SCRIPTABLE bool onCheckBadBlocksCount(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize,int flag);

    /**
     * @brief 坏道检测（检测时间）
     * @param devicePath：设备信息路径
     * @param blockStart：开始柱面
     * @param blockEnd：结束柱面
     * @param checkTime: 检测超时时间
     * @param checkSize：检测柱面大小
     * @param flag：暂停，检测，继续标志
     * @return true错误false正常
     */
    Q_SCRIPTABLE bool onCheckBadBlocksTime(const QString &devicePath, int blockStart, int blockEnd,const QString &checkTime, int checkSize, int flag);

    /**
     * @brief 坏道修复
     * @param devicePath：设备信息路径
     * @param badBlocksList：坏道柱面集合
     * @param checkSize：检测柱面大小
     * @param flag：暂停，检测，继续标志
     * @return true错误false正常
     */
    Q_SCRIPTABLE bool onFixBadBlocks(const QString &devicePath, QStringList badBlocksList, int checkSize, int flag);

    /**
     * @brief USB插入
     */
    Q_SCRIPTABLE void updateUsb();

    /**
     * @brief USB拔出
     */
    Q_SCRIPTABLE void updateUsbRemove();

    /**
     * @brief 刷新按钮
     */
    Q_SCRIPTABLE void refreshFunc();

    /**
     * @brief 测试
     * @param 无
     */
    Q_SCRIPTABLE int test();


private:
    /**
     * @brief 初始化信号和槽函数
     * @param 无
     */
    void initConnection();

private:
    PartedCore *m_partedcore;  //磁盘操作类对象
};

} // namespace DiskManager
#endif // DISKMANAGERSERVICE_H
