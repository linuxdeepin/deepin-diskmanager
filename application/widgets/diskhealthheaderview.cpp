// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "diskhealthheaderview.h"

#include <DApplication>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#endif
#include <DPalette>
#include <DStyleHelper>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include <QPainterPath>

static const int kSpacingMargin = 4;
// recommend size
// static const QSize kDropDownSize {10, 6};
static const QSize kDropDownSize {11, 10};

DiskHealthHeaderView::DiskHealthHeaderView(Qt::Orientation orientation, QWidget *parent) : DHeaderView(orientation, parent)
{
    qDebug() << "DiskHealthHeaderView constructor";
    installEventFilter(this);

    viewport()->setAutoFillBackground(false);
    qDebug() << "DiskHealthHeaderView constructor finished.";
}

QSize DiskHealthHeaderView::sizeHint() const
{
    qDebug() << "DiskHealthHeaderView::sizeHint called.";
    return QSize(width(), 36 + m_spacing);
}

int DiskHealthHeaderView::sectionSizeHint(int logicalIndex) const
{
    qDebug() << "DiskHealthHeaderView::sectionSizeHint called for logicalIndex:" << logicalIndex;
//    QStyleOptionHeader option;
//    initStyleOption(&option);
//    auto *style = dynamic_cast<DStyle *>(DApplication::style());
//    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);

//    QFontMetrics fm(this->font());
//    QString buf = model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();
//    if (sortIndicatorSection() == logicalIndex) {
//        return fm.size(Qt::TextSingleLine, buf).width() + margin * 3 + kDropDownSize.width();
//    } else {
//        return fm.size(Qt::TextSingleLine, buf).width() + margin * 2;
//    }
    return DHeaderView::sectionSizeHint(logicalIndex);
}

void DiskHealthHeaderView::paintEvent(QPaintEvent *event)
{
    // qDebug() << "DiskHealthHeaderView::paintEvent called.";
    QPainter painter(viewport());
    painter.save();

    DPalette::ColorGroup cg;
#ifdef ENABLE_INACTIVE_DISPLAY
    // qDebug() << "ENABLE_INACTIVE_DISPLAY is defined.";
    QWidget *wnd = DApplication::activeWindow();
    if (!wnd) {
        // qDebug() << "No active window, setting ColorGroup to Inactive.";
        cg = DPalette::Inactive;
    } else {
        // qDebug() << "Active window found, setting ColorGroup to Active.";
        cg = DPalette::Active;
    }
#else
    // qDebug() << "ENABLE_INACTIVE_DISPLAY is not defined, setting ColorGroup to Active.";
    cg = DPalette::Active;
//    cg = DPalette::Current;
#endif

#if QT_VERSION_MAJOR > 5
    // qDebug() << "Using Qt6 or higher for application palette.";
    auto palette = DGuiApplicationHelper::instance()->applicationPalette();
#else
    // qDebug() << "Using Qt5 for application palette.";
    auto palette = DApplicationHelper::instance()->applicationPalette();
#endif

    auto *style = dynamic_cast<DStyle *>(DApplication::style());

    QBrush bgBrush(palette.color(cg, DPalette::Base));

    QStyleOptionHeader option;
    initStyleOption(&option);
    auto radius = style->pixelMetric(DStyle::PM_FrameRadius, &option);

    QRect rect = viewport()->rect();
    QRectF clipRect(rect.x(), rect.y(), rect.width(), rect.height() * 2);
    QRectF subRect(rect.x(), rect.y() + rect.height(), rect.width(), rect.height());
    QPainterPath clipPath, subPath;
    clipPath.addRoundedRect(clipRect, radius, radius);
    subPath.addRect(subRect);
    clipPath = clipPath.subtracted(subPath);

    painter.fillPath(clipPath, bgBrush);

    DHeaderView::paintEvent(event);
    painter.restore();

    // draw focus
    if (hasFocus()) {
        // qDebug() << "Header view has focus, drawing focus rect.";
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(option);
        QRect focusRect {rect.x() - offset(), rect.y(), length() - sectionPosition(0), rect.height()};
        o.rect = style->visualRect(layoutDirection(), rect, focusRect);
        style->drawPrimitive(DStyle::PE_FrameFocusRect, &o, &painter);
    }
    // qDebug() << "DiskHealthHeaderView::paintEvent completed.";
}

void DiskHealthHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // qDebug() << "DiskHealthHeaderView::paintSection called for logicalIndex:" << logicalIndex << ", rect:" << rect;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(1);

    DPalette::ColorGroup cg;
    QStyleOption opt;
    opt.initFrom(this);
    if (!(opt.state & DStyle::State_Enabled)) {
        // qDebug() << "Style option state is not enabled, setting ColorGroup to Disabled.";
        cg = DPalette::Disabled;
    } else {
#ifdef ENABLE_INACTIVE_DISPLAY
        // qDebug() << "ENABLE_INACTIVE_DISPLAY is defined in paintSection.";
        if (!wnd) {
            // qDebug() << "No active window, setting ColorGroup to Inactive in paintSection.";
            cg = DPalette::Inactive;
        } else {
            if (wnd->isModal()) {
                // qDebug() << "Window is modal, setting ColorGroup to Inactive in paintSection.";
                cg = DPalette::Inactive;
            } else {
                // qDebug() << "Window is active and not modal, setting ColorGroup to Active in paintSection.";
                cg = DPalette::Active;
            }
        }
#else
        // qDebug() << "ENABLE_INACTIVE_DISPLAY is not defined in paintSection, setting ColorGroup to Active.";
        cg = DPalette::Active;
//        cg = DPalette::Current;
#endif
    }
