#include "dmtreeviewdelegate.h"
#include <QDebug>
DmTreeviewDelegate::DmTreeviewDelegate(QAbstractItemView *parent) : DStyledItemDelegate(parent)/*, m_parentView(parent)*/
{

}
void DmTreeviewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        QVariant var = index.data(Qt::UserRole + 1);
        qDebug() << var.data();
        // MailAccountData *data = static_cast<MailAccountData *>(var.value<void *>());
        DiskInfoData data = var.value<DiskInfoData>();
        qDebug() << data.disksize << data.disklabel;
        QRect rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());
        painter->setRenderHints(QPainter::SmoothPixmapTransform);
        // QRect paintRect = QRect(rect.left() + 10, rect.top(), rect.width() - 8, rect.height());
        QRect paintRect = QRect(rect.left(), rect.top(), rect.width(), rect.height());
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

        if (option.state & QStyle::State_Selected) {
            QColor fillColor = option.palette.color(DPalette::Normal, DPalette::Highlight);
            painter->setBrush(QBrush(fillColor));
            painter->fillPath(path, painter->brush());
            painter->setPen(QPen(Qt::white));
        }
        QRect m_lefticon1Rect;
        QRect m_lefticonRect2;
        QRect m_textRect;
        qDebug() << data.level;
        if (data.level == 0) {
            m_lefticonRect2.setRect(paintRect.left() + 8, paintRect.top() + 7, 10, 10);
            QImage image(":/icons/deepin/builtin/light/icons/arrow_right.svg");
            painter->drawImage(m_lefticonRect2, image);
        } else {
            m_lefticon1Rect.setRect(paintRect.left() + 5, paintRect.top() + 10, 24, 24);
            QImage image1(":/icons/deepin/builtin/light/icons/arrow_down.svg");
            painter->drawImage(m_lefticon1Rect, image1);
        }
        painter->restore();
    }


}
