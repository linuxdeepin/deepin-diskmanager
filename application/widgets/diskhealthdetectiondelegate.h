/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskhealthdetectiondelegate.h
 *
 * @brief 磁盘健康检测列表代理类
 *
 * @date 2020-08-19 17:22
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
#ifndef DISKHEALTHDETECTIONDELEGATE_H
#define DISKHEALTHDETECTIONDELEGATE_H

#include <DDialog>
#include <QStyledItemDelegate>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

/**
 * @class DiskHealthDetectionDelegate
 * @brief 磁盘健康检测列表代理类
*/

class DiskHealthDetectionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DiskHealthDetectionDelegate(DDialog *dialog);

    /**
     * @brief 设置表格item字体颜色
     * @param color 颜色
     */
    void setTextColor(const QColor &color);

signals:

public slots:

protected:
    /**
     * @brief 重写绘画事件
     * @param painter 画笔
     * @param option 视图小部件中绘制项的参数
     * @param index 数据模型中的数据
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;      //绘制事件

private:
    DDialog *m_dialog; //父类窗口指针
    QColor m_color; // 字体颜色
};

#endif // DISKHEALTHDETECTIONDELEGATE_H
