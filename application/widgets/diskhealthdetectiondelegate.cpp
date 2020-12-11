/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskhealthdetectiondelegate.cpp
 *
 * @brief 磁盘健康检测列表代理类
 *
 * @date 2020-08-19 17:24
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
#include "diskhealthdetectiondelegate.h"

#include <DFontSizeManager>

#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QFileIconProvider>
#include <QTemporaryFile>
#include <QEvent>
#include <QAbstractItemModel>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QDir>
#include <QDebug>
#include <QPainterPath>

DiskHealthDetectionDelegate::DiskHealthDetectionDelegate(DDialog *dialog)
{
    m_dialog = dialog;
}

void DiskHealthDetectionDelegate::setTextColor(const QColor &color)
{
    m_color = color;
}

void DiskHealthDetectionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QRect paintRect = QRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    QPainterPath path;
    const int radius = 8;
//qDebug() << paintRect.topRight() << paintRect.topLeft() + QPoint(radius, 0)
//         << paintRect.bottomLeft() - QPoint(0, radius) << paintRect.bottomRight();
    if (index.column() == 0)
    {
        // 左上和左下角为圆角
        path.moveTo(paintRect.bottomRight() - QPoint(0, radius));
        path.lineTo(paintRect.topRight());
        //    path.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
        //                     QSize(radius * 2, radius * 2)),
        //               0, 90);
        path.lineTo(paintRect.topLeft() + QPoint(radius, 0));
        path.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
        path.lineTo(paintRect.bottomLeft() - QPoint(0, radius));
        path.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                         QSize(radius * 2, radius * 2)),
                   180, 90);
        path.lineTo(paintRect.bottomRight());
        //    path.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
        //                     QSize(radius * 2, radius * 2)),
        //               270, 90);

        if (index.row() % 2 == 1) {
            painter->fillPath(path, option.palette.base());
        } else {
            painter->fillPath(path, option.palette.alternateBase());
        }
    }
    else if(index.column() == 6)
    {
        // 右上和右下角为圆角
        path.moveTo(paintRect.bottomRight() - QPoint(0, radius));
        path.lineTo(paintRect.topRight());
        path.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                         QSize(radius * 2, radius * 2)),
                   0, 90);
        path.lineTo(paintRect.topLeft());
    //    path.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
        path.lineTo(paintRect.bottomLeft());
    //    path.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
    //                     QSize(radius * 2, radius * 2)),
    //               180, 90);
        path.lineTo(paintRect.bottomRight());
        path.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                         QSize(radius * 2, radius * 2)),
                   270, 90);

        if (index.row() % 2 == 1) {
            painter->fillPath(path, option.palette.base());
        } else {
            painter->fillPath(path, option.palette.alternateBase());
        }
    }
    else {
        paintRect = QRect(option.rect.left() - 1, option.rect.top(), option.rect.width(), option.rect.height() - 1);
        if(index.column() == 5)
        {
            paintRect = QRect(option.rect.left() - 1, option.rect.top(), option.rect.width() + 1, option.rect.height() - 1);
        }

        if (index.row() % 2 == 1) {
            painter->fillRect(paintRect, option.palette.base());
        } else {
            painter->fillRect(paintRect, option.palette.alternateBase());
        }
    }






    painter->restore();
    painter->save();

    QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, paintRect.width() - 25);

    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8, QFont::Normal);
    painter->setFont(font);
    if (text == "G") {
        painter->setPen(QColor("#00c800"));
    } else if (text == "W") {
        painter->setPen(QColor("#FA6400"));
    } else if (text == "D") {
        painter->setPen(QColor("#E02020"));
    } else if (text == "U") {
        painter->setPen(QColor("#777777"));
    } else {
        painter->setPen(m_color);
    }

    painter->drawText(paintRect.x() + 5, paintRect.y() + 20, text);

    painter->restore();
}



