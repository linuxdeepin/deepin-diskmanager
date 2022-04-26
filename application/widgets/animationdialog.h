/**
 * @copyright 2020-2021 Uniontech Technology Co., Ltd.
 *
 * @file animationdialog.h
 *
 * @brief 动画窗口类
 *
 * @date 2021-12-10 16:09
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
#ifndef ANIMATIONDIALOG_H
#define ANIMATIONDIALOG_H

#include "customcontrol/ddbase.h"

#include <DSpinner>
#include <DLabel>

#include <QWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class AnimationDialog
 * @brief 动画窗口类
 */

class AnimationDialog : public DDialog
{
    Q_OBJECT
public:
    explicit AnimationDialog(QWidget *parent = nullptr);

    /**
     * @brief 设置动画是否可见
     * @param isShow ture显示，false不显示
     * @param title 提示文本
     */
    void setShowSpinner(bool isShow, const QString &title = "");

signals:

public slots:

protected:
    /**
     * @brief 键盘按下事件
     * @param event事件
     */
    void keyPressEvent(QKeyEvent *event) override;

private:

    /**
     * @brief 初始化界面
     */
    void initUi();

    DSpinner *m_spinner;
    DLabel *m_label;
};

#endif // ANIMATIONDIALOG_H
