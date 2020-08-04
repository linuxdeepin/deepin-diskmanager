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


