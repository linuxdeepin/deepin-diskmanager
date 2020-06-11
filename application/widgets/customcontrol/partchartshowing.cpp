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
#define radius 8
#define space 11
#define rightspace 3
PartChartShowing::PartChartShowing(QWidget *parent)
    : QWidget(parent)
{
    QColor color("#70BEFF");
    QColor color1("#4293FF");
    QColor color2("#6751E4");
    QColor color3("#FA7404");
    QColor color4("#FFD027");
    QColor color5("#2CCBBE");
    basecolor = QVector<QColor> {color, color1, color2, color3, color4, color5};

//    connect(this, SIGNAL(mouseMove(QMouseEvent *)), this, SLOT(showMouseTip(QMouseEvent *)));
    setMouseTracking(true);
}

void PartChartShowing::getData(const double &totals, const QVector<double> sizeInfo)
{
    total = totals;
    partsize = sizeInfo;
}

void PartChartShowing::getflag(int mflag, int value)
{
    flag = mflag;
    sumvalue = value;
    update();
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
    QRect paintRect = QRect(5, 10, rect.width() - 5, 35);
    QPainterPath paintpath;

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
    QRect paintRect = QRect(3, 10, rect.width() - 4, 35);
    QPainterPath path0, path1, path2, path3, path4, path5, path6, path7, path8, path9, path10, path11, path12, path13, path14, path15, path16, path17, path18, path19, path20, path21, path22, path23;
    QVector<QPainterPath> path {path0, path1, path2, path3, path4, path5, path6, path7, path8, path9, path10, path11, path12, path13, path14, path15, path16, path17, path18, path19, path20, path21, path22, path23};
    QPainterPath paintpath;
    double sum = 0.00;
    if (flag == 1) {
        painter->setPen(QPen(QColor(this->palette().highlight().color()), 2));
        painter->drawRoundedRect(paintRect, 8, 8);
    }

    for (i = 0; i < partsize.size(); i++) {
        double widths = (partsize.at(i) / total) * (paintRect.width() - radius);
        double width1 = 0.00;
        widths = widths - rightspace;
        if (partsize.at(i) / total < 0.01 || widths < radius) {
            widths = 8;
        }
        sum = sum + partsize.at(i);
        sums = sum;
        if (i == 0) {
            if (static_cast<int>(sum) < static_cast<int>(total)) {
                path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
                path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                    QSize(radius * 2, radius * 2)),
                              180, 90);
                path[0].lineTo(paintRect.bottomLeft() + QPoint((static_cast<int>(widths) + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint((static_cast<int>(widths) + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
            }
//            qDebug() << path[0].currentPosition().x();
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
                painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 1));
                painter->drawPath(path[0]);
                number = -1;
                flag = 0;
            }

        } else if (static_cast<int>(sum) < static_cast<int>(total) && i > 0) {
            width1 = (partsize.at(i - 1) / total) * (paintRect.width() - radius);
            width1 = width1 - rightspace;
            if (width1 < 8 || partsize.at(i - 1) / total < 0.01)
                width1 = 8;
            if (path[i - 1].currentPosition().x() + width1 + widths > paintRect.width() - radius) {
                emit judgeLastPartition();
            }
            path[i].moveTo(path[i - 1].currentPosition() + QPoint(static_cast<int>(width1), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(int(width1 + widths), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(int(width1 + widths), paintRect.height() - 1));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((static_cast<int>(width1)), paintRect.height() - 1));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((static_cast<int>(width1)), 0));
            QColor fillcolor;
            if (i > basecolor.size() - 1) {
                fillcolor = basecolor.at(i % (basecolor.size() - 1));
            } else {
                fillcolor = basecolor.at(i);
            }
            painter->fillPath(path[i], QBrush(fillcolor));
            if (number > 0 && number != -1 && number == i) {
                painter->setBrush(fillcolor);
                painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 1));
                painter->drawPath(path[number]);
                flag = 0;
            }
        } else if (sumvalue >= 100 || sum >= total) {
            double width = ((partsize.at(partsize.size() - 2) / total)) * (paintRect.width() - radius) - rightspace;
            if (partsize.at(partsize.size() - 2) / total < 0.01) {
                width = 8;
            }
            path[partsize.size() - 1].moveTo(path[partsize.size() - 2].currentPosition() + QPoint(int((partsize.at(partsize.size() - 2) / total) * (paintRect.width() - radius)), 0));
            path[partsize.size() - 1].lineTo(paintRect.topRight() - QPoint(radius, 0));
            path[partsize.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                  QSize(radius * 2, radius * 2)),
                                            90, -90);
            path[partsize.size() - 1].lineTo(paintRect.bottomRight() - QPoint(0, radius));

            path[partsize.size() - 1].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                                  QSize(radius * 2, radius * 2)),
                                            0, -90);
            path[partsize.size() - 1].lineTo(path[partsize.size() - 2].currentPosition() + QPoint(static_cast<int>(width), paintRect.height() - 1));
            path[partsize.size() - 1].lineTo(path[partsize.size() - 2].currentPosition() + QPoint(static_cast<int>(width), 0));
            QColor fillcolor;
            if (i > basecolor.size() - 1) {
                fillcolor = basecolor.at(i % (basecolor.size() - 1));
            } else {
                fillcolor = basecolor.at(i);
            }
            painter->fillPath(path[partsize.size() - 1], QBrush(fillcolor));
            if (number == partsize.size() - 1) {
                painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 1));
                painter->setBrush(fillcolor);
                painter->drawPath(path[partsize.size() - 1]);
                number = -1;
            }
        }

    }
    if (flag == 3 && sum < total) {
        double width2 = (partsize.at(partsize.size() - 1) / total) * (paintRect.width() - radius) - rightspace;
        if (partsize.at(partsize.size() - 1) / total < 0.01) {
            width2 = 8;
        }
        painter->setPen(QPen(this->palette().color(DPalette::Normal, DPalette::Highlight), 1));
        //                paintpath.moveTo()
        paintpath.moveTo(paintRect.bottomRight() - QPoint(0, radius));
        paintpath.lineTo(paintRect.topRight() + QPoint(0, radius));
        paintpath.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                              QSize(radius * 2, radius * 2)),
                        0, 90);
        paintpath.lineTo(QPoint(static_cast<int>(allpath[partsize.size() - 1].currentPosition().x()) + static_cast<int>(width2), 10));
        paintpath.lineTo(QPoint(static_cast<int>(allpath[partsize.size() - 1].currentPosition().x()) + static_cast<int>(width2), paintRect.height() + 9));
        paintpath.lineTo(paintRect.bottomRight() - QPoint(radius, 0));
        paintpath.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                              QSize(radius * 2, radius * 2)),
                        270, 90);
        painter->drawPath(paintpath);
        flag = 0;
    }
    allpath = path;
}

