/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file formatedialog.h
 *
 * @brief 格式化窗口类
 *
 * @date 2020-09-10 17:22
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
#ifndef FORMATEDIALOG_H
#define FORMATEDIALOG_H

#include "customcontrol/ddbase.h"

#include <DComboBox>
#include <DLineEdit>

DWIDGET_USE_NAMESPACE

/**
 * @class FormateDialog
 * @brief 格式化窗口类
 */

class FormateDialog : public DDBase
{
    Q_OBJECT
public:
    explicit FormateDialog(QWidget *parent = nullptr);

private:

    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

    DComboBox *m_formatComboBox = nullptr;
    DLineEdit *m_fileNameEdit = nullptr;
signals:

public slots:

    /**
     * @brief 初始化连接
     * @param index 所点击按钮标志
     * @param text 所点击按钮文本
     */
    void onButtonClicked(int index, const QString &text);
};

#endif // FORMATEDIALOG_H
