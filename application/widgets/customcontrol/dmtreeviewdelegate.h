/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmtreeviewdelegate.h
 *
 * @brief 设备树结构代理类
 *
 * @date 2020-09-18 09:06
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
#ifndef DMTREEVIEWDELEGATE_H
#define DMTREEVIEWDELEGATE_H

#include "commondef.h"

#include <DApplicationHelper>
#include <DStyledItemDelegate>
#include <DPalette>

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QAbstractItemView>
#include <QImage>

struct DiskInfoData {
    QString m_diskPath;
    QString m_diskSize;
    QImage m_iconImage;
    QString m_partitonPath;
    QString m_partitionSize;
    QString m_used;
    QString m_unused;
    QString m_fstype;
    QString m_sysLabel;
    QString m_mountpoints;
    Sector m_sectorsUnallocated;
    Sector m_start;
    Sector m_end;

    int m_level;
    int m_flag;
};
Q_DECLARE_METATYPE(DiskInfoData)

DWIDGET_USE_NAMESPACE

/**
 * @class DmTreeviewDelegate
 * @brief 设备树结构代理类
*/

class DmTreeviewDelegate : public DStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DmTreeviewDelegate(QAbstractItemView *parent = nullptr);

    /**
     * @brief 重写绘画事件
     * @param painter 画笔
     * @param option 视图小部件中绘制项的参数
     * @param index 数据模型中的数据
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /**
     * @brief 重写窗口初始大小函数
     * @param option 视图小部件中绘制项的参数
     * @param index 数据模型中的数据
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:

public slots:

    /**
     * @brief 主题切换信号响应的槽函数
     */
    void onHandleChangeTheme();

private:
    QAbstractItemView *m_parentView {nullptr};
    DPalette m_parentPb;
};

#endif // DMTREEVIEWDELEGATE_H
