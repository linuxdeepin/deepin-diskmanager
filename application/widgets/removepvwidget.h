/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file removepvwidget.h
 *
 * @brief 删除PV窗口
 *
 * @date 2022-03-11 09:42
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
#ifndef REMOVEPVWIDGET_H
#define REMOVEPVWIDGET_H

#include "customcontrol/ddbase.h"
#include "partedproxy/dmdbushandler.h"
#include "customcontrol/waterloadingwidget.h"

#include <DLabel>
#include <DPushButton>
#include <DWarningButton>
#include <DStackedWidget>

#include <QWidget>

#include <set>
using std::set;

DWIDGET_USE_NAMESPACE

class RemovePVWidget : public DDBase
{
    Q_OBJECT
public:
    explicit RemovePVWidget(QWidget *parent = nullptr);

signals:

private slots:
    /**
     * @brief 取消按钮点击响应的槽函数
     */
    void onCancelButtonClicked();

    /**
     * @brief 删除按钮点击响应的槽函数
     */
    void onButtonClicked();

    /**
     * @brief 删除PV信号响应的槽函数
     */
    void onPVDeleteMessage(const QString &pvMessage);

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
    DPushButton *m_cancelButton;
    DWarningButton *m_deleteButton;
    DStackedWidget *m_stackedWidget;
    WaterLoadingWidget *m_waterLoadingWidget;
};

#endif // REMOVEPVWIDGET_H
