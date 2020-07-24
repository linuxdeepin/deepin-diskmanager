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



