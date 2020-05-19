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
#include "sizeinfowidget.h"
#include <QDebug>

SizeInfoWidget::SizeInfoWidget(QWidget *parent) : QWidget(parent)
{

}
SizeInfoWidget::SizeInfoWidget(double used, double unused, bool flag, QWidget *parent): QWidget(parent), m_used(used), m_noused(unused), m_flag(flag)
{

}

void SizeInfoWidget::setdata(PartitionInfo info, QVector<QColor>color, QVector<double>size, bool flag)
{
    sizeinfo = size;
    colorinfo = color;
    m_flag = flag;
    m_noused = Utils::sector_to_unit(info.sectors_unused, info.sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sector_to_unit(info.sectors_used, info.sector_size, SIZE_UNIT::UNIT_GIB);
    m_totalsize = m_noused + m_used;
    update();

}

//void SizeInfoWidget::settextdata(, bool flag)
//{
//    m_noused = Utils::sector_to_unit(info.sectors_unused, info.sector_size, SIZE_UNIT::UNIT_GIB);
//    m_used = Utils::sector_to_unit(info.sectors_used, info.sector_size, SIZE_UNIT::UNIT_GIB);
//    m_flag = flag;
//    QColor fillcolor = this->palette().color(DPalette::Normal, DPalette::Highlight);
//    QColor fillcolor1 = this->palette().color(DPalette::Normal, DPalette::ToolTipText);
//    fillcolor1.setAlphaF(0.1);
//    QVector<QColor>color{fillcolor, fillcolor1};
//    QVector<double>size{m_used, m_noused};
//    update();


//}
void SizeInfoWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect;
    QPainterPath path0, path1, path2, path3, path4, path5, path6, path7, path8, path9;
    QVector<QPainterPath>path{path0, path1, path2, path3, path4, path5, path6, path7, path8, path9};
    rect.setX(this->x());
    rect.setY(this->y());
    rect.setWidth(this->width());
    rect.setHeight(this->height());
    QRect paintRect = QRect(0, 60, rect.width(), rect.height() - 150);
    QPainterPath paintpath0;
    const int radius = 8;
//    paintpath.moveTo(paintRect.bottomLeft() - QPoint(0, radius));
//    paintpath.lineTo(paintRect.topLeft() + QPoint(0, radius));
//    paintpath.arcTo(QRect(QPoint(paintRect.topLeft() + QPoint(radius * 2, 0)),
//                          QSize(radius * 2, radius * 2)),
//                    0, 90);
//    paintpath.lineTo(paintRect.topLeft() + QPoint(m_used / m_total * paintRect.width(), 0));
//    paintpath.lineTo(paintRect.topLeft() + QPoint(m_used / m_total * paintRect.width(), 0) + QPoint(0, paintRect.height()));
//    paintpath.lineTo(paintRect.bottomLeft() + QPoint(radius, 0));
//    paintpath.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
//                          QSize(radius * 2, radius * 2)),
//                    180, 90);
    const double total = m_used + m_noused;
//            QPainterPath paintpath;
    //根据color和size数据遍历绘制矩形
    for (int i = 0; i < sizeinfo.size(); i++) {
        if (i == 0) {
            path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
            path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
            path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
            path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                QSize(radius * 2, radius * 2)),
                          180, 90);
            path[0].lineTo(paintRect.bottomLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius, 0));
            path[0].lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius, 0));
            path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
            painter.setBrush(QBrush(colorinfo[0]));
            painter.setPen(QPen(QColor(colorinfo[0]), 3));
            painter.fillPath(path[0], colorinfo[0]);
        } else if (i > 0 && i < sizeinfo.size() - 1) {
            path[i].moveTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize)*paintRect.width(), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize)*paintRect.width() + (sizeinfo[i] / m_totalsize)*paintRect.width(), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize)*paintRect.width() + (sizeinfo[i] / m_totalsize)*paintRect.width(), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize)*paintRect.width(), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize)*paintRect.width(), 0));
            painter.setBrush(QBrush(colorinfo[i]));
            painter.setPen(QPen(QColor(colorinfo[i]), 3));
            painter.fillPath(path[i], colorinfo[i]);
        } else if (i == sizeinfo.size() - 1) {
            path[sizeinfo.size() - 1].moveTo(paintRect.bottomRight() - QPoint(0, radius));
            path[sizeinfo.size() - 1].lineTo(paintRect.topRight() + QPoint(0, radius));
            path[sizeinfo.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                  QSize(radius * 2, radius * 2)),
                                            0, 90);
            path[sizeinfo.size() - 1].lineTo(path[sizeinfo.size() - 2].currentPosition() + QPoint((sizeinfo[sizeinfo.size() - 2] / m_totalsize)*paintRect.width(), 0));
            path[sizeinfo.size() - 1].lineTo(path[sizeinfo.size() - 2].currentPosition() + QPoint((sizeinfo[sizeinfo.size() - 2] / m_totalsize)*paintRect.width(), paintRect.height() - 1));
            path[sizeinfo.size() - 1].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
            path[sizeinfo.size() - 1].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                                  QSize(radius * 2, radius * 2)),
                                            270, 90);
            painter.setBrush(QBrush(colorinfo[sizeinfo.size() - 1]));
            painter.setPen(QPen(QColor(colorinfo[sizeinfo.size() - 1]), 3));
            painter.fillPath(path[sizeinfo.size() - 1], colorinfo[sizeinfo.size() - 1]);
        }
    }

