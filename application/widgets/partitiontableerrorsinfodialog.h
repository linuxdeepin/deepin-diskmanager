/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitiontableerrorsinfodialog.h
 *
 * @brief 分区表错误报告，展示分区表具体错误信息
 *
 * @date 2020-08-21 13:15
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
#ifndef PARTITIONTABLEERRORSINFODIALOG_H
#define PARTITIONTABLEERRORSINFODIALOG_H

#include <DDialog>
#include <DLabel>
#include <DTableView>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class QStandardItemModel;
class PartitionTableErrorsInfoDelegate;
class DiskHealthHeaderView;

/**
 * @class PartitionTableErrorsInfoDialog
 * @brief 分区表错误报告类
 */

class PartitionTableErrorsInfoDialog : public DDialog
{
    Q_OBJECT
public:
    explicit PartitionTableErrorsInfoDialog(const QString &deviceInfo, QWidget *parent = nullptr);
    ~PartitionTableErrorsInfoDialog();

signals:

public slots:

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
    DLabel *m_Label; // 磁盘信息提示
    QString m_deviceInfo; // 当前磁盘
    DTableView *m_tableView; // 表格
    QStandardItemModel *m_standardItemModel; // 表格模型
    PartitionTableErrorsInfoDelegate *m_partitionTableErrorsInfoDelegatee; // 表格代理
    DPushButton *pushButton; // 确定按钮
    DiskHealthHeaderView *m_diskHealthHeaderView;
};

#endif // PARTITIONTABLEERRORSINFODIALOG_H