#if QT_VERSION_MAJOR > 5
    // qDebug() << "Using Qt6 or higher for application palette in paintSection.";
    DGuiApplicationHelper *dAppHelper = DGuiApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
#else
    // qDebug() << "Using Qt5 for application palette in paintSection.";
    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
#endif
    //    DPalette palette = dAppHelper->palette(this);

    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());

    QStyleOptionHeader option;
    initStyleOption(&option);
    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);

    // title
    QRect contentRect(rect.x(), rect.y(), rect.width(), rect.height() - m_spacing);
    QRect hSpacingRect(rect.x(), contentRect.height(), rect.width(),
                       rect.height() - contentRect.height());

    QBrush contentBrush(palette.color(cg, DPalette::Base));
    // horizontal spacing
    QBrush hSpacingBrush(palette.color(cg, DPalette::FrameBorder));
    //    QBrush hSpacingBrush(
    //        DApplicationHelper::instance()->palette(this).color(DPalette::FrameBorder));
    // vertical spacing
    QBrush vSpacingBrush(palette.color(cg, DPalette::FrameBorder));
    QRectF vSpacingRect(rect.x(), rect.y() + kSpacingMargin, m_spacing,
                        rect.height() - kSpacingMargin * 2);
    QBrush clearBrush(palette.color(cg, DPalette::Window));

    painter->fillRect(hSpacingRect, clearBrush);
    painter->fillRect(hSpacingRect, hSpacingBrush);

    if (visualIndex(logicalIndex) > 0) {
        // qDebug() << "Visual index is greater than 0, filling vertical spacing rects.";
        painter->fillRect(vSpacingRect, clearBrush);
        painter->fillRect(vSpacingRect, vSpacingBrush);
    }

    QPen foreground;
    foreground.setColor(palette.color(cg, DPalette::Text));
    QRect textRect;
    if (sortIndicatorSection() == logicalIndex) {
        // qDebug() << "Logical index matches sort indicator section, adjusting textRect for sort indicator.";
        textRect = {contentRect.x() + margin, contentRect.y() + 7,
                    contentRect.width() - margin * 3 - kDropDownSize.width(), contentRect.height()
                   };
    } else {
        // qDebug() << "Logical index does not match sort indicator section, setting normal textRect.";
        textRect = {contentRect.x() + margin, contentRect.y() + 7, contentRect.width() - margin,
                    contentRect.height()
                   };
    }
    QString title = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
    int align = model()->headerData(logicalIndex, orientation(), Qt::TextAlignmentRole).toInt();
    painter->setPen(foreground);
    painter->drawText(textRect, int(align), title);

    // sort indicator
    if (isSortIndicatorShown() && logicalIndex == sortIndicatorSection()) {
        // qDebug() << "Sort indicator is shown and logical index matches sort indicator section, drawing sort indicator.";
        QRect sortIndicator(textRect.x() + textRect.width() + margin,
                            textRect.y() + qCeil((textRect.height() - kDropDownSize.height()) / 2.),
                            kDropDownSize.width(), kDropDownSize.height());
        option.rect = sortIndicator;
        if (sortIndicatorOrder() == Qt::DescendingOrder) {
            // qDebug() << "Drawing descending sort indicator.";
            style->drawPrimitive(DStyle::PE_IndicatorArrowDown, &option, painter, this);
        } else if (sortIndicatorOrder() == Qt::AscendingOrder) {
            // qDebug() << "Drawing ascending sort indicator.";
            style->drawPrimitive(DStyle::PE_IndicatorArrowUp, &option, painter, this);
        }
    }

    painter->restore();
    // qDebug() << "DiskHealthHeaderView::paintSection completed.";
}

bool DiskHealthHeaderView::eventFilter(QObject *obj, QEvent *ev)
{
    // qDebug() << "DiskHealthHeaderView::eventFilter called for object:" << obj << ", event type:" << ev->type();
    if (ev->type() == QEvent::KeyPress) {
        qDebug() << "Event type is KeyPress.";
        QKeyEvent *kev = dynamic_cast<QKeyEvent *>(ev);
        if (kev->key() == Qt::Key_Space ||
                kev->key() == Qt::Key_Up ||
                kev->key() == Qt::Key_Down) {
            qDebug() << "Key is Space, Up, or Down. Filtering event.";
            return true;
        }
    }
    // qDebug() << "Calling base eventFilter. Event type:" << ev->type();
    return DHeaderView::eventFilter(obj, ev);
}
