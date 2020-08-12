/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

class DeviceListWidget : public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);
    ~DeviceListWidget();

private:
    void initUi();
    void initConnection();
    void add();
    DmTreeview *m_treeview = nullptr;

signals:
    void sigPartitionSelectChanged();

public slots:
    void slotUpdateDeviceInfo();
    void slotDeletePartition(const QString &deleteMessage);
    void slotHidePartition(const QString &hideMessage);
    void slotShowPartition(const QString &showMessage);

    /**
     * @brief 设备树右键菜单按钮
     * @param pos 当前点击树节点位置
     */
    void treeMenu(const QPoint &pos);

    /**
     * @brief 设备树右键按钮响应的槽函数
     */
    void onTreeMenuClicked();

private:
    int flag = 0;
    int num = 0;
    int a = 0;
    int additem = 0;
    QString devicepath_;
    int devicenum_ ;
    DiskInfoData m_curDiskInfoData;
    //    DMDbusHandler *m_handler;
    //    DmDiskinfoBox *m_box = nullptr;
    //    DmDiskinfoBox *m_childbox = nullptr;
};

#endif // DEVICELISTWIDGET_H
