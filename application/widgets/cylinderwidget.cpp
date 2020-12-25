/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file cylinderwidget.cpp
 *
 * @brief 柱面小方块实现类，主要实现柱面数据存储以及进入、离开信号的发送
 *
 * @date 2020-12-22 16:02
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
#include "cylinderwidget.h"

CylinderWidget::CylinderWidget(QWidget *parent) : QLabel(parent)
{

}

void CylinderWidget::setUserData(const QVariant &userData)
{
    m_userData = userData;
}

QVariant CylinderWidget::getUserData()
{
    return m_userData;
}

void CylinderWidget::enterEvent(QEvent *event)
{
    emit enter();
}

void CylinderWidget::leaveEvent(QEvent *event)
{
    emit leave();
}

