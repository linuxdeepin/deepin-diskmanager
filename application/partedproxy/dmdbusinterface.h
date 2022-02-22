/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmdbusinterface.h
 *
 * @brief 服务接口类
 *
 * @date 2020-09-18 15:24
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
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

#ifndef DMDBUSINTERFACE_H
#define DMDBUSINTERFACE_H

#include "deviceinfo.h"

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/**
 * @class DMDBusInterface
 * @brief 服务接口类
 */

/*
 * Proxy class for interface com.deepin.diskmanager
 */
class DMDBusInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:

    /**
     * @brief 获取dbus名称
     */
    static inline const char *staticInterfaceName()
    {
        return "com.deepin.diskmanager";
    }

public:
    DMDBusInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~DMDBusInterface();

public Q_SLOTS: // METHODS

    /**
     * @brief 退出服务
     */
    inline QDBusPendingReply<> Quit()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Quit"), argumentList);
    }

//    /**
//     * @brief 启动服务
//     */
//    inline QDBusPendingReply<> Start(qint64 applicationPid)
//    {
//        QList<QVariant> argumentList;
//        argumentList << QVariant::fromValue(applicationPid);
//        return asyncCallWithArgumentList(QStringLiteral("Start"), argumentList);
//    }
    /**
     * @brief 启动服务
     */
    inline QDBusPendingReply<> Start()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Start"), argumentList);
    }


    /**
     * @brief  刷新
     */
    inline QDBusPendingReply<> refreshFunc()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("refreshFunc"), argumentList);
    }

    /**
     * @brief 获取设备信息
     */
    inline QDBusPendingReply<DeviceInfo> getDeviceinfo()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getDeviceinfo"), argumentList);
    }

    /**
     * @brief 获取所有设备
     */
    inline QDBusPendingReply<> getalldevice()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getalldevice"), argumentList);
    }

    /**
     * @brief 设置当前选择分区
     * @param info 分区信息
     */
    inline QDBusPendingReply<> setCurSelect(const PartitionInfo &info)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(info);
        return asyncCallWithArgumentList(QStringLiteral("setCurSelect"), argumentList);
    }

    /**
     * @brief 挂载
     * @param mountPath 挂载路径
     */
    inline QDBusPendingReply<bool> mount(const QString &mountPath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(mountPath);
        return asyncCallWithArgumentList(QStringLiteral("mount"), argumentList);
    }

    /**
     * @brief 卸载
     */
    inline QDBusPendingReply<bool> unmount()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("unmount"), argumentList);
    }

    /**
     * @brief 获取所有文件系统支持
     */
    inline QDBusPendingReply<QStringList> getallsupportfs()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("getallsupportfs"), argumentList);
    }

    /**
     * @brief 格式化
     * @param type 分区类型
     * @param name 分区名称
     */
    inline QDBusPendingReply<bool> format(const QString &type, const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(type) << QVariant::fromValue(name);
        return asyncCallWithArgumentList(QStringLiteral("format"), argumentList);
    }

    /**
    * @brief 擦除
    * @param type 分区类型
    * @param path 磁盘或分区路径
    * @param name 劵标名
    * @param user 当前用户名
    * @param diskType 磁盘或者分区类型 0是分区 1是磁盘
    * @param clearType 擦除标准 1为快速 2安全 3为安全擦除7次 4为安全擦除35次
    */
    inline QDBusPendingReply<bool> clear(const QString &fstype, const QString &path, const QString &name, const QString &user, const int &diskType, const int &clearType)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(fstype) << QVariant::fromValue(path) << QVariant::fromValue(name) << QVariant::fromValue(user) << QVariant::fromValue(diskType) << QVariant::fromValue(clearType);
        return asyncCallWithArgumentList(QStringLiteral("clear"), argumentList);
    }

    /**
     * @brief 空间调整
     * @param info 分区信息
     */
    inline QDBusPendingReply<bool> resize(const PartitionInfo &info)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(info);
        return asyncCallWithArgumentList(QStringLiteral("resize"), argumentList);
    }

    /**
     * @brief 创建分区
     * @param infovec 分区信息
     */
    inline QDBusPendingReply<bool> create(const PartitionVec &infovec)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(infovec);
        return asyncCallWithArgumentList(QStringLiteral("create"), argumentList);
    }

    /**
     * @brief 获取磁盘健康状态
     * @param devicePath 磁盘路径
     */
    inline QDBusPendingReply<QString> onGetDeviceHardStatus(const QString &devicePath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath);
        return asyncCallWithArgumentList(QStringLiteral("onGetDeviceHardStatus"), argumentList);
    }

    /**
     * @brief 获取磁盘信息
     * @param devicePath 磁盘路径
     */
    inline QDBusPendingReply<HardDiskInfo> onGetDeviceHardInfo(const QString &devicePath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath);
        return asyncCallWithArgumentList(QStringLiteral("onGetDeviceHardInfo"), argumentList);
    }

    /**
     * @brief 获取磁盘检测信息
     * @param devicePath 磁盘路径
     */
    inline QDBusPendingReply<HardDiskStatusInfoList> onGetDeviceHardStatusInfo(const QString &devicePath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath);
        return asyncCallWithArgumentList(QStringLiteral("onGetDeviceHardStatusInfo"), argumentList);
    }

    /**
     * @brief 删除分区
     */
    inline QDBusPendingReply<bool> onDeletePartition()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("onDeletePartition"), argumentList);
    }

    /**
     * @brief 隐藏分区
     */
    inline QDBusPendingReply<bool> onHidePartition()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("onHidePartition"), argumentList);
    }

    /**
     * @brief 显示分区
     */
    inline QDBusPendingReply<bool> onShowPartition()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("onShowPartition"), argumentList);
    }

    /**
     * @brief 分区表错误检测
     * @param devicePath 磁盘路径
     */
    inline QDBusPendingReply<bool> onDetectionPartitionTableError(const QString &devicePath)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath);
        return asyncCallWithArgumentList(QStringLiteral("onDetectionPartitionTableError"), argumentList);
    }

    /**
     * @brief 创建分区表
     * @param devicepath：设备信息路径
     * @param length:设备大小
     * @param sectorSize:扇区大小
     * @param diskLabel:分区表格式
     * @return true成功，false失败
     */
    inline QDBusPendingReply<bool> onCreatePartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath) << QVariant::fromValue(length) << QVariant::fromValue(sectorSize) << QVariant::fromValue(diskLabel);
        return asyncCallWithArgumentList(QStringLiteral("onCreatePartitionTable"), argumentList);
    }

    /**
     * @brief 坏道检测(次数)
     * @param devicePath 磁盘路径
     * @param blockStart 检测开始
     * @param blockEnd 检测结束
     * @param checkConut 检测次数
     * @param checkSize 检测柱面大小
     * @param flag：检测状态(检测，停止，继续)
     */
    inline QDBusPendingReply<bool> onCheckBadBlocksCount(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize, int flag)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath) << QVariant::fromValue(blockStart) << QVariant::fromValue(blockEnd) << QVariant::fromValue(checkConut) << QVariant::fromValue(checkSize) << QVariant::fromValue(flag);
        return asyncCallWithArgumentList(QStringLiteral("onCheckBadBlocksCount"), argumentList);
    }

    /**
     * @brief 坏道检测(时间)
     * @param devicePath 磁盘路径
     * @param blockStart 检测开始
     * @param blockEnd 检测结束
     * @param checkTime 检测时间
     * @param checkSize 检测柱面大小
     * @param flag：检测状态(检测，停止，继续)
     */
    inline QDBusPendingReply<bool> onCheckBadBlocksTime(const QString &devicePath, int blockStart, int blockEnd, const QString &checkTime, int checkSize, int flag)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath) << QVariant::fromValue(blockStart) << QVariant::fromValue(blockEnd) << QVariant::fromValue(checkTime) << QVariant::fromValue(checkSize) << QVariant::fromValue(flag);
        return asyncCallWithArgumentList(QStringLiteral("onCheckBadBlocksTime"), argumentList);
    }

    /**
     * @brief 坏道修复
     * @param devicePath 磁盘路径
     * @param badBlocksList 修复柱面合集
     * @param checkSize 检测柱面大小
     * @param flag：检测状态(检测，停止，继续)
     */
    inline QDBusPendingReply<bool> onFixBadBlocks(const QString &devicePath, QStringList badBlocksList, int checkSize, int flag)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(devicePath) << QVariant::fromValue(badBlocksList) << QVariant::fromValue(checkSize) << QVariant::fromValue(flag);
        return asyncCallWithArgumentList(QStringLiteral("onFixBadBlocks"), argumentList);
    }

    /**
     * @brief 创建vg
     * @param vgName:待创建vg名称
     * @param devList: pv设备集合
     * @param size:vg总大小
     */
    inline QDBusPendingReply<bool> onCreateVG(QString vgName, QList<PVData>devList, long long size)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(vgName) << QVariant::fromValue(devList) << QVariant::fromValue(size);
        return asyncCallWithArgumentList(QStringLiteral("onCreateVG"), argumentList);
    }

    /**
     * @brief 创建lv
     * @param vgName:vg名称
     * @param lvList: 待创建lv列表
     */
    inline QDBusPendingReply<bool> onCreateLV(QString vgName, QList<LVAction>lvList)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(vgName) << QVariant::fromValue(lvList);
        return asyncCallWithArgumentList(QStringLiteral("onCreateLV"), argumentList);
    }

    /**
     * @brief 删除vg
     * @param vglist: 待删除vg列表
     */
    inline QDBusPendingReply<bool> onDeleteVG(QStringList vglist)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(vglist) ;
        return asyncCallWithArgumentList(QStringLiteral("onDeleteVG"), argumentList);
    }

    /**
     * @brief 删除lv
     * @param lvlist: 待删除lv列表

     */
    inline QDBusPendingReply<bool> onDeleteLV(QStringList lvlist)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(lvlist);
        return asyncCallWithArgumentList(QStringLiteral("onDeleteLV"), argumentList);
    }

    /**
     * @brief vg空间调整
     * @param vgName:vg名称
     * @param devList: pv设备集合
     * @param size:调整后vg总大小
     */
    inline QDBusPendingReply<bool> onResizeVG(QString vgName, QList<PVData>devList, long long size)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(vgName) << QVariant::fromValue(devList) << QVariant::fromValue(size);
        return asyncCallWithArgumentList(QStringLiteral("onResizeVG"), argumentList);
    }


    /**
     * @brief lv空间调整
     * @param act:操作lv结构体
     */
    inline QDBusPendingReply<bool> onResizeLV(LVAction act)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(act);
        return asyncCallWithArgumentList(QStringLiteral("onResizeLV"), argumentList);
    }


