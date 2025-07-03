// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


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
    qDebug() << "DiskHealthDetectionDelegate constructor";
    m_dialog = dialog;
}

void DiskHealthDetectionDelegate::setTextColor(const QColor &color)
{
    qDebug() << "Setting text color:" << color;
    m_color = color;
}

void DiskHealthDetectionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // qDebug() << "Painting cell at row:" << index.row() << "column:" << index.column();
    painter->save();

    QRect paintRect = QRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    QPainterPath path;
    const int radius = 8;
//qDebug() << paintRect.topRight() << paintRect.topLeft() + QPoint(radius, 0)
//         << paintRect.bottomLeft() - QPoint(0, radius) << paintRect.bottomRight();
    if (index.column() == 0) {
        // qDebug() << "Painting for column 0 (first column).";
        paintRect = QRect(option.rect.left() + 10, option.rect.top(), option.rect.width() - 10, option.rect.height());
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
            // qDebug() << "Filling path with base palette for odd row.";
            painter->fillPath(path, option.palette.base());
        } else {
            // qDebug() << "Filling path with alternate base palette for even row.";
            painter->fillPath(path, option.palette.alternateBase());
        }
    } else if(index.column() == 6) {
        // qDebug() << "Painting for column 6 (last column).";
        paintRect = QRect(option.rect.left(), option.rect.top(), option.rect.width() - 10, option.rect.height());
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
            // qDebug() << "Filling path with base palette for odd row (column 6).";
            painter->fillPath(path, option.palette.base());
        } else {
            // qDebug() << "Filling path with alternate base palette for even row (column 6).";
            painter->fillPath(path, option.palette.alternateBase());
        }
    } else {
        // qDebug() << "Painting for middle columns.";
        paintRect = QRect(option.rect.left() - 1, option.rect.top(), option.rect.width(), option.rect.height() - 1);
        if(index.column() == 5)
        {
            // qDebug() << "Adjusting paintRect for column 5.";
            paintRect = QRect(option.rect.left() - 1, option.rect.top(), option.rect.width() + 1, option.rect.height() - 1);
        }

        if (index.row() % 2 == 1) {
            // qDebug() << "Filling rect with base palette for odd row (middle column).";
            painter->fillRect(paintRect, option.palette.base());
        } else {
            // qDebug() << "Filling rect with alternate base palette for even row (middle column).";
            painter->fillRect(paintRect, option.palette.alternateBase());
        }
    }

    painter->restore();
    painter->save();

    QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, paintRect.width() - 25);

    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8, QFont::Normal);
    painter->setFont(font);
    if (text == "G") {
        // qDebug() << "Drawing GOOD status icon";
        painter->setPen(QColor("#00c800"));
    } else if (text == "W") {
        // qDebug() << "Drawing WARNING status icon";
        painter->setPen(QColor("#FA6400"));
    } else if (text == "D") {
        // qDebug() << "Drawing DAMAGED status icon";
        painter->setPen(QColor("#E02020"));
    } else if (text == "U") {
        // qDebug() << "Drawing UNKNOWN status icon";
        painter->setPen(QColor("#777777"));
    } else {
        // qDebug() << "Drawing UNKNOWN status icon";
        painter->setPen(m_color);
    }

    painter->drawText(paintRect.x() + 15, paintRect.y() + 20, text);

    painter->restore();
    // qDebug() << "Paint event completed for row:" << index.row() << "column:" << index.column();
}



