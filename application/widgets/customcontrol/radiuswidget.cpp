// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "radiuswidget.h"

RadiusWidget::RadiusWidget(QWidget *parent) : QWidget(parent)
{
    qCDebug(appLog) << "RadiusWidget constructor called.";
    m_leftTop = false;
    m_leftBottom = false;
    m_rightTop = false;
    m_rightBottom = false;
    m_isEnter = false;
    m_isChecked = false;
}

void RadiusWidget::setMode(bool leftTop, bool leftBottom, bool rightTop, bool rightBottom)
{
    qCDebug(appLog) << "Setting corner modes - LT:" << leftTop
                           << " LB:" << leftBottom
                           << " RT:" << rightTop
                           << " RB:" << rightBottom;
    m_leftTop = leftTop;
    m_leftBottom = leftBottom;
    m_rightTop = rightTop;
    m_rightBottom = rightBottom;
}

void RadiusWidget::setChecked(bool isChecked)
{
    qCDebug(appLog) << "Setting checked state:" << isChecked;
    m_isChecked = isChecked;
    update();
}

void RadiusWidget::paintEvent(QPaintEvent *event)
{
    // qCDebug(appLog) << "RadiusWidget::paintEvent called.";
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;

    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    // qCDebug(appLog) << "Theme type:" << themeType;
    if (themeType == DGuiApplicationHelper::LightType) {
        // qCDebug(appLog) << "Theme is LightType.";
        if (m_isChecked) {
            // qCDebug(appLog) << "m_isChecked is true, setting brush for light theme.";
            painter.setBrush(QBrush(QColor(0, 0, 0, 30)));
        } else {
            // qCDebug(appLog) << "m_isChecked is false for light theme.";
            if (m_isEnter) {
                // qCDebug(appLog) << "m_isEnter is true, setting brush for light theme.";
                painter.setBrush(QBrush(QColor(0, 0, 0, 30)));
            } else {
                // qCDebug(appLog) << "m_isEnter is false, setting brush for light theme.";
                painter.setBrush(QBrush(QColor(0, 0, 0, 10)));
            }
        }
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        // qCDebug(appLog) << "Theme is DarkType.";
        if (m_isChecked) {
            // qCDebug(appLog) << "m_isChecked is true, setting brush for dark theme.";
            painter.setBrush(QBrush(QColor(255, 255, 255, 30)));
        } else {
            // qCDebug(appLog) << "m_isChecked is false for dark theme.";
            if (m_isEnter) {
                // qCDebug(appLog) << "m_isEnter is true, setting brush for dark theme.";
                painter.setBrush(QBrush(QColor(255, 255, 255, 30)));
            } else {
                // qCDebug(appLog) << "m_isEnter is false, setting brush for dark theme.";
                painter.setBrush(QBrush(QColor(255, 255, 255, 10)));
            }
        }
    }

    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width());
    rect.setHeight(rect.height());

    QPointF pointstop[4] = {
        QPointF(rect.topLeft().x() + 10, rect.topLeft().y()),
        QPointF(rect.topLeft().x() + 10, rect.topLeft().y() + 10),
        QPointF(rect.topRight().x() - 10, rect.topLeft().y() + 10),
        QPointF(rect.topRight().x() - 10, rect.topLeft().y())
    };
    painter.drawPolygon(pointstop, 4);
    // qCDebug(appLog) << "Drawing top polygon.";

    QPointF pointscenter[4] = {
        QPointF(rect.topLeft().x(), rect.topLeft().y() + 10),
        QPointF(rect.bottomLeft().x(), rect.bottomLeft().y() - 10),
        QPointF(rect.bottomRight().x(), rect.bottomRight().y() - 10),
        QPointF(rect.topRight().x(), rect.topRight().y() + 10)

    };
    painter.drawPolygon(pointscenter, 4);
    // qCDebug(appLog) << "Drawing center polygon.";

    QPointF pointsbottom[4] = {
        QPointF(rect.bottomLeft().x() + 10, rect.bottomLeft().y() - 10),
        QPointF(rect.bottomLeft().x() + 10, rect.bottomLeft().y()),
        QPointF(rect.bottomRight().x() - 10, rect.bottomRight().y()),
        QPointF(rect.bottomRight().x() - 10, rect.bottomRight().y() - 10)
    };
    painter.drawPolygon(pointsbottom, 4);
    // qCDebug(appLog) << "Drawing bottom polygon.";

    //给各个角画直角
    if (m_leftTop) {
        // qCDebug(appLog) << "Drawing leftTop corner.";
        QRectF rectf(QPointF(rect.topLeft().x(), rect.topLeft().y()), QSizeF(20, 20));
        painter.drawPie(rectf, 90 * 16, 90 * 16);
    } else {
        // qCDebug(appLog) << "Drawing leftTop straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.topLeft()), QSize(10, 10)));
    }

    if (m_leftBottom) {
        // qCDebug(appLog) << "Drawing leftBottom corner.";
        QRectF rectf(QPointF(rect.bottomLeft().x(), rect.bottomLeft().y() - 20), QSizeF(20, 20));
        painter.drawPie(rectf, 180 * 16, 90 * 16);
    } else {
        // qCDebug(appLog) << "Drawing leftBottom straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - 10), QSize(10, 10)));
    }

    if (m_rightTop) {
        // qCDebug(appLog) << "Drawing rightTop corner.";
        QRectF rectf(QPointF(rect.topRight().x() - 20, rect.topRight().y()), QSizeF(20, 20));
        painter.drawPie(rectf, 0 * 16, 90 * 16);
    } else {
        // qCDebug(appLog) << "Drawing rightTop straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.topRight().x() - 10, rect.topRight().y()), QSize(10, 10)));
    }

    if (m_rightBottom) {
        // qCDebug(appLog) << "Drawing rightBottom corner.";
        QRectF rectf(QPointF(rect.bottomRight().x() - 20, rect.bottomRight().y() - 20), QSizeF(20, 20));
        painter.drawPie(rectf, 270 * 16, 90 * 16);
    } else {
        // qCDebug(appLog) << "Drawing rightBottom straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.bottomRight().x() - 10, rect.bottomRight().y() - 10), QSize(10, 10)));
    }

    QWidget::paintEvent(event);
}

#if QT_VERSION_MAJOR > 5
void RadiusWidget::enterEvent(QEnterEvent *event)
#else
void RadiusWidget::enterEvent(QEvent *event)
#endif
{
    // qCDebug(appLog) << "RadiusWidget::enterEvent called.";
    Q_UNUSED(event);
    m_isEnter =true;
    update();
}

void RadiusWidget::leaveEvent(QEvent *event)
{
    // qCDebug(appLog) << "RadiusWidget::leaveEvent called.";
    Q_UNUSED(event);
    m_isEnter = false;
    update();
}


