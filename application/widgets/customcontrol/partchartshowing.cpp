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
#include <QHBoxLayout>
PartChartShowing::PartChartShowing(QWidget *parent) : QWidget(parent)
{

}


void PartChartShowing::getData(const double &totals, const QVector<double>sizeInfo)
{
//    int i = totals.lastIndexOf("G");
    QColor color("#70BEFF");
    QColor color1("#4293FF");
    QColor color2("#6751E4");
    QColor color3("#FA7404");
    QColor color4("#FFD027");
    QColor color5("#2CCBBE");
    basecolor = QVector<QColor> {color, color1, color2, color3, color4, color5};
    total = totals;
    partsize = sizeInfo;
//    total = static_cast<int>(totals);
}

void PartChartShowing::getflag(int mflag, int value)
{
    flag = mflag;
    sumvalue = value;
    update();
}

//void PartChartShowing::slotvalue(int value)
//{
//    sumvalue += value / 100.00;
//    qDebug() << value / 100.00;;
//    qDebug() << sumvalue;
//}

void PartChartShowing::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect;
    rect.setX(this->x());
    rect.setY(this->y());
    rect.setWidth(this->width());
    QRect paintRect = QRect(5, 10, rect.width() - 3, 35);
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

    QColor fillColor = QColor(this->palette().dark().color()); //this->palette().color(DPalette::Normal, DPalette::Highlight)
    painter.setBrush(QBrush(fillColor));
    painter.fillPath(paintpath, fillColor);
    painter.setPen(QPen(QColor(fillColor), 3));
    painter.drawLine(paintRect.topLeft().x() + radius, paintRect.topLeft().y() + radius, paintRect.topLeft().x() + radius + paintRect.width(), paintRect.bottomLeft().y() - radius);
    QColor tipColor = QColor(this->palette().dark().color());


    painter.setPen(tipColor);
//    QFont font;
//    font.setBold(true);
//    painter.setFont(font);
    painter.drawRect(this->x(), this->y() + 50, 10, 10);

    painter.setPen(this->palette().text().color());
    QRect textRect(this->x() + 15, this->y() + 42, 100, 50);
    painter.drawText(textRect, tr("Unallocated"));


    QWidget::paintEvent(event);

    addPaint(&painter);
    painter.restore();


}

