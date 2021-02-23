/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmdbushandler.h
 *
 * @brief 数据接口类，通过dbus与后端服务进行通信
 *
 * @date 2020-09-03 16:46
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
#ifndef DMDBUSHANDLER_H
#define DMDBUSHANDLER_H

#include "dmdbusinterface.h"

#include <QObject>
#include <QDebug>
#include <QDBusConnection>

/**
 * @class DMDbusHandler
 * @brief 数据接口类
 */

class DMDbusHandler : public QObject
{
    Q_OBJECT
public:
    static DMDbusHandler *instance(QObject *parent = nullptr);
    ~DMDbusHandler();

    /**
     * @brief 开启服务
     */
    void startService();

    /**
     * @brief 关闭服务
     */
    void Quit();

    /**
     * @brief 刷新
     */
    void refresh();

    /**
     * @brief 获取登录结果
     * @return 返回登录认证结果
     */
    QString getRootLoginResult();

    /**
     * @brief 给服务发获取所有设备信息的消息
     */
    void getDeviceInfo();

    /**
     * @brief 获取所有设备信息
     */
    const DeviceInfoMap &probDeviceInfo() const;

    /**
     * @brief 根据设备路径获取该设备的所有分区信息
     */
    PartitionVec getCurDevicePartitionArr();

    /**
     * @brief 获取当前分区信息
     */
    const PartitionInfo &getCurPartititonInfo();

    /**
     * @brief 根据设备路径获取该设备的所有信息
     */
    const DeviceInfo &getCurDeviceInfo();

    /**
     * @brief 根据设备路径获取该设备的扇区长度
     */
    const Sector &getCurDeviceInfoLength();

    /**
     * @brief 挂载分区
     * @param mountPath 挂载路径
     */
    void mount(const QString &mountPath);

    /**
     * @brief 卸载分区
     */
    void unmount();

    /**
     * @brief 获取所有文件系统支持
     */
    QStringList getAllSupportFileSystem();

    /**
     * @brief 格式化分区
     * @param fstype 分区格式
     * @param name 分区名称
     */
    void format(const QString &fstype, const QString &name = QString());

    /**
     * @brief 空间调整
     * @param info 当前分区信息
     */
    void resize(const PartitionInfo &info);

    /**
     * @brief 创建分区
     * @param infovec 将要创建的分区列表
     */
    void create(const PartitionVec &infovec);

    /**
     * @brief 获取磁盘基本信息
     * @param devicePath 磁盘路径
     * @return 返回磁盘信息
     */
    HardDiskInfo getHardDiskInfo(const QString &devicePath);

    /**
     * @brief 获取磁盘健康状态
     * @param devicePath 磁盘路径
     * @return 返回磁盘健康状态
     */
    QString getDeviceHardStatus(const QString &devicePath);

    /**
     * @brief 获取磁盘健康检测信息
     * @param devicePath 磁盘路径
     * @return 返回磁盘健康检测信息
     */
    HardDiskStatusInfoList getDeviceHardStatusInfo(const QString &devicePath);

    /**
     * @brief 删除分区
     */
    void deletePartition();

    /**
     * @brief 隐藏分区
     */
    void hidePartition();

    /**
     * @brief 显示分区
     */
    void unhidePartition();

    /**
     * @brief 分区表错误检测
     * @param devicePath 磁盘路径
     * @return 返回true检测错误，0检测正常
     */
    bool detectionPartitionTableError(const QString &devicePath);

    /**
     * @brief 获取磁盘是否存在空闲空间
     * @return 返回所有磁盘是否存在空闲空间信息
     */
    QMap<QString, QString> getIsExistUnallocated();

    /**
     * @brief 获取所有的磁盘名称
     * @return 返回磁盘名称列表
     */
    QStringList getDeviceNameList();

    /**
     * @brief 坏道检测
     * @param devicePath 磁盘路径
     * @param blockStart 检测开始
     * @param blockEnd 检测结束
     * @param checkConut 检测次数
     * @param checkSize 检测柱面大小
     * @param flag：检测状态(检测: 1，停止: 2，继续: 3)
     */
    void checkBadSectors(const QString &devicePath, int blockStart, int blockEnd, int checkNumber, int checkSize, int flag);

