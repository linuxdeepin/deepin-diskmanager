/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "dmtreeviewdelegate.h"
#include "common.h"
#include "dmtreeview.h"
#include <DFontSizeManager>
#include <QDebug>
#include <QTextOption>

DmTreeviewDelegate::DmTreeviewDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
    , m_parentView(parent)
{
    m_parentPb = DApplicationHelper::instance()->palette(m_parentView);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &DmTreeviewDelegate::slothandleChangeTheme);
}

QSize DmTreeviewDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Q_UNUSED(option);

    DiskInfoData d = index.data(Qt::UserRole + 1).value<DiskInfoData>();
    if (d.level == 0)
        return QSize(180, 62);

    return QSize(180, 55);
}

void DmTreeviewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        QVariant var = index.data(Qt::UserRole + 1);
        DiskInfoData data = var.value<DiskInfoData>();
        QRect rect;
        if (data.level == 0) {
            rect.setX(option.rect.x());
        } else {
            rect.setX(option.rect.x() + 10);
        }
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());
        painter->setRenderHints(QPainter::SmoothPixmapTransform);
        QRect paintRect = QRect(rect.left(), rect.top(), rect.width() - 30, rect.height());
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
        if (data.level == 1) {
            QBrush brush = m_parentPb.itemBackground();
            painter->setBrush(brush);
            painter->fillPath(path, brush);
        }

        if (option.state & QStyle::State_Selected && data.level == 1) {
            QColor fillColor = m_parentPb.color(DPalette::Normal, DPalette::Highlight);
            painter->setBrush(QBrush(fillColor));
            painter->fillPath(path, painter->brush());
            painter->setPen(QPen(Qt::white));
        }
        DmTreeview *pTreeView = qobject_cast<DmTreeview *>(m_parentView);
        if (pTreeView == nullptr) {
        }

        QIcon directionIcon;
        if (pTreeView->getModelByIndex(index) && pTreeView->getModelByIndex(index)->hasChildren()) {
            if (pTreeView->isExpanded(index)) {
                directionIcon = Common::getIcon("arrow");
            } else {
                directionIcon = Common::getIcon("arrow_right");
            }
        }
        QRect m_lefticon1Rect;
        QRect m_lefticonRect2;
        QRect m_textRect;
        QRect m_textRect1;
        QString text = data.diskpath;
        QString text1 = data.disksize;
        QString text2 = data.partitonpath;
        QString text3 = data.partitionsize;
        if (data.level == 0) {
            m_lefticon1Rect.setRect(paintRect.left() + 8, paintRect.top() + 20, 8, 8);
            painter->drawPixmap(m_lefticon1Rect, directionIcon.pixmap(17, 17));
            m_lefticonRect2.setRect(paintRect.left() + 15, paintRect.top() + 4, 40, 40);
            QIcon icon = Common::getIcon("treedisk");
            painter->drawPixmap(m_lefticonRect2, icon.pixmap(38, 38));
            QTextOption option;
            QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textcolor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter->setPen(textcolor);
            painter->setFont(font);
            m_textRect.setRect(paintRect.left() + 60, paintRect.top() + 5, 100, 100);
            painter->drawText(m_textRect, text);
            QColor text1color = m_parentPb.color(DPalette::Normal, DPalette::TextTips);
            painter->setPen(text1color);
            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            painter->setFont(font);
            m_textRect1.setRect(paintRect.left() + 60, paintRect.top() + 25, 100, 100);
            painter->drawText(m_textRect1, text1);
        } else {
            m_lefticon1Rect.setRect(paintRect.left() + 25, paintRect.top() + 10, 30, 30);
            QIcon icon = Common::getIcon("harddisk");
            QIcon icon1 = Common::getIcon("mounticon");
            QIcon icon2 = Common::getIcon("uninstallicon");
            painter->drawPixmap(m_lefticon1Rect, icon.pixmap(28, 28));
            QRect mounticonRect = QRect(paintRect.left() + 45, paintRect.top() + 25, 10, 10);
            if (data.fstype == "unallocated") {
                painter->drawPixmap(mounticonRect, icon2.pixmap(10, 10));
            } else {
                painter->drawPixmap(mounticonRect, icon1.pixmap(10, 10));
            }
            QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            if (option.state & QStyle::State_Selected && data.level == 1) {
                QColor textcolor = m_parentPb.color(DPalette::Normal, DPalette::HighlightedText);
                painter->setPen(textcolor);
                painter->setFont(font);
                m_textRect.setRect(paintRect.left() + 65, paintRect.top() + 8, 100, 100);
                painter->drawText(m_textRect, text2);
                QColor text1color = m_parentPb.color(DPalette::Normal, DPalette::HighlightedText);
                painter->setPen(text1color);
                font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
                painter->setFont(font);
                m_textRect1.setRect(paintRect.left() + 65, paintRect.top() + 28, 100, 100);
                painter->drawText(m_textRect1, text3);
            } else {
                QColor textcolor = m_parentPb.color(DPalette::Normal, DPalette::Text);
                painter->setPen(textcolor);
                painter->setFont(font);
                m_textRect.setRect(paintRect.left() + 65, paintRect.top() + 8, 100, 100);
                painter->drawText(m_textRect, text2);
                QColor text1color = m_parentPb.color(DPalette::Normal, DPalette::TextTips);
                painter->setPen(text1color);
                font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
                painter->setFont(font);
                m_textRect1.setRect(paintRect.left() + 65, paintRect.top() + 28, 100, 100);
                painter->drawText(m_textRect1, text3);
            }
        }

        painter->restore();
    }
}
void DmTreeviewDelegate::slothandleChangeTheme()
{
    m_parentPb = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    m_parentView->update(m_parentView->currentIndex());
}