void PartChartShowing::addPaint(QPainter *painter)
{

    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    QRect rect;
    rect.setX(this->x());
    rect.setY(this->y());
    rect.setWidth(this->width());
    QRect paintRect = QRect(3, 10, rect.width() - 3, 35);
    QPainterPath path0, path1, path2, path3, path4, path5, path6, path7, path8, path9, path10, path11, path12, path13, path14, path15, path16, path17, path18, path19, path20;
    QVector<QPainterPath>path{path0, path1, path2, path3, path4, path5, path6, path7, path8, path9, path10, path11, path12, path13, path14, path15, path16, path17, path18, path19, path20};
    QPainterPath paintpath;
    const int radius = 8;
    double sum = 0.00;
    if (flag == 1) {
        painter->setPen(QPen(QColor(this->palette().highlight().color()), 2));
        painter->drawRoundedRect(paintRect, 8, 8);
    }

    for (i = 0; i < partsize.size(); i++) {
        double widths = (partsize.at(i) / total) * (paintRect.width() - 8);
        double width1 = 0.00;
        sum = sum + partsize.at(i);
        qDebug() << sumvalue << "sumvalue";
        if (i == 0) {
            if (static_cast<int>(sum) < static_cast<int>(total)) {
                path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
                path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                    QSize(radius * 2, radius * 2)),
                              180, 90);
                path[0].lineTo(paintRect.bottomLeft() + QPoint((widths + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint((widths + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
            }
            if (static_cast<int>(sum) >= static_cast<int>(total)) {
                path[0].moveTo(paintRect.bottomRight() - QPoint(0, radius));
                path[0].lineTo(paintRect.topRight() + QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                    QSize(radius * 2, radius * 2)),
                              0, 90);
                path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
                path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                    QSize(radius * 2, radius * 2)),
                              180, 90);
                path[0].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                    QSize(radius * 2, radius * 2)),
                              270, 90);

            }
            painter->fillPath(path[0], QBrush(basecolor.at(0)));
            if (number == 0) {
                painter->setBrush(QBrush(basecolor.at(0)));
                painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 2));
                painter->drawPath(path[0]);
                number = -1;
                flag = 0;
            }

        } else if (static_cast<int>(sum) < static_cast<int>(total) && i > 0) {
            width1 = (partsize.at(i - 1) / total) * (paintRect.width() - 8);
            path[i].moveTo(path[i - 1].currentPosition() + QPoint(width1, 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(int(width1 + widths), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(int(width1 + widths), paintRect.height() - 1));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((width1), paintRect.height() - 1));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((width1), 0));
            if (path[i - 1].currentPosition().x() + width1 + widths > this->width() - 8) {
                emit judgeLastPartition();
            }
            QColor fillcolor;
            if (i > basecolor.size() - 1) {
                fillcolor = basecolor.at(i % basecolor.size());
            } else {
                fillcolor = basecolor.at(i);
            }
            painter->fillPath(path[i], QBrush(fillcolor));
            if (number > 0 && number != -1 && number == i) {
                painter->setBrush(fillcolor);
                painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 2));
                painter->drawPath(path[number]);
                flag = 0;
            }
        } else if (sumvalue >= 100) {
            path[partsize.size() - 1].moveTo(path[partsize.size() - 2].currentPosition() + QPoint(int((partsize.at(partsize.size() - 2) / total) * (paintRect.width())), 0));
            path[partsize.size() - 1].lineTo(paintRect.topRight() - QPoint(radius, 0));
            path[partsize.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                  QSize(radius * 2, radius * 2)),
                                            90, -90);
            path[partsize.size() - 1].lineTo(paintRect.bottomRight() - QPoint(0, radius));

            path[partsize.size() - 1].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                                  QSize(radius * 2, radius * 2)),
                                            0, -90);
            path[partsize.size() - 1].lineTo(path[partsize.size() - 2].currentPosition() + QPoint(((partsize.at(partsize.size() - 2) / total))*paintRect.width(), paintRect.height() - 1));
            path[partsize.size() - 1].lineTo(path[partsize.size() - 2].currentPosition() + QPoint(((partsize.at(partsize.size() - 2) / total))*paintRect.width(), 0));
            QColor fillcolor;
            if (i > basecolor.size() - 1) {
                fillcolor = basecolor.at(i % basecolor.size());
            } else {
                fillcolor = basecolor.at(i);
            }
            painter->fillPath(path[partsize.size() - 1], QBrush(fillcolor));
            if (number == partsize.size() - 1) {
                painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 2));
                painter->setBrush(fillcolor);
                painter->drawPath(path[partsize.size() - 1]);
                number = -1;
            }
        }
        allpath = path;
    }
    if (flag == 3) {
        double width2 = (partsize.at(partsize.size() - 1) / total) * (this->width());
//        if (width2 <= 3) {
//            width2 = 10;
//        }
        painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 2));
        //                paintpath.moveTo()
        paintpath.moveTo(paintRect.bottomRight() - QPoint(0, radius));
        paintpath.lineTo(paintRect.topRight() + QPoint(0, radius));
        paintpath.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                              QSize(radius * 2, radius * 2)),
                        0, 90);
        paintpath.lineTo(QPoint(allpath[partsize.size() - 1].currentPosition().x() + width2 - 1, 10));
        paintpath.lineTo(QPoint(allpath[partsize.size() - 1].currentPosition().x() + width2 - 1, paintRect.height() + 9));
        paintpath.lineTo(paintRect.bottomRight() - QPoint(radius, 0));
        paintpath.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                              QSize(radius * 2, radius * 2)),
                        270, 90);
        painter->drawPath(paintpath);
        flag = 0;
    }

}

void PartChartShowing::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    int i ;
    if (partsize.size() == 0) {
        if (event->button() == Qt::LeftButton) {
            if (x > 0 && x < this->width() && y > 10 && y < 45) {
                flag = 1;
                update();
            }

        }
    } else if (partsize.size() > 0) {
        if (event->button() == Qt::LeftButton) {
            for (i = 0; i < partsize.size(); i++) {
                double width = ((partsize.at(i) / total) * (this->width()));
//                if (width <= 3) {
//                    width = 10;
//                }
                if ((x > allpath[i].currentPosition().x() && x < (allpath[i].currentPosition().x() + width) && y > 10 && y < 45) || (i == 0 && y > 10 && y < 45)) {
                    flag = 2;
                    number = i;
                    update();
                }
            }
            double width1 = ((partsize.at(partsize.size() - 1) / total) * (this->width()));
            //空闲分区
            if (x > allpath[partsize.size() - 1].currentPosition().x() + width1 && x < this->width() && y > 10 && y < 45 && static_cast<int>(sum) < static_cast<int>(total)) {
                flag = 3;
                i = -1;
                number = 100;
                update();
            }

        }
    }
    if (event->button() == Qt::LeftButton) {
        if (!(x > 0 && x < this->width() && y > 10 && y < 45)) {
            flag = 0;
            number = -1;
            update();
        }
    }

    emit sendFlag(flag, number, x);
}

