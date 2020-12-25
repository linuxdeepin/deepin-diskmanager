/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file cylinderwidget.h
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
#ifndef CYLINDERWIDGET_H
#define CYLINDERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVariant>

/**
 * @class CylinderWidget
 * @brief 柱面小方块类
 */

class CylinderWidget : public QLabel
{
    Q_OBJECT
public:
    explicit CylinderWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置当前柱面的检测信息数据
     * @param userData 当前柱面检测信息
     */
    void setUserData(const QVariant &userData);

    /**
     * @brief 获取当前柱面的检测信息数据
     * @return 当前柱面检测信息
     */
    QVariant getUserData();

signals:
    /**
     * @brief 鼠标进入信号
     */
    void enter();

    /**
     * @brief 鼠标离开信号
     */
    void leave();

public slots:

protected:
    /**
     * @brief 重写进入事件
     */
    void enterEvent(QEvent *event);

    /**
     * @brief 重写离开事件
     */
    void leaveEvent(QEvent *event);

private:
    QVariant m_userData;
};

#endif // CYLINDERWIDGET_H
