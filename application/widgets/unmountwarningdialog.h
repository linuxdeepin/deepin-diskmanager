/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file unmountwarningdialog.h
 *
 * @brief 系统盘挂载点卸载警告窗口
 *
 * @date 2022-03-17 16:38
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
#ifndef UNMOUNTWARNINGDIALOG_H
#define UNMOUNTWARNINGDIALOG_H

#include "customcontrol/ddbase.h"

#include <DCheckBox>
#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @class UnmountWarningDialog
 * @brief 卸载警告窗口类
 */

class UnmountWarningDialog : public DDBase
{
    Q_OBJECT
public:
    explicit UnmountWarningDialog(QWidget *parent = nullptr);

private slots:
    /**
     * @brief 复选框状态改变响应的槽函数
     * @param state 复选框当前状态
     */
    void onCheckBoxStateChange(int state);

private:
    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

private:
    DCheckBox *m_checkBox;
};

#endif // UNMOUNTWARNINGDIALOG_H
