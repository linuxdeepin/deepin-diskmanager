/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitiondialog.h
 *
 * @brief 是否创建分区选择窗口
 *
 * @date 2020-09-08 14:56
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
#ifndef PARTITIONDIALOG_H
#define PARTITIONDIALOG_H

#include "customcontrol/ddbase.h"

#include <QWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class PartitionDialog
 * @brief 是否进行分区确认窗口类
 */

class PartitionDialog : public DDBase
{
    Q_OBJECT
public:
    explicit PartitionDialog(QWidget *parent = nullptr);

private:
    /**
     * @brief 初始化界面
     */
    void initUi();
    //    void initConnection();
signals:
    //    void showPartWidget();
public slots:
    //    void sendSignal(int index);
};

#endif // PARTITIONDIALOG_H
