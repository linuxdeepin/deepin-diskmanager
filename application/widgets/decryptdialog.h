/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file decryptdialog.h
 *
 * @brief 解密窗口
 *
 * @date 2022-05-19 10:44
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
#ifndef DECRYPTDIALOG_H
#define DECRYPTDIALOG_H

#include "customcontrol/ddbase.h"
#include "customcontrol/waterloadingwidget.h"
#include "partedproxy/dmdbushandler.h"

#include <DPasswordEdit>
#include <DPushButton>
#include <DLabel>
#include <DTextEdit>
#include <DStackedWidget>
#include <DSuggestButton>

DWIDGET_USE_NAMESPACE

/**
 * @class DecryptDialog
 * @brief 解密窗口类
 */

class DecryptDialog : public DDBase
{
    Q_OBJECT
public:
    explicit DecryptDialog(QWidget *parent = nullptr);

signals:

protected:
    /**
     * @brief 键盘按下事件
     * @param event事件
     */
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    /**
     * @brief 输入密码框文本改变响应的槽函数
     */
    void onPasswordEditTextChanged(const QString &);

    /**
     * @brief 密码提示按钮点击响应的槽函数
     */
    void onPasswordHintButtonClicked();

    /**
     * @brief 取消按钮点击响应的槽函数
     */
    void onCancelButtonClicked();

    /**
     * @brief 解锁按钮点击响应的槽函数
     */
    void onButtonClicked();
    
    /**
     * @brief 解锁信号响应的槽函数
     * @param luks 解锁信息
     */
    void onDecryptMessage(const LUKS_INFO &luks);

    /**
     * @brief 定时器超时信号响应的槽函数
     */
    void onTimeOut();

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
     * @brief 初始化数据
     */
    void initData();

    /**
     * @brief 更新数据
     * @param baseMinute 基础时间
     * @param minutes 已过时间
     * @param message 警告信息
     */
    void updateData(const int &baseMinute, const int &minutes, const QString &message);

    /**
     * @brief 设置密码输入框
     * @param isAlert 是否设置为警告模式
     * @param message 警告信息
     * @param isDisabled 是否可用
     */
    void setPasswordEditStatus(const bool &isDisabled, const bool &isAlert, const QString &message = "");

private:
    DPasswordEdit *m_passwordEdit;
    DPushButton *m_pushButton;
    DLabel *m_passwordHint;
    DFrame *m_frame;
    LUKS_INFO m_luksInfo;
    DStackedWidget *m_stackedWidget;
    DPushButton *m_cancelButton;
    DSuggestButton *m_decryptButton;
    WaterLoadingWidget *m_waterLoadingWidget;
    int m_height;
    QString m_devName;
    QString m_devPath;
    QTimer m_timer;
    QString m_titleText;
};

#endif // DECRYPTDIALOG_H
