/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file selecteditemwidget.h
 *
 * @brief 已选分区信息或者磁盘信息展示窗口
 *
 * @date 2022-02-24 10:48
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
#ifndef SELECTEDITEMWIDGET_H
#define SELECTEDITEMWIDGET_H

#include "radiuswidget.h"
#include "buttonlabel.h"

#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @class SelectedItemWidget
 * @brief 已选分区信息或者磁盘信息展示类
 */

class SelectedItemWidget : public RadiusWidget
{
    Q_OBJECT
public:
    explicit SelectedItemWidget(PVInfoData pvInfoData, QWidget *parent = nullptr);

signals:
    void deleteItem(PVInfoData pvInfoData);

private slots:
    /**
     * @brief 删除按钮点击响应的槽函数
     */
    void onDeleteClicked();

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
    DLabel *m_nameInfo;
    DLabel *m_sizeInfo;
    ButtonLabel *m_buttonLabel;
    PVInfoData m_pvInfoData;
};

#endif // SELECTEDITEMWIDGET_H
