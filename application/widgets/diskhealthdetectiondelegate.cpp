// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
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
    qCDebug(appLog) << "DiskHealthDetectionDelegate constructor";
    m_dialog = dialog;
}

void DiskHealthDetectionDelegate::setTextColor(const QColor &color)
{
    qCDebug(appLog) << "Setting text color:" << color;
    m_color = color;
}

void DiskHealthDetectionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // qCDebug(appLog) << "Painting cell at row:" << index.row() << "column:" << index.column();
    painter->save();

    QRect paintRect = QRect(option.rect.left(), option.rect.top(), option.rect.width(), option.rect.height());
    QPainterPath path;
    const int radius = 8;
//qCDebug(appLog) << paintRect.topRight() << paintRect.topLeft() + QPoint(radius, 0)
//         << paintRect.bottomLeft() - QPoint(0, radius) << paintRect.bottomRight();
    if (index.column() == 0) {
        // qCDebug(appLog) << "Painting for column 0 (first column).";
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
            // qCDebug(appLog) << "Filling path with base palette for odd row.";
            painter->fillPath(path, option.palette.base());
        } else {
            // qCDebug(appLog) << "Filling path with alternate base palette for even row.";
            painter->fillPath(path, option.palette.alternateBase());
        }
    } else if(index.column() == 6) {
        // qCDebug(appLog) << "Painting for column 6 (last column).";
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
            // qCDebug(appLog) << "Filling path with base palette for odd row (column 6).";
            painter->fillPath(path, option.palette.base());
        } else {
            // qCDebug(appLog) << "Filling path with alternate base palette for even row (column 6).";
            painter->fillPath(path, option.palette.alternateBase());
        }
    } else {
        // qCDebug(appLog) << "Painting for middle columns.";
        paintRect = QRect(option.rect.left() - 1, option.rect.top(), option.rect.width(), option.rect.height() - 1);
        if(index.column() == 5)
        {
            // qCDebug(appLog) << "Adjusting paintRect for column 5.";
            paintRect = QRect(option.rect.left() - 1, option.rect.top(), option.rect.width() + 1, option.rect.height() - 1);
        }

        if (index.row() % 2 == 1) {
            // qCDebug(appLog) << "Filling rect with base palette for odd row (middle column).";
            painter->fillRect(paintRect, option.palette.base());
        } else {
            // qCDebug(appLog) << "Filling rect with alternate base palette for even row (middle column).";
            painter->fillRect(paintRect, option.palette.alternateBase());
        }
    }

    painter->restore();
    painter->save();

    QString text = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, paintRect.width() - 25);

    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8, QFont::Normal);
    painter->setFont(font);
    if (text == "G") {
        // qCDebug(appLog) << "Drawing GOOD status icon";
        painter->setPen(QColor("#00c800"));
    } else if (text == "W") {
        // qCDebug(appLog) << "Drawing WARNING status icon";
        painter->setPen(QColor("#FA6400"));
    } else if (text == "D") {
        // qCDebug(appLog) << "Drawing DAMAGED status icon";
        painter->setPen(QColor("#E02020"));
    } else if (text == "U") {
        // qCDebug(appLog) << "Drawing UNKNOWN status icon";
        painter->setPen(QColor("#777777"));
    } else {
        painter->setPen(option.palette.color(QPalette::Text));
    }

    painter->drawText(paintRect.x() + 15, paintRect.y() + 20, text);

    painter->restore();
    // qCDebug(appLog) << "Paint event completed for row:" << index.row() << "column:" << index.column();
}



