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
#include "piechartwidget.h"
#include <QDebug>

extern QList<qreal> dataValue;
extern QList<QString> strName;
PieChartWidget::PieChartWidget(QWidget *parent) : QWidget(parent)
{
    color << QColor(208, 234, 255)  << QColor(210, 236, 205) << QColor(235, 236, 205) << QColor(180, 160, 205) << QColor(190, 190, 205)
          << QColor(154, 234, 124)  << QColor(135, 150, 150) << QColor(142, 190, 200) << QColor(196, 120, 190) << QColor(130, 120, 130);
//    update();
}

int PieChartWidget::getAlldata(QList<qreal> list)
{
    int a = 0;
    for (int i = 0; i < list.count(); i++) {
        a = int(a + list.at(i));
    }
    return a;
}

void PieChartWidget::paintEvent(QPaintEvent *event)
{
    QRectF rectangle1(20.0, 20.0, 300.0, 300.0);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
//    painter.rotate();
    painter.setPen(QPen(QColor(154, 186, 241), 3));
    painter.drawEllipse(rectangle1);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(color.at(0))));
    QRectF rectangle2(20.0, 20.0, 300.0, 300.0);

    painter.drawPie(rectangle2, 0 * 16, 360 * 16);


    drawPie(&painter);
    QWidget::paintEvent(event);

//    update();
}

void PieChartWidget::drawPie(QPainter *painter)
{
    qreal sum = 0;
    qreal mid = 0;
    qreal rec = 0;

    for (int i = 0; i < dataValue.count(); i++) {
        qreal value = dataValue.at(i);
        all = getAlldata(dataValue);
//        qDebug() << all;
        qreal len = (value / all) * 360;

        painter->setPen(Qt::NoPen);
        painter->setBrush(color.at(i));
        painter->drawPie(20.0, 20.0, 300.0, 300.0, int(rec * 16), int(len * 16));
        rec = rec + len;

        mid = qreal(sum + len / 2);
        sum = sum + len;

        painter->setPen(QColor(Qt::black));
        painter->drawText(QPoint(170 + 75 * qCos((mid * 3.14) / 180) - strName.at(i).length(), 170 - 75 * qSin((mid * 3.14) / 180)), strName.at(i));

    }
    sum = 0;
}