void PartChartShowing::mousePressEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    if (partsize.size() == 0) {
        if (event->button() == Qt::LeftButton) {
            if (x > 0 && x < this->width() && y > 10 && y < 45) {
                flag = 1;
                emit sendFlag(flag, number, x);
                update();
            }
        }
    } else if (partsize.size() > 0) {
        if (event->button() == Qt::LeftButton) {
            for (i = 0; i < partsize.size(); i++) {
                double width = ((partsize.at(i) / total) * (this->width() - space)) - rightspace;
                if (partsize.at(i) / total < 0.01) {
                    width = 8;
                }
                if ((x > allpath[i].currentPosition().x() && x < (allpath[i].currentPosition().x() + width) && y > 10 && y < 45) || (i == 0 && y > 10 && y < 45)) {
                    flag = 2;
                    number = i;
                    emit sendFlag(flag, number, x);
                    update();
                }
            }
            //空闲分区
            double width1 = ((partsize.at(partsize.size() - 1) / total) * (this->width() - space)) - rightspace;
            if (partsize.size() > 1) {
                if (partsize.at(partsize.size() - 1) / total < 0.01) {
                    width1 = 8;
                }
            }
            if (x > allpath[partsize.size() - 1].currentPosition().x() + width1 && y > 10 && y < 45 && int(sums) < int(total)) {
                flag = 3;
                i = -1;
                number = 100;
                emit sendFlag(flag, number, x);
                update();
//                qDebug() << x << allpath[partsize.size() - 1].curentPosition().x() + width1 << this->width();
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
    if (event->button() == Qt::LeftButton) {
//        emit sendFlag(flag, number, x);
    }
}


void PartChartShowing::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    if (partsize.size() == 0) {

        if (x > 0 && x < this->width() && y > 10 && y < 45) {
//            this->setToolTip("1212121");

            emit sendMoveFlag(1, number, x);

        }

    } else if (partsize.size() > 0) {

        for (i = 0; i < partsize.size(); i++) {
            double width = ((partsize.at(i) / total) * (this->width() - space)) - rightspace;
            if (partsize.at(i) / total < 0.01) {
                width = 8;
            }
            if ((x > allpath[i].currentPosition().x() && x < (allpath[i].currentPosition().x() + width) && y > 10 && y < 40) || (i == 0 && y > 10 && y < 40)) {
                number = i;
                emit sendMoveFlag(2, number, x);
            }
        }
        //空闲分区
        double width1 = ((partsize.at(partsize.size() - 1) / total) * (this->width() - space)) - rightspace;
        if (partsize.size() > 1) {
            if (partsize.at(partsize.size() - 1) / total < 0.01) {
                width1 = 8;
            }
        }
        if (x > allpath[partsize.size() - 1].currentPosition().x() + width1 && y > 10 && y < 45 && int(sums) < int(total)) {
            i = -1;
            emit sendMoveFlag(3, number, x);
        }
    }

}

void PartChartShowing::enterEvent(QEvent *event)
{

}


