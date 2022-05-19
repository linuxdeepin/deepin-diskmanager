/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file common.h
 *
 * @brief 公共类
 *
 * @date 2020-09-18 15:14
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
#ifndef COMMON_H
#define COMMON_H

#include <DLabel>

#include <QIcon>

DWIDGET_USE_NAMESPACE

/**
 * @class Common
 * @brief 公共类
*/

const static QString appName = "deepin-diskmanager";
class Common
{
public:

    /**
     * @brief 获取图标
     * @return 返回图标
    */
    static QIcon getIcon(const QString &iconName);

    /**
     * @brief 获取label自适应后的高度
     * @param width: label的宽度
     * @param text: label显示的文本
     * @param font: label字体样式
     * @return label当前高度
     */
    static int getLabelAdjustHeight(const int &width, const QString &text, const QFont &font);

};

#endif // COMMON_H