Q_SIGNALS: // SIGNALS
    Q_SCRIPTABLE void MessageReport(const QString &msg);
    Q_SCRIPTABLE void updateDeviceInfo(const DeviceInfoMap &infomap, const LVMInfo &lvmInfo);
    Q_SCRIPTABLE void deletePartition(const QString &deleteMessage);
    Q_SCRIPTABLE void hidePartitionInfo(const QString &hideMessage);
    Q_SCRIPTABLE void showPartitionInfo(const QString &showMessage);
    Q_SCRIPTABLE void usbUpdated();
    Q_SCRIPTABLE void checkBadBlocksCountInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);
    Q_SCRIPTABLE void checkBadBlocksDeviceStatusError();
    Q_SCRIPTABLE void fixBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming);
    Q_SCRIPTABLE void checkBadBlocksFinished();
    Q_SCRIPTABLE void fixBadBlocksFinished();
//    Q_SCRIPTABLE void rootLogin(const QString &loginMessage);
    Q_SCRIPTABLE void unmountPartition(const QString &unmountMessage);
    Q_SCRIPTABLE void createTableMessage(const bool &flag);
    Q_SCRIPTABLE void clearMessage(const QString &clearMessage);
};

namespace com {
namespace deepin {
typedef ::DMDBusInterface diskmanager;
}
} // namespace com
#endif
