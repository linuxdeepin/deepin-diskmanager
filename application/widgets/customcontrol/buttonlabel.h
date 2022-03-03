/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file buttonlabel.h
 *
 * @brief 可点击Label窗口
 *
 * @date 2022-02-24 10:00
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
#ifndef BUTTONLABEL_H
#define BUTTONLABEL_H

#include <QWidget>
#include <QLabel>
#include <QVariant>

/**
 * @class ButtonLabel
 * @brief label按钮类
 */

class ButtonLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ButtonLabel(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    /**
     * @brief 鼠标点击事件
     */
    virtual void mousePressEvent(QMouseEvent *event) override;

};

#endif // BUTTONLABEL_H
