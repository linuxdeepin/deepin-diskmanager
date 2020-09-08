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

public Q_SLOTS:
    /**
     * @brief 退出服务
     */
    Q_SCRIPTABLE void Quit();
    /**
    *@brief 启动服务
    */
    Q_SCRIPTABLE void Start();
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
     * @brief qdbus结构体传参测试
     * @return 测试结构体
     */
    Q_SCRIPTABLE stCustest interfacetest();
    /**
     * @brief 获取硬盘硬件信息
     * @param devicepath：设备信息路径
     * @return 返回硬盘信息表
     */
    Q_SCRIPTABLE HardDiskInfo onGetDeviceHardInfo(const QString &devicepath);
    //Q_SCRIPTABLE HardDiskInfo onGetDeviceHardInfo();
    /**
     * @brief 获取硬盘健康状态
     * @param devicepath：设备信息路径
     * @return 返回硬盘健康状态
     */
    Q_SCRIPTABLE QString onGetDeviceHardStatus(const QString &devicepath);
    //Q_SCRIPTABLE QString onGetDeviceHardStatus();
    /**
     * @brief 获取硬盘健康信息
     * @param devicepath：设备信息路径
     * @return 返回硬盘健康信息
     */
    Q_SCRIPTABLE HardDiskStatusInfoList onGetDeviceHardStatusInfo(const QString &devicepath);
    //Q_SCRIPTABLE HardDiskStatusInfoList onGetDeviceHardStatusInfo();
    /**
     * @brief 删除分区
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool onDeletePartition(const QString &devicepath, const QString &parttitionpath);
    /**
     * @brief 隐藏分区
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool onHidePartition(const QString &devicepath, const QString &parttitionpath);
    /**
     * @brief 显示分区
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return true成功false失败
     */
    Q_SCRIPTABLE bool onShowPartition(const QString &devicepath, const QString &parttitionpath);
    /**
     * @brief 获取隐藏分区是否隐藏属性
     * @param devicepath：设备信息路径
     * @param parttitionpath：分区路径
     * @return 1隐藏0没隐藏
     */
    Q_SCRIPTABLE int onGetPartitionHiddenFlag(const QString &devicePath, const QString &parttitionPath);
    /**
     * @brief 分区表错误检测
     * @param devicepath：设备信息路径
     * @return true错误false正常
     */
    Q_SCRIPTABLE bool onDetectionPartitionTableError(const QString &devicePath);
    /**
     * @brief USB插入
     */
    Q_SCRIPTABLE void updateUsb();
    /**
     * @brief USB拔出
     * @param 无
     */
    Q_SCRIPTABLE void updateUsbRemove();
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
