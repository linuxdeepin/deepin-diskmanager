/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file titlewidget.h
 *
 * @brief 标题功能图标按钮类，主要实现功能图标按钮展示以及点击操作
 *
 * @date 2020-09-04 14:06
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
#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include "partedproxy/dmdbushandler.h"
#include "customcontrol/partitionwidget.h"
#include "partitiondialog.h"
#include "utils.h"

#include <DWidget>
#include <DPushButton>
//#include "customcontrol/tippartdialog.h"

DWIDGET_USE_NAMESPACE

/**
 * @class TitleWidget
 * @brief 标题功能图标按钮类
 */

class TitleWidget : public DWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(DWidget *parent = nullptr);

private:

    /**
     * @brief 初始化页面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

    /**
     * @brief 创建按钮
     * @param btnName 按钮名称
     * @param objName 按钮图标名称
     * @param bCheckable 是否可选
     */
    DPushButton *createBtn(const QString &btnName, const QString &objName, bool bCheckable = false);

    /**
     * @brief 更新按钮是否可选状态
     */
    void updateBtnStatus();

signals:

private slots:

    /**
     * @brief 更新按钮是否可选状态
     */
    void onCurSelectChanged();

    /**
     * @brief 分区按钮点击响应的槽函数
     */
    void showPartInfoWidget();

    /**
     * @brief 格式化按钮点击响应的槽函数
     */
    void showFormateInfoWidget();

    /**
     * @brief 挂载按钮点击响应的槽函数
     */
    void showMountInfoWidget();

    /**
     * @brief 卸载按钮点击响应的槽函数
     */
    void showUnmountInfoWidget();

    /**
     * @brief 空间调整按钮点击响应的槽函数
     */
    void showResizeInfoWidget();

protected:

    /**
     * @brief 鼠标点击事件
     */
    void mousePressEvent(QMouseEvent *event) override;

private:
    DPushButton *m_btnParted; // 分区按钮
    DPushButton *m_btnFormat; // 格式化按钮
    DPushButton *m_btnMount; // 挂载按钮
    DPushButton *m_btnUnmount; // 卸载按钮
    DPushButton *m_btnResize; // 空间调整按钮
};

#endif // TITLEWIDGET_H
