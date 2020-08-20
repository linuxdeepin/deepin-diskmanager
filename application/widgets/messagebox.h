/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file messagebox.h
 *
 * @brief 提示信息窗口类，主要实现各种样式的弹窗
 *
 * @date 2020-08-19 16:26
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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <DDialog>
#include <DProgressBar>

DWIDGET_USE_NAMESPACE

/**
 * @class MessageBox
 * @brief 弹窗消息
 */

class MessageBox : public DDialog
{
    Q_OBJECT
public:
    explicit MessageBox(DDialog *parent = nullptr);

    /**
     * @brief 设置警告窗口
     * @param title 标题，没有标题写空字符串
     * @param warningMsg 文本内容
     * @param sureBtnText 确定按钮
     * @param cancalBtnText="" 取消按钮，可不填
    */
    void setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, const QString &cancalBtnText = "");

    /**
     * @brief 设置警告窗口
     * @param title 标题，没有标题写空字符串
     * @param warningMsg 文本内容
     * @param sureBtnText 确定按钮
     * @param sureBtnType=ButtonNormal 确定按钮类型
     * @param cancalBtnText="" 取消按钮，可不填
    */
    void setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, ButtonType sureBtnType, const QString &cancalBtnText = "");

    /**
     * @brief 设置进度窗口
     * @param title 标题，没有标题写空字符串
     * @param cancalBtnText 取消按钮
    */
    void setProgressBar(const QString &title, const QString &cancalBtnText);

    /**
     * @brief 设置label
     * @param text label内容
    */
    void addLabel(QString text);

    /**
     * @brief 设置进度条
    */
    void addProgressBar();

signals:

public slots:

private:
    DProgressBar *m_progressBar; // 进度条
};

#endif // MESSAGEBOX_H
