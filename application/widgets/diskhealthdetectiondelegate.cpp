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

    if (index.row() % 2 == 1) {
        painter->fillRect(option.rect, option.palette.base());
    } else {
        painter->fillRect(option.rect, option.palette.alternateBase());
    }

    painter->restore();
    painter->save();

    QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 25);

    painter->setFont(QFont("SourceHanSansSC", 10, 50));
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

    painter->drawText(option.rect.x() + 5, option.rect.y() + 20, text);

    painter->restore();
}



