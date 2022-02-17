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
#include <DLabel>
#include <DPushButton>
#include <DWarningButton>
#include <DSpinner>
#include <DStackedWidget>

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

    enum WipeType {
        FAST = 1,
        SECURE,
        DOD,
        GUTMANN
    };

private:

    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

    DComboBox *m_formatComboBox;
    DLineEdit *m_fileNameEdit;
    DComboBox *m_securityComboBox;
    DComboBox *m_wipingMethodComboBox;
    DLabel *m_describeInfo;
    QWidget *m_wipingMethodWidget;
    DLabel *m_labelTmp;
    DLabel *m_label;
    DPushButton *m_pushButton;
    DWarningButton *m_warningButton;
    DStackedWidget *m_stackedWidget;
    DSpinner *m_spinner;
    DLabel *m_failLabel;
    DLabel *m_titleLabel;
    QString m_pathInfo;
    int m_curWipeMethod;
    QString m_curDiskMediaType;
    QHBoxLayout *m_buttonLayout;
signals:

private slots:

    /**
     * @brief 取消按钮点击槽函数
     */
    void onCancelButtonClicked();

    /**
     * @brief 擦除按钮点击槽函数
     */
    void onWipeButtonClicked();

    /**
     * @brief 当前文本改变
     * @param text 当前文本
     */
    void onTextChanged(const QString &text);

    /**
     * @brief 下拉框分区格式切换
     * @param text 当前选择文本
     */
    void onComboxFormatTextChange(const QString &text); //下拉框分区格式切换

    /**
     * @brief 下拉框安全选择切换
     * @param index 当前选择下标
     */
    void onSecurityCurrentIndexChanged(int index);

    /**
     * @brief 下拉框覆盖次数切换
     * @param index 当前选择下标
     */
    void onWipingMethodCurrentIndexChanged(int index);

    /**
     * @brief 擦除结果显示
     * @param info 擦除结果信息
     */
    void onWipeResult(const QString &info);
};

#endif // FORMATEDIALOG_H