    /**
     * @brief 坏道修复
     * @param devicePath 磁盘路径
     * @param badBlocksList 坏道列表
     * @param repairSize 修复柱面大小
     * @param flag：修复状态(修复: 1，停止: 2，继续: 3)
     */
    void repairBadBlocks(const QString &devicePath, QStringList badBlocksList, int repairSize, int flag);

    /**
     * @brief Root认证消息
     * @param loginMessage 登录消息（1：成功，0：失败）
     */
    void onRootLogin(const QString &loginMessage);

    /**
     * @brief 创建分区表
     * @param devicepath：设备信息路径
     * @param length:设备大小
     * @param sectorSize:扇区大小
     * @param diskLabel:分区表格式
     * @return true：成功，false：失败
     */
    void createPartitionTable(const QString &devicePath, const QString &length, const QString &sectorSize, const QString &diskLabel);

private:
    explicit DMDbusHandler(QObject *parent = nullptr);

    /**
     * @brief 初始化连接
     */
    void initConnection();

signals:
    void showSpinerWindow(bool);
    void updateDeviceInfo();
    void curSelectChanged();
    void deletePartitionMessage(const QString &deleteMessage);
    void hidePartitionMessage(const QString &hideMessage);
    void showPartitionMessage(const QString &showMessage);
    void unmountPartitionMessage(const QString &unmountMessage);
    void createPartitionTableMessage(const bool &flag);
    void updateUsb();
    void checkBadBlocksCountInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);
//    void checkBadBlocksDeviceStatusError();
    void repairBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming);
    void checkBadBlocksFinished();
    void fixBadBlocksFinished();
    void rootLogin();

public slots:
    /**
     * @brief 树结构当前选择分区节点信号响应的槽函数
     * @param devicePath 磁盘路径
     * @param partitionPath 分区路径
     * @param start 分区扇区开始位置
     * @param end 分区扇区结束位置
     */
    void onSetCurSelect(const QString &devicePath, const QString &partitionPath, Sector start, Sector end);

private slots:

    /**
     * @brief 消息报告信号响应的槽函数
     * @param msg 消息报告信息
     */
    void onMessageReport(const QString &msg);

    /**
     * @brief 数据更新信号响应的槽函数
     * @param infoMap 设备信息
     */
    void onUpdateDeviceInfo(const DeviceInfoMap &infoMap);

    /**
     * @brief 接收卸载分区返回执行结果的槽函数
     * @param unmountMessage 执行结果
     */
    void onUnmountPartition(const QString &unmountMessage);

    /**
     * @brief 接收删除分区返回执行结果的槽函数
     * @param deleteMessage 执行结果
     */
    void onDeletePartition(const QString &deleteMessage);

    /**
     * @brief 接收隐藏分区返回执行结果的槽函数
     * @param hideMessage 执行结果
     */
    void onHidePartition(const QString &hideMessage);

    /**
     * @brief 接收显示分区返回执行结果的槽函数
     * @param showMessage 执行结果
     */
    void onShowPartition(const QString &showMessage);

    /**
     * @brief 接收新建分区表返回执行结果的槽函数
     * @param flag 执行结果
     */
    void onCreatePartitionTable(const bool &flag);

    /**
     * @brief 接收USB插拔信号的槽函数
     */
    void onUpdateUsb();

private:
    DMDBusInterface *m_dbus = nullptr;
    static DMDbusHandler *m_staticHandeler;
    QString m_curDevicePath;
    QString m_curPartitionPath;
    DeviceInfoMap m_deviceMap;
    PartitionInfo m_curPartitionInfo;
    QStringList m_supportFileSystem;
    HardDiskInfo m_hardDiskInfo;
    QString m_deviceHardStatus;
    HardDiskStatusInfoList m_hardDiskStatusInfoList;
    int m_partitionHiddenFlag;
    bool m_partitionTableError;
    QMap<QString, QString> m_isExistUnallocated;
    QStringList m_deviceNameList;
    QString m_loginMessage;
};

#endif // DMDBUSHANDLER_H
