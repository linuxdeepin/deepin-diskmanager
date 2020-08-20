/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskhealthdetectiondialog.h
 *
 * @brief 磁盘健康检测页面展示类
 *
 * @date 2020-08-19 17:08
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
#ifndef DISKHEALTHDETECTIONDIALOG_H
#define DISKHEALTHDETECTIONDIALOG_H

#include <DDialog>
#include <DTableView>
#include <DCommandLinkButton>
#include <DLabel>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class QStandardItemModel;
class DiskStandardItemModel;
class DiskHealthDetectionDelegate;

/**
 * @class DiskHealthDetectionDialog
 * @brief 磁盘健康检测类
 */

class DiskHealthDetectionDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DiskHealthDetectionDialog(const QString &devicePath, QWidget *parent = nullptr);

signals:

public slots:

private slots:
    /**
     * @brief 导出按钮点击响应的槽函数
     */
    void onExportButtonClicked();

private:
    /**
     * @brief 初始化界面
     */
    void initUI();

    /**
     * @brief 初始化信号连接
     */
    void initConnections();

private:
    DTableView *m_tableView; // 表格
    QStandardItemModel *m_standardItemModel; // 数据模型
    DCommandLinkButton *m_linkButton; // 导出按钮
    DiskHealthDetectionDelegate *m_diskHealthDetectionDelegate; // 表格代理
    QString m_devicePath; // 磁盘路径
    DLabel *m_serialNumberValue; // 序列号值
    DLabel *m_userCapacityValue;// 用户容量值
    DLabel *m_healthStateValue; // 健康状态值
    DLabel *m_temperatureValue; // 温度值

};

#endif // DISKHEALTHDETECTIONDIALOG_H
