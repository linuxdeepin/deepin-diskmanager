/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mainsplitter.cpp
 *
 * @brief 用Splitter控件，主要实现页面左右布局，左侧显示设备树结构，右侧显示分区详细信息
 *
 * @date 2020-09-04 13:31
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
#include "mainsplitter.h"
#include "devicelistwidget.h"
#include "infoshowwidget.h"

MainSplitter::MainSplitter(DWidget *parent)
    : DSplitter(parent)
{
    setFrameShape(QFrame::NoFrame);

    initui();
    initConnection();
}

void MainSplitter::initui()
{
    setHandleWidth(1);
    setChildrenCollapsible(false); //  子部件不可拉伸到 0

    m_deviceList = new DeviceListWidget(this);
    m_infoShow = new InfoShowWidget(this);

    setMouseTracking(true);
    setStretchFactor(1, 1);

    m_deviceList->setMinimumWidth(200);
    m_deviceList->setMaximumWidth(300);
    m_infoShow->setFrameShape(QFrame::NoFrame);

    addWidget(m_deviceList);
    addWidget(m_infoShow);
}

void MainSplitter::initConnection()
{
}
