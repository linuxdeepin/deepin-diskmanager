/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file unmountdialog.h
 *
 * @brief 实现分区卸载
 *
 * @date 2020-09-17 09:12
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
#ifndef UNMOUNTDIALOG_H
#define UNMOUNTDIALOG_H

#include "customcontrol/ddbase.h"

DWIDGET_USE_NAMESPACE

/**
 * @class UnmountDialog
 * @brief 卸载类
 */

class UnmountDialog : public DDBase
{
    Q_OBJECT
public:
    explicit UnmountDialog(QWidget *parent = nullptr);

private:
    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

    /**
     * @brief 卸载当前选择的分区或者逻辑卷
     */
    void umountCurMountPoints();

private slots:
    /**
     * @brief 按钮点击响应的槽函数
     * @param index 当前点击按钮
     * @param text 当前点击按钮文本
     */
    void onButtonClicked(int index, const QString &text);
};

#endif // UNMOUNTDIALOG_H
