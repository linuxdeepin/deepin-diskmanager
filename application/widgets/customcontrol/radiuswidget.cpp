// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "radiuswidget.h"

RadiusWidget::RadiusWidget(QWidget *parent) : QWidget(parent)
{
    qDebug() << "RadiusWidget constructor called.";
    m_leftTop = false;
    m_leftBottom = false;
    m_rightTop = false;
    m_rightBottom = false;
    m_isEnter = false;
    m_isChecked = false;
}

void RadiusWidget::setMode(bool leftTop, bool leftBottom, bool rightTop, bool rightBottom)
{
    qDebug() << "Setting corner modes - LT:" << leftTop
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
    qDebug() << "Setting checked state:" << isChecked;
    m_isChecked = isChecked;
    update();
}

void RadiusWidget::paintEvent(QPaintEvent *event)
{
    // qDebug() << "RadiusWidget::paintEvent called.";
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;

    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    // qDebug() << "Theme type:" << themeType;
    if (themeType == DGuiApplicationHelper::LightType) {
        // qDebug() << "Theme is LightType.";
        if (m_isChecked) {
            // qDebug() << "m_isChecked is true, setting brush for light theme.";
            painter.setBrush(QBrush(QColor(0, 0, 0, 30)));
        } else {
            // qDebug() << "m_isChecked is false for light theme.";
            if (m_isEnter) {
                // qDebug() << "m_isEnter is true, setting brush for light theme.";
                painter.setBrush(QBrush(QColor(0, 0, 0, 30)));
            } else {
                // qDebug() << "m_isEnter is false, setting brush for light theme.";
                painter.setBrush(QBrush(QColor(0, 0, 0, 10)));
            }
        }
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        // qDebug() << "Theme is DarkType.";
        if (m_isChecked) {
            // qDebug() << "m_isChecked is true, setting brush for dark theme.";
            painter.setBrush(QBrush(QColor(255, 255, 255, 30)));
        } else {
            // qDebug() << "m_isChecked is false for dark theme.";
            if (m_isEnter) {
                // qDebug() << "m_isEnter is true, setting brush for dark theme.";
                painter.setBrush(QBrush(QColor(255, 255, 255, 30)));
            } else {
                // qDebug() << "m_isEnter is false, setting brush for dark theme.";
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
    // qDebug() << "Drawing top polygon.";

    QPointF pointscenter[4] = {
        QPointF(rect.topLeft().x(), rect.topLeft().y() + 10),
        QPointF(rect.bottomLeft().x(), rect.bottomLeft().y() - 10),
        QPointF(rect.bottomRight().x(), rect.bottomRight().y() - 10),
        QPointF(rect.topRight().x(), rect.topRight().y() + 10)

    };
    painter.drawPolygon(pointscenter, 4);
    // qDebug() << "Drawing center polygon.";

    QPointF pointsbottom[4] = {
        QPointF(rect.bottomLeft().x() + 10, rect.bottomLeft().y() - 10),
        QPointF(rect.bottomLeft().x() + 10, rect.bottomLeft().y()),
        QPointF(rect.bottomRight().x() - 10, rect.bottomRight().y()),
        QPointF(rect.bottomRight().x() - 10, rect.bottomRight().y() - 10)
    };
    painter.drawPolygon(pointsbottom, 4);
    // qDebug() << "Drawing bottom polygon.";

    //给各个角画直角
    if (m_leftTop) {
        // qDebug() << "Drawing leftTop corner.";
        QRectF rectf(QPointF(rect.topLeft().x(), rect.topLeft().y()), QSizeF(20, 20));
        painter.drawPie(rectf, 90 * 16, 90 * 16);
    } else {
        // qDebug() << "Drawing leftTop straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.topLeft()), QSize(10, 10)));
    }

    if (m_leftBottom) {
        // qDebug() << "Drawing leftBottom corner.";
        QRectF rectf(QPointF(rect.bottomLeft().x(), rect.bottomLeft().y() - 20), QSizeF(20, 20));
        painter.drawPie(rectf, 180 * 16, 90 * 16);
    } else {
        // qDebug() << "Drawing leftBottom straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.bottomLeft().x(), rect.bottomLeft().y() - 10), QSize(10, 10)));
    }

    if (m_rightTop) {
        // qDebug() << "Drawing rightTop corner.";
        QRectF rectf(QPointF(rect.topRight().x() - 20, rect.topRight().y()), QSizeF(20, 20));
        painter.drawPie(rectf, 0 * 16, 90 * 16);
    } else {
        // qDebug() << "Drawing rightTop straight corner.";
        painter.drawPolygon(QRect(QPoint(rect.topRight().x() - 10, rect.topRight().y()), QSize(10, 10)));
    }

    if (m_rightBottom) {
        // qDebug() << "Drawing rightBottom corner.";
        QRectF rectf(QPointF(rect.bottomRight().x() - 20, rect.bottomRight().y() - 20), QSizeF(20, 20));
        painter.drawPie(rectf, 270 * 16, 90 * 16);
    } else {
        // qDebug() << "Drawing rightBottom straight corner.";
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
    // qDebug() << "RadiusWidget::enterEvent called.";
    Q_UNUSED(event);
    m_isEnter =true;
    update();
}

void RadiusWidget::leaveEvent(QEvent *event)
{
    // qDebug() << "RadiusWidget::leaveEvent called.";
    Q_UNUSED(event);
    m_isEnter = false;
    update();
}


