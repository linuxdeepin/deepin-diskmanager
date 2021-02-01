/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file devicelistwidget.h
 *
 * @brief 主要实现磁盘树结构展示以及右键磁盘信息、硬盘健康检测、分区表错误处理、隐藏分区、显示分区、删除分区等操作
 *
 * @date 2020-09-03 14:55
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
#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include "customcontrol/dmtreeview.h"
#include "partedproxy/dmdbushandler.h"
#include <dirent.h>
#include <DWidget>
#include <QTextCodec>
#include <QByteArray>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

/**
 * @class DeviceListWidget
 * @brief 磁盘设备树列表类
 */

class DeviceListWidget : public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);
    ~DeviceListWidget();

    bool isHideSuccess = false;
    bool isShowSuccess = false;
    bool isDeleteSuccess = false;
    bool isUnmountSuccess = false;
private:

    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化信号连接
     */
    void initConnection();

    DmTreeview *m_treeView = nullptr;

signals:

public slots:

private slots:
    /**
     * @brief 更新设备信息信号响应的槽函数
     */
    void onUpdateDeviceInfo();

    /**
     * @brief 删除分区信号响应的槽函数
     * @param deleteMessage 执行结果
     */
    void onDeletePartition(const QString &deleteMessage);

    /**
     * @brief 隐藏分区信号响应的槽函数
     * @param hideMessage 执行结果
     */
    void onHidePartition(const QString &hideMessage);

    /**
     * @brief 显示分区信号响应的槽函数
     * @param showMessage 执行结果
     */
    void onShowPartition(const QString &showMessage);

    /**
     * @brief 卸载分区信号响应的槽函数
     * @param unmountMessage 执行结果
     */
    void onUnmountPartition(const QString &unmountMessage);

    /**
     * @brief usb热插拔信号响应的槽函数
     */
    void onUpdateUsb();

    /**
     * @brief 设备树右键菜单按钮
     * @param pos 当前点击树节点位置
     */
    void treeMenu(const QPoint &pos);

    /**
     * @brief 磁盘信息按钮点击响应的槽函数
     */
    void onDiskInfoClicked();

    /**
     * @brief 磁盘健康检测按钮点击响应的槽函数
     */
    void onDiskCheckHealthClicked();

    /**
     * @brief 分区表错误检测按钮点击响应的槽函数
     */
    void onPartitionErrorCheckClicked();

    /**
     * @brief 隐藏分区按钮点击响应的槽函数
     */
    void onHidePartitionClicked();

    /**
     * @brief 显示分区按钮点击响应的槽函数
     */
    void onShowPartitionClicked();

    /**
     * @brief 删除分区按钮点击响应的槽函数
     */
    void onDeletePartitionClicked();

    /**
     * @brief 磁盘坏道检测与修复按钮点击响应的槽函数
     */
    void onDiskBadSectorsClicked();

private:
    int m_flag = 0;
    int m_num = 0;
    int m_addItem = 0;
    QString m_devicePath;
    int m_deviceNum;
    DiskInfoData m_curDiskInfoData;
    QString m_curChooseDevicePath;
    //    DMDbusHandler *m_handler;
    //    DmDiskinfoBox *m_box = nullptr;
    //    DmDiskinfoBox *m_childbox = nullptr;
};

#endif // DEVICELISTWIDGET_H
