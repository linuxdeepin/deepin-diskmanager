/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     zhangkai <zhangkai@uniontech.com>
* Maintainer: zhangkai <zhangkai@uniontech.com>
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
#include "partchartshowing.h"
#include <QDebug>
PartChartShowing::PartChartShowing(QWidget *parent) : QWidget(parent)
{

}



PartChartShowing::PartChartShowing(int used, int trueused, int noused, QWidget *parent): m_used(used), m_tureused(trueused), m_noused(noused)
{

}
void PartChartShowing::getData(int used, int trueused, int noused)
{
    m_used = used;
    m_tureused = trueused;
    m_noused = noused;
    qDebug() << used << trueused << noused;

}
void PartChartShowing::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect;
    rect.setX(this->x());
    rect.setY(this->y());
    rect.setWidth(this->width());
    QRect paintRect = QRect(0, 10, rect.width(), 35);
    QPainterPath paintpath;
    const int radius = 8;

    paintpath.moveTo(paintRect.bottomRight() - QPoint(0, radius));
    paintpath.lineTo(paintRect.topRight() + QPoint(0, radius));
    paintpath.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                          QSize(radius * 2, radius * 2)),
                    0, 90);
    paintpath.lineTo(paintRect.topLeft() + QPoint(radius, 0));
    paintpath.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
    paintpath.lineTo(paintRect.bottomLeft() - QPoint(0, radius));
    paintpath.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                          QSize(radius * 2, radius * 2)),
                    180, 90);
    paintpath.lineTo(paintRect.bottomRight() - QPoint(radius, 0));
    paintpath.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                          QSize(radius * 2, radius * 2)),
                    270, 90);

    QColor fillColor = QColor(210, 210, 210); //this->palette().color(DPalette::Normal, DPalette::Highlight)
    painter.setBrush(QBrush(fillColor));
    painter.fillPath(paintpath, fillColor);
    painter.setPen(QPen(QColor(fillColor), 3));
    const int total = m_used + m_tureused + m_noused;
    painter.drawLine(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used), paintRect.topLeft().y() + radius, paintRect.topLeft().x() + radius + paintRect.width() / (total / m_used), paintRect.bottomLeft().y() - radius);
//    QPainterPath paintpath1;
//    paintpath1.moveTo(QPoint(paintRect.topLeft().x() + radius + paintRect.width() / (total / m_used), paintRect.topLeft().y()));
//    paintpath1.lineTo(QPoint(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used) + paintRect.width() / (total / m_tureused), paintRect.topLeft().y()));
//    paintpath1.lineTo(QPoint(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used) + paintRect.width() / (total / m_tureused), paintRect.bottomRight().y()));
//    paintpath1.lineTo(QPoint(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used), paintRect.bottomRight().y()));
//    QColor fillcolor1 = Qt::green;
//    painter.setBrush(QBrush(fillcolor1));
//    painter.fillPath(paintpath1, fillcolor1);
//    QPainterPath paintpath2;
//    paintpath2.moveTo(paintRect.bottomRight() - QPoint(0, radius));
//    paintpath2.lineTo(paintRect.topRight() + QPoint(0, radius));
//    paintpath2.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
//                           QSize(radius * 2, radius * 2)),
//                     0, 90);
//    paintpath2.lineTo(QPoint(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used) + paintRect.width() / (total / m_tureused), paintRect.topLeft().y()));
//    paintpath2.lineTo(QPoint(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used) + paintRect.width() / (total / m_tureused), paintRect.bottomLeft().y()));
//    paintpath2.lineTo(QPoint(paintRect.topRight().x() - radius, paintRect.bottomLeft().y()));
//    paintpath2.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
//                           QSize(radius * 2, radius * 2)),
//                     270, 90);
//    QColor fillcolor2 = Qt::gray;
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setBrush(QBrush(fillcolor2));
//    painter.fillPath(paintpath2, fillcolor2);

    QWidget::paintEvent(event);

    update();
    painter.restore();


}
