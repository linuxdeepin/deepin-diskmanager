/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file thread.h
 *
 * @brief 坏道检测修复线程类
 *
 * @date 2021-01-04 13:56
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
#ifndef THREAD_H
#define THREAD_H

#include "sigtype.h"
#include "device.h"
#include "deviceinfo.h"
#include <QObject>
#include <parted/parted.h>
#include <parted/device.h>

namespace DiskManager {

/**
 * @class ProbeThread
 * @brief 硬件信息刷新类
 */
class ProbeThread :public QObject
{
    Q_OBJECT
public:
    ProbeThread(QObject *parent = nullptr);

    /**
     * @brief 设置要发送的信号标识及相应的信号参数
     * @param type：信号标识，表示将发送哪种信号给主线程
     * @param arg1：信号可能需要的bool型参数
     * @param arg2：信号可能需要的QString型参数
     */
    void setSignal(int type = 0, bool arg1 = true, QString arg2 = "");

    /**
     * @brief 根据setSignal设置的信息发送相应的信号
     */
    void sendsignals();
public slots:
    /**
     * @brief 刷新硬件信息
     */
    void probeDeviceInfo();

    /**
     * @brief 返回硬件信息
     */
    QMap<QString, Device> getDeviceMap();
signals:
    /**
     * @brief 更新硬件信息信号
     * @param 要同步的硬件信息
     */
    void updateDeviceInfo(/*const QMap<QString, Device> m_deviceMap, */const DeviceInfoMap infomap);

    /**
     * @brief 发送卸载分区信号
     * @param 卸载信息
     */
    void unmountPartition(const QString &m);

    /**
     * @brief 发送删除分区信号
     * @param 删除结果
     */
    void deletePartitionMessage(const QString &m);

    /**
     * @brief 显示分区信号
     * @param 分区信息
     */
    void showPartitionInfo(const QString &m);

    /**
     * @brief 创建分区表信
     * @param 创建结果
     */
    void createTableMessage(const bool &flag);

    /**
     * @brief USB设备刷新信号
     */
    void usbUpdated();
private:
    QMap<QString, Device> m_deviceMap; //设备对应信息表
    DeviceInfoMap m_inforesult;        //全部设备分区信息
    int m_type;                        //刷新结束后需要发送的信号类型
    bool m_arg1;                       //需要发送的信号bool类型参数
    QString m_arg2;                    //需要发送的信号QString类型参数
};

/**
 * @class WorkThread
 * @brief 坏道检测类
 */
class WorkThread :public QObject
{
     Q_OBJECT
public:
    WorkThread(QObject *parent = nullptr);

    /**
     * @brief 设置检测次数信息
     * @param devicePath：设备路径
     * @param blockStart：开始柱面信息号
     * @param blockEnd：检测结束柱面号
     * @param checkConut：检测次数
     * @param checkSize：检测柱面范围大小
     */
    void setCountInfo(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize);

    /**
     * @brief 设置检测超时时间信息
     * @param devicePath：设备路径
     * @param blockStart：开始柱面信息号
     * @param blockEnd：检测结束柱面号
     * @param checkTime：检测超时时间
     * @param checkSize：检测柱面范围大小
     */
    void setTimeInfo(const QString &devicePath, int blockStart, int blockEnd, QString checkTime, int checkSize);

    /**
     * @brief 设置修复数据
     * @param devicePath：设备路径
     * @param list: 修复柱面集合
     * @param checkSize：检测柱面范围大小
     */
    void setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize);

    /**
     * @brief 设置停止状态
     * @param flag：停止状态
     */
    void setStopFlag(int flag);

public slots:

    /**
     * @brief 坏道检测线程(检测次数方式)
     */
    void runCount();

    /**
     * @brief 坏道检测线程(超时时间方式)
     */
    void runTime();

signals:

    /**
     * @brief 坏道检测检测信息信号(次数检测)
     * @param cylinderNumber：检测柱面号
     * @param cylinderTimeConsuming：柱面耗时
     * @param cylinderStatus：柱面状态
     * @param cylinderErrorInfo：柱面错误信息
     */
    void checkBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);

    /**
     * @brief 坏道检测完成信号
     */
    void checkBadBlocksFinished();

private:
    QString m_devicePath;   //设备路径
    int m_blockStart;       //开始检测柱面号
    int m_blockEnd;         //检测结束柱面号
    int m_checkConut;       //检测次数
    int m_checkSize;        //检测柱面大小
    QString m_checkTime;    //检测超时时间
    int m_stopFlag;         //暂停状态
};

/**
 * @class fixthread
 * @brief 坏道修复类
 */
class FixThread :public QObject
{
    Q_OBJECT
public:
    FixThread(QObject *parent = nullptr);

    /**
     * @brief 设置停止状态
     * @param flag：停止状态
     */
    void setStopFlag(int flag);

    /**
     * @brief 设置修复数据
     * @param devicePath：设备路径
     * @param list: 修复柱面集合
     * @param checkSize：检测柱面范围大小
     */
    void setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize);

public slots:
    /**
     * @brief 坏道修复线程
     */
    void runFix();

signals:

    /**
     * @brief 坏道修复信息信号
     * @param cylinderNumber：检测柱面号
     * @param cylinderStatus：柱面状态
     */
    void fixBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming);


    /**
     * @brief 坏道修复完成信号
     */
    void fixBadBlocksFinished();

private:
    QString m_devicePath;   //设备路径
    int m_stopFlag;         //暂停状态
    QStringList m_list;     //需要修复柱面集合
    int m_checkSize;        //检测柱面大小
};
}
#endif // THREAD_H