//    paintpath0.moveTo(paintRect.topLeft() + QPoint(radius, 0));
//    paintpath0.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
//    paintpath0.lineTo(paintRect.bottomLeft() - QPoint(0, radius));
//    paintpath0.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
//                           QSize(radius * 2, radius * 2)),
//                     180, 90);
//    paintpath0.lineTo(paintRect.bottomLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius, 0));
//    paintpath0.lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius, 0));
//    paintpath0.lineTo(paintRect.topLeft() + QPoint(radius, 0));
//    QColor fillColor = colorinfo[0];
//    painter.setBrush(QBrush(fillColor));
//    painter.setPen(QPen(QColor(fillColor), 3));
//    painter.fillPath(paintpath0, fillColor);

//    QPainterPath    paintpath1;
//    paintpath1.moveTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius, 0));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius + (sizeinfo[1] / m_totalsize)*paintRect.width(), 0));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius + (sizeinfo[1] / m_totalsize)*paintRect.width(), paintRect.height()));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius, paintRect.height()));
////    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width(), paintRect.height()));
////    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + (sizeinfo[1] / m_totalsize)*paintRect.width(), paintRect.height()));
////    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + (sizeinfo[1] / m_totalsize)*paintRect.width(), 0));
//    QColor fillColor1 = colorinfo[1];
//    painter.setBrush(QBrush(fillColor1));
//    painter.setPen(QPen(QColor(fillColor1), 3));
//    painter.fillPath(paintpath1, fillColor1);
//    QPainterPath    paintpath2;
//    paintpath2.moveTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius + (sizeinfo[1] / m_totalsize)*paintRect.width(), 0));
//    paintpath2.lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius + (sizeinfo[1] / m_totalsize)*paintRect.width()
//                                                   + (sizeinfo[2] / m_totalsize)*paintRect.width(), 0));
//    paintpath2.lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + radius + (sizeinfo[1] / m_totalsize)*paintRect.width()
//                                                   + (sizeinfo[2] / m_totalsize)*paintRect.width(), paintRect.height()));
//    QColor fillColor2 = colorinfo[2];
//    painter.setBrush(QBrush(fillColor2));
//    painter.setPen(QPen(QColor(fillColor2), 3));
//    painter.fillPath(paintpath2, fillColor2);
//    QPainterPath    paintpath2;
//    paintpath1.moveTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width(), 0));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width(), paintRect.height()));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + (sizeinfo[1] / m_totalsize)*paintRect.width(), paintRect.height()));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((sizeinfo[0] / m_totalsize)*paintRect.width() + (sizeinfo[1] / m_totalsize)*paintRect.width(), 0));
//    QColor fillColor1 = colorinfo[1];
//    painter.setBrush(QBrush(fillColor1));
//    painter.setPen(QPen(QColor(fillColor1), 3));
//    painter.fillPath(paintpath1, fillColor);

