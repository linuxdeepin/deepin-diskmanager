/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file infoshowwidget.h
 *
 * @brief 分区信息展示窗口
 *
 * @date 2020-09-04 16:10
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
#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H

#include "customcontrol/dmtreeviewdelegate.h"

#include <DWidget>
#include <DLabel>
#include <DFrame>
#include <DProgressBar>

class DmFrameWidget;
class SizeInfoWidget;
class InfoTopFrame;

DWIDGET_USE_NAMESPACE

/**
 * @class InfoShowWidget
 * @brief 分区信息展示类
 */

class InfoShowWidget : public DFrame
{
    Q_OBJECT
public:
    explicit InfoShowWidget(DWidget *parent = nullptr);

private:

    /**
     * @brief 设置显示容量大小窗口（右侧页面中间部分）
     */
    void midFramSettings();

    /**
     * @brief 设置分区详细信息窗口（右侧页面下面表格部分）
     */
    void bottomFramSettings();

    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

private slots:
    /**
     * @brief 当前选择分区信号响应的槽函数
     */
    void onCurSelectChanged();

    /**
     * @brief 主题切换信号响应的槽函数
     */
    void onHandleChangeTheme();

private:
    InfoTopFrame *m_infoTopFrame = nullptr;
    DFrame *m_frame = nullptr;
    DFrame *m_frameMid = nullptr;
    DmFrameWidget *m_frameBottom = nullptr;
    SizeInfoWidget *m_infoWidget = nullptr;
    double m_used;
    double m_noused;
    DiskInfoData m_diskInfoData;
    QColor fillcolor;
    QColor fillcolor1;

    DLabel *m_mountpointLabel;
    DLabel *m_freeLabel;
    DLabel *m_usedLabel;
    DLabel *m_typeLabel;
    DLabel *m_capacityLabel;
    DLabel *m_volumeLabel;
};

#endif // INFOSHOWWIDGET_H
