/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mainsplitter.h
 *
 * @brief 用Splitter控件，主要实现页面左右布局，左侧显示设备树结构，右侧显示分区详细信息
 *
 * @date 2020-09-04 13:28
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
#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H
#include <DSplitter>

DWIDGET_USE_NAMESPACE

class DeviceListWidget;
class InfoShowWidget;

/**
 * @class MainSplitter
 * @brief 主分束器，将页面进行左右布局显示
 */

class MainSplitter : public DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(MainSplitter)
public:
    explicit MainSplitter(DWidget *parent = nullptr);

private:

    /**
     * @brief 初始化页面
     */
    void initui();

    /**
     * @brief 初始化连接
     */
    void initConnection();

private:
    InfoShowWidget *m_infoShow; // 分区信息展示窗口
    DeviceListWidget *m_deviceList; // 设备树结构窗口
};

#endif // MAINSPLITTER_H
