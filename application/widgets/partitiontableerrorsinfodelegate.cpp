// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


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
#include <QPainterPath>

PartitionTableErrorsInfoDelegate::PartitionTableErrorsInfoDelegate(DDialog *dialog)
{
    qDebug() << "PartitionTableErrorsInfoDelegate initialized";
    m_dialog = dialog;
}

void PartitionTableErrorsInfoDelegate::setTextColor(const QColor &color)
{
    qDebug() << "Setting text color to:" << color.name();
    m_color = color;
}

void PartitionTableErrorsInfoDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // qDebug() << "paint called for index:" << index.row() << ", column:" << index.column();
    painter->save();

    QRect paintRect = QRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    if (index.row() % 2 == 0) {
        // qDebug() << "Row is even, adjusting paintRect.";
        paintRect = QRect(option.rect.left() + 10, option.rect.top(), option.rect.width() - 20, option.rect.height());
    }

    QPainterPath path;
    const int radius = 8;
    path.moveTo(paintRect.bottomRight() - QPoint(0, radius));
    path.lineTo(paintRect.topRight() + QPoint(0, radius));
    path.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                     QSize(radius * 2, radius * 2)),
               0, 90);
    path.lineTo(paintRect.topLeft() + QPoint(radius, 0));
    path.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
    path.lineTo(paintRect.bottomLeft() - QPoint(0, radius));
    path.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                     QSize(radius * 2, radius * 2)),
               180, 90);
    path.lineTo(paintRect.bottomLeft() + QPoint(radius, 0));
    path.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                     QSize(radius * 2, radius * 2)),
               270, 90);

    if (index.row() % 2 == 1) {
        // qDebug() << "Row is odd, filling path with base palette.";
        painter->fillPath(path, option.palette.base());
    } else {
        // qDebug() << "Row is even, filling path with alternateBase palette.";
        painter->fillPath(path, option.palette.alternateBase());
    }

    painter->restore();
    painter->save();

    QIcon icon = Common::getIcon("warning");
    painter->drawPixmap(option.rect.x() + 17, option.rect.y() + 9, icon.pixmap(12, 12));

    QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 25);

    painter->setFont(QFont("SourceHanSansSC", 10, 50));
    painter->setPen(m_color);

    painter->drawText(option.rect.x() + 36, option.rect.y() + 20, text);

    painter->restore();
    // qDebug() << "paint completed.";
}