//    paintpath.moveTo(paintRect.bottomRight() - QPoint(0, radius));
//    paintpath.lineTo(paintRect.topRight() + QPoint(0, radius));
//    paintpath.arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
//                          QSize(radius * 2, radius * 2)),
//                    0, 90);
//    paintpath.lineTo(paintRect.topRight() - QPoint(radius, 0) - QPoint(paintRect.topRight().x() - (m_lastsize / m_totalsize)*paintRect.width(), 0));
//    paintpath.lineTo(paintRect.topRight() - QPoint(radius, 0) - QPoint(paintRect.topRight().x() - (m_lastsize / m_totalsize)*paintRect.width(), (-paintRect.height())));
//    paintpath.lineTo(paintRect.topRight() - QPoint(radius, 0) - QPoint(0, (-paintRect.height())));
//    paintpath.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
//                          QSize(radius * 2, radius * 2)),
//                    270, 90);
//    QColor fillColor = m_lastcolor;
//    painter.setBrush(QBrush(fillColor));
//    painter.setPen(QPen(QColor(fillColor), 3));
//    painter.fillPath(paintpath, fillColor);
//    QPainterPath paintpath1;
//    paintpath1.moveTo(paintRect.topLeft() + QPoint(radius, 0));
//    paintpath1.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
//    paintpath1.lineTo(paintRect.bottomLeft() - QPoint(0, radius));
//    paintpath1.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
//                           QSize(radius * 2, radius * 2)),
//                     180, 90);
//    paintpath1.lineTo(paintRect.bottomLeft() + QPoint((m_previousize / m_totalsize)*paintRect.width() + radius, 0));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint((m_previousize / m_totalsize)*paintRect.width() + radius, 0));
//    paintpath1.lineTo(paintRect.topLeft() + QPoint(radius, 0));
//    paintpath.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);

//    paintpath.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((m_previousize / m_totalsize)*paintRect.width(), 0));
//    paintpath.lineTo(paintRect.topLeft() + QPoint(radius, 0) + QPoint((m_previousize / m_totalsize)*paintRect.width(), paintRect.height()));
//    paintpath.lineTo(paintRect.bottomLeft() + QPoint(radius, 0));
//    paintpath.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
//                          QSize(radius * 2, radius * 2)),
//                    180, 90);
//    QColor fillColor1 = m_previouscolor;
//    painter.setBrush(QBrush(fillColor1));
//    painter.setPen(QPen(QColor(fillColor1), 3));
//    painter.fillPath(paintpath1, fillColor1);
    //    paintpath.arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
//    paintpath.lineTo(paintRect.bottomLeft() - QPoint(0, radius));
//    paintpath.arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
//                          QSize(radius * 2, radius * 2)),
//                    180, 90);
//    paintpath.lineTo(paintRect.bottomRight() - QPoint(radius, 0));
//    paintpath.arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
//                          QSize(radius * 2, radius * 2)),
//                    270, 90);

//    QColor fillColor = m_basecolor;
//    painter.setBrush(QBrush(fillColor));
//    painter.fillPath(paintpath, fillColor);
//    painter.setPen(QPen(QColor(fillColor), 3));

//    painter.drawLine(paintRect.topLeft().x() + radius +  paintRect.width() / (total / m_used), paintRect.topLeft().y() + radius, paintRect.topLeft().x() + radius + paintRect.width() / (total / m_used), paintRect.bottomLeft().y() - radius);
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
    //绘制首页下方标注
    if (m_flag) {
        QRect recticon = QRect(paintRect.bottomLeft().x(), paintRect.bottomLeft().y() + 20, 15, 15);
        QImage image(":/icons/deepin/builtin/light/icons/dr_exception-logo_36px.svg");
        painter.drawImage(recticon, image);
        recticon.moveTo(paintRect.width() / 2 - 60, paintRect.bottomLeft().y() + 20);
        painter.drawImage(recticon, image);
//    recticon.moveTo(paintRect.bottomRight().x() - 200, paintRect.bottomLeft().y() + 20);
//    painter.drawImage(recticon, image);
        QRect recttext = QRect(paintRect.bottomLeft().x() + 20, paintRect.bottomLeft().y() + 17, 140, 20);
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        painter.setFont(font);

        painter.setPen(Qt::black);
        painter.drawText(recttext, QString(tr("The total amount:")));
        recttext.moveTo(paintRect.width() / 2 - 40, paintRect.bottomLeft().y() + 17);
        painter.drawText(recttext, QString(tr("Has been used:")));
//    recttext.moveTo(paintRect.bottomRight().x() - 180, paintRect.bottomLeft().y() + 17);
//    painter.drawText(recttext, QString(tr("The actual available:")));
        QString totalsize = QString::number(total) + "GB";
        QString usedsize = QString::number(m_used) + "GB";
//    QString trueusedsize = QString::number(m_tureused) + "GB";
        QRect rectsizenum = QRect(paintRect.bottomLeft().x() + 140, paintRect.bottomLeft().y() + 20, 100, 20);
        font.setBold(false);
        font.setPointSize(8);
        painter.setFont(font);
        painter.drawText(rectsizenum, totalsize);
        rectsizenum.moveTo(paintRect.width() / 2 + 60, paintRect.bottomLeft().y() + 20);
        painter.drawText(rectsizenum, usedsize);
    }
//    rectsizenum.moveTo(paintRect.bottomRight().x() - 45, paintRect.bottomLeft().y() + 20);
//    painter.drawText(rectsizenum, trueusedsize);
    painter.restore();

}
