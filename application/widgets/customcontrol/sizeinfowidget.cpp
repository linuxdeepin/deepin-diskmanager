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
#include <DFontSizeManager>

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
    const double total = m_used + m_noused;
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
    //绘制首页下方标注
    if (m_flag) {
        QRect recticon = QRect(paintRect.bottomLeft().x(), paintRect.bottomLeft().y() + 20, 15, 15);
        QImage image(":/icons/deepin/builtin/light/icons/dr_exception-logo_36px.svg");
        painter.drawImage(recticon, image);
        recticon.moveTo(paintRect.width() / 2 - 60, paintRect.bottomLeft().y() + 20);
        painter.drawImage(recticon, image);
        QRect recttext = QRect(paintRect.bottomLeft().x() + 20, paintRect.bottomLeft().y() + 17, 140, 20);
        QFont font;
        font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
        QColor textcolor = this->palette().color(DPalette::Normal, DPalette::Text);
        painter.setFont(font);
        painter.setPen(textcolor);
        painter.drawText(recttext, QString(tr("The total amount:")));
        recttext.moveTo(paintRect.width() / 2 - 40, paintRect.bottomLeft().y() + 17);
        painter.drawText(recttext, QString(tr("Has been used:")));
        QString totalsize = QString::number(total, 'f', 2) + "GB";
        QString usedsize = QString::number(m_used, 'f', 2) + "GB";
        QRect rectsizenum = QRect(paintRect.bottomLeft().x() + 140, paintRect.bottomLeft().y() + 20, 100, 20);
        font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
        painter.setFont(font);
        painter.drawText(rectsizenum, totalsize);
        rectsizenum.moveTo(paintRect.width() / 2 + 60, paintRect.bottomLeft().y() + 20);
        painter.drawText(rectsizenum, usedsize);
    }
    painter.restore();

}
