/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file infotopframe.h
 *
 * @brief 主界面右侧信息展示上部分实现类
 *
 * @date 2020-09-17 14:05
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
#ifndef INFOTOPFRAME_H
#define INFOTOPFRAME_H

#include <DFrame>
#include <DWidget>
#include <DLabel>

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

/**
 * @class InfoTopFrame
 * @brief 分区信息上部分实现类
 */

class InfoTopFrame : public DFrame
{
    Q_OBJECT
public:
    explicit InfoTopFrame(DWidget *parent = nullptr);

public:
    /**
     * @brief 更新界面显示
     */
    void updateDiskInfo();

protected:
    /**
     * @brief 字号变更事件
     */
    void resizeEvent(QResizeEvent *event);

private:
    /**
     * @brief 初始化左侧信息
     */
    void initLeftInfo();

    /**
     * @brief 初始化右侧信息
     */
    void initRightInfo();

private:
    QHBoxLayout *m_mainLayout = nullptr;
    DLabel *m_pictureLabel = nullptr;
    DLabel *m_nameLabel = nullptr;
    DLabel *m_typeLabel = nullptr;
    DLabel *m_allNameLabel = nullptr;
    DLabel *m_allMemoryLabel = nullptr;
};

#endif // INFOTOPFRAME_H
