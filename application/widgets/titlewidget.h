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

    /**
     * @brief 设置当前操作设备路径
     */
    void setCurDevicePath(const QString &devPath);

    /**
     * @brief 判断当前磁盘是否存在已被挂载分区
     * @return 存在返回true，否则返回false
     */
    bool isExistMountPartition();

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

    /**
     * @brief usb热插拔信号响应的槽函数
     */
    void onUpdateUsb();

    /**
     * @brief 创建逻辑卷按钮点击响应的槽函数
     */
    void onCreateLVClicked();

    /**
     * @brief 删除逻辑卷按钮点击响应的槽函数
     */
    void onDeleteLVClicked();

    /**
     * @brief 逻辑卷空间调整按钮点击响应的槽函数
     */
    void onResizeLVClicked();

    /**
     * @brief 创建逻辑卷组按钮点击响应的槽函数
     */
    void onCreateVGClicked();

    /**
     * @brief 删除逻辑卷组按钮点击响应的槽函数
     */
    void onDeleteVGClicked();

    /**
     * @brief 逻辑卷组空间调整按钮点击响应的槽函数
     */
    void onResizeVGClicked();

    /**
     * @brief 删除物理卷按钮点击响应的槽函数
     */
    void onDeletePVClicked();


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
    DPushButton *m_btnCreateVG; // 创建逻辑卷组按钮
    DPushButton *m_btnDeleteVG; // 删除逻辑卷组按钮
    DPushButton *m_btnDeleteLV; // 删除逻辑卷按钮
    DPushButton *m_btnDeletePV; // 删除物理卷按钮
    DPushButton *m_btnCreateLV; // 创建逻辑卷按钮
    DPushButton *m_btnResizeVG; // 逻辑卷组空间调整按钮
    DPushButton *m_btnResizeLV; // 逻辑卷空间调整按钮
    QString m_curChooseDevicePath; // 当前操作磁盘
};

#endif // TITLEWIDGET_H
