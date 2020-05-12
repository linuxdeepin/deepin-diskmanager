/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer: linxun <linxun@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dmtreeviewdelegate.h"
#include <QDebug>
#include "dmtreeview.h"
DmTreeviewDelegate::DmTreeviewDelegate(QAbstractItemView *parent) : DStyledItemDelegate(parent), m_parentView(parent)
{
    m_parentPb = DApplicationHelper::instance()->palette(m_parentView);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &DmTreeviewDelegate::slothandleChangeTheme);
}

void DmTreeviewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    qDebug() << option.rect.x() << option.rect.y();
    editor->setGeometry(option.rect.x() + 100, option.rect.y() + 100, 500, 500);



}
QSize DmTreeviewDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(30, 50);
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
        QRect paintRect = QRect(rect.left(), rect.top(), rect.width() - 20, rect.height());
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
        DmTreeview *pTreeView = qobject_cast<DmTreeview *>(m_parentView);
        if (pTreeView == nullptr) {
            // qDebug() << "111111111" << endl;
        }

        QImage leftImage;

        bool bHasSubItem = false;
        bool bExpand = false;

        if (pTreeView->getModelByIndex(index) && pTreeView->getModelByIndex(index)->hasChildren()) {
            bHasSubItem = true;
            if (pTreeView->isExpanded(index)) {
                bExpand = true;
                leftImage.load(":/icons/deepin/builtin/light/icons/arrow.svg");
            } else {
                leftImage.load(":/icons/deepin/builtin/light/icons/arrow_right.svg");
                bExpand = false;
            }
        } else {
            bHasSubItem = false;
        }
        QRect m_lefticon1Rect;
        QRect m_lefticonRect2;
        QRect m_textRect;
        QRect m_textRect1;
        QString text = data.disklabel;
        QString text1 = data.disksize;
        qDebug() << data.level;
        if (data.level == 0) {
            m_lefticon1Rect.setRect(paintRect.left() + 8, paintRect.top() + 20, 8, 8);
//            QImage image(":/icons/deepin/builtin/light/icons/next_normal.svg");
            painter->drawImage(m_lefticon1Rect, leftImage);
            m_lefticonRect2.setRect(paintRect.left() + 15, paintRect.top() + 4, 40, 40);
            QImage image2(":/icons/deepin/builtin/light/icons/drive-removable-media-48px.png");
            painter->drawImage(m_lefticonRect2, image2);
            m_textRect.setRect(paintRect.left() + 60, paintRect.top() + 5, 100, 100);
            painter->drawText(m_textRect, text);
            m_textRect1.setRect(paintRect.left() + 60, paintRect.top() + 20, 100, 100);
            painter->drawText(m_textRect1, text1);
        } else {
            m_lefticon1Rect.setRect(paintRect.left() + 5, paintRect.top() + 5, 15, 15);
            QImage image1(":/icons/deepin/builtin/exception-logo.svg");
            painter->drawImage(m_lefticon1Rect, image1);
        }


        painter->restore();
    }


}
void DmTreeviewDelegate::slothandleChangeTheme()
{



}
