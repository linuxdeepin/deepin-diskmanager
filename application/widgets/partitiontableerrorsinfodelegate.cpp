/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitiontableerrorsinfodelegate.cpp
 *
 * @brief 分区表错误报告列表代理类
 *
 * @date 2020-08-21 13:34
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
#include "partitiontableerrorsinfodelegate.h"
#include "common.h"

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

PartitionTableErrorsInfoDelegate::PartitionTableErrorsInfoDelegate(DDialog *dialog)
{
    m_dialog = dialog;
}

void PartitionTableErrorsInfoDelegate::setTextColor(const QColor &color)
{
    m_color = color;
}

void PartitionTableErrorsInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if (index.row() % 2 == 1) {
        painter->fillRect(option.rect, option.palette.base());
    } else {
        painter->fillRect(option.rect, option.palette.alternateBase());
    }

    painter->restore();
    painter->save();

    QIcon icon = Common::getIcon("warning");
    painter->drawPixmap(option.rect.x() + 7, option.rect.y() + 9, icon.pixmap(12, 12));

    QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 25);

    painter->setFont(QFont("SourceHanSansSC", 10, 50));
    painter->setPen(m_color);

    painter->drawText(option.rect.x() + 26, option.rect.y() + 20, text);

    painter->restore();
}


