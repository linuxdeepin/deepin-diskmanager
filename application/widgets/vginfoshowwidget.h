/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file vginfoshowwidget.h
 *
 * @brief 逻辑卷组下的逻辑卷信息展示类
 *
 * @date 2022-01-24 13:33
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
#ifndef VGINFOSHOWWIDGET_H
#define VGINFOSHOWWIDGET_H

#include <QWidget>

#include <DFrame>
#include <DScrollArea>

DWIDGET_USE_NAMESPACE

/**
 * @class VGInfoShowWidget
 * @brief 逻辑卷组下的逻辑卷信息展示类，主界面图示区域“…”鼠标悬浮窗口
 */

class QGridLayout;

class VGInfoShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VGInfoShowWidget(QWidget *parent = nullptr);

    void setData(const QList< QMap<QString, QVariant> > &lstInfo);

signals:

public slots:

private:
    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();
};

#endif // VGINFOSHOWWIDGET_H
