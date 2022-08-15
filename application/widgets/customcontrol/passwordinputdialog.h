/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file passwordinputdialog.h
 *
 * @brief 密码输入窗口
 *
 * @date 2022-05-11 10:01
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
#ifndef PASSWORDINPUTDIALOG_H
#define PASSWORDINPUTDIALOG_H

#include "ddbase.h"

#include <DPasswordEdit>
#include <DTextEdit>
#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @class PasswordInputDialog
 * @brief 密码输入窗口类
 */

class PasswordInputDialog : public DDBase
{
    Q_OBJECT
public:
    explicit PasswordInputDialog(QWidget *parent = nullptr);

    /**
     * @brief 设置设备名称
     * @param devName 设备名称
     */
    void setDeviceName(const QString &devName);

    /**
     * @brief 设置标题
     * @param text 标题
     */
    void setTitleText(const QString &text);

    /**
     * @brief 获取密码
     * @return 输入密码
     */
    QString getPassword();

    /**
     * @brief 获取密码提示
     * @return 密码提示
     */
    QString getPasswordHint();

signals:

private slots:
    /**
     * @brief 输入密码框文本改变响应的槽函数
     */
    void onInputPasswordEditTextChanged(const QString &);

    /**
     * @brief 验证密码框文本改变响应的槽函数
     */
    void onCheckPasswordEditTextChanged(const QString &);

    /**
     * @brief 密码提示文本改变响应的槽函数
     */
    void passwordHintTextChanged();

    /**
     * @brief 按钮点击响应的槽函数
     * @param index 当前点击按钮
     * @param text 当前点击按钮文本
     */
    void onButtonClicked(int index, const QString &text);

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
    DPasswordEdit *m_inputPasswordEdit;
    DPasswordEdit *m_checkPasswordEdit;
    DTextEdit *m_textEdit;
};

#endif // PASSWORDINPUTDIALOG_H
