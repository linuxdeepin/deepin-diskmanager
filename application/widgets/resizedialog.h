/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file resizedialog.h
 *
 * @brief 实现分区空间调整
 *
 * @date 2020-09-17 09:21
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
#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include "customcontrol/ddbase.h"

#include <DLabel>
#include <DLineEdit>
#include <DComboBox>

DWIDGET_USE_NAMESPACE

/**
 * @class ResizeDialog
 * @brief 空间调整类
 */

class ResizeDialog : public DDBase
{
    Q_OBJECT
public:
    ResizeDialog(QWidget *parent = nullptr);

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
     * @brief 分区空间调整
     */
    void partitionResize();

    /**
     * @brief 逻辑卷空间调整
     */
    void lvResize();

    /**
     * @brief 不支持文件系统调整提示窗口
     */
    void noSupportFSDailog();

    DLineEdit *m_lineEdit;
    DComboBox *m_comboBox;
    DLabel *m_label;

private slots:
    /**
     * @brief 按钮点击响应的槽函数
     * @param index 当前点击按钮
     * @param text 当前点击按钮文本
     */
    void onButtonClicked(int index, const QString &text);

    /**
     * @brief 下拉框当前文本改变响应的槽函数
     * @param index 当前选择文本
     */
    void onComboSelectedChanged(int index);

    /**
     * @brief 输入框文本改变响应的槽函数
     */
    void onEditTextChanged(const QString &);
};

#endif // RESIZEDIALOG_H
