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
#include "sizeinfowidget.h"
#include <DFontSizeManager>
#include <QDebug>

SizeInfoWidget::SizeInfoWidget(QWidget *parent)
    : QWidget(parent)
{
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::slothandleChangeTheme);
}
SizeInfoWidget::SizeInfoWidget(double used, double unused, bool flag, QWidget *parent)
    : QWidget(parent)
    , m_used(used)
    , m_noused(unused)
    , m_flag(flag)
{
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::slothandleChangeTheme);
}

void SizeInfoWidget::setdata(PartitionInfo info, QVector<QColor> color, QVector<double> size, bool flag)
{
    sizeinfo = size;
    colorinfo = color;
    m_flag = flag;

    m_noused = Utils::sector_to_unit(info.sectors_unused, info.sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sector_to_unit(info.sectors_used, info.sector_size, SIZE_UNIT::UNIT_GIB);
    totalsize = Utils::format_size(info.sector_end - info.sector_start, info.sector_size);
    usedsize = Utils::format_size(info.sectors_used, info.sector_size);
    m_totalsize = m_noused + m_used;
    m_partitionpath = info.path.remove(0, 5);
    if (size.at(0) < 0.00 || size.at(1) < 0.00) {
        sizeinfo = QVector<double> {0.00, 0.00};
    }
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
    QVector<QPainterPath> path {path0, path1, path2, path3, path4, path5, path6, path7, path8, path9};
    rect.setX(this->x());
    rect.setY(this->y());
    rect.setWidth(this->width());
    rect.setHeight(this->height());
    QRect paintRect = QRect(0, 60, rect.width(), rect.height() - 150);
    QPainterPath paintpath0;
    int radius = 8;
//    int sizeflag = 8;
//    const double total = m_used + m_noused;
    //根据color和size数据遍历绘制矩形
    for (int i = 0; i < sizeinfo.size(); i++) {
        if (i == 0) {
            path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
            path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
            path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
            path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                QSize(radius * 2, radius * 2)),
                          180, 90);
            path[0].lineTo(paintRect.bottomLeft() + QPoint((sizeinfo[0] / m_totalsize) * paintRect.width() + radius, 0));
            path[0].lineTo(paintRect.topLeft() + QPoint((sizeinfo[0] / m_totalsize) * paintRect.width() + radius, 0));
            path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
            if (sizeinfo.at(i) == 0.00) {
                painter.setBrush(QBrush(colorinfo[1]));
                painter.setPen(QPen(QColor(colorinfo[1]), 3));
                painter.fillPath(path[0], colorinfo[1]);
            } else {
                painter.setBrush(QBrush(colorinfo[0]));
                painter.setPen(QPen(QColor(colorinfo[0]), 3));
                painter.fillPath(path[0], colorinfo[0]);
            }
        } else if (i > 0 && i < sizeinfo.size() - 1) {
            path[i].moveTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize) * paintRect.width(), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize) * paintRect.width() + (sizeinfo[i] / m_totalsize) * paintRect.width(), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize) * paintRect.width() + (sizeinfo[i] / m_totalsize) * paintRect.width(), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize) * paintRect.width(), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint((sizeinfo[i - 1] / m_totalsize) * paintRect.width(), 0));
            painter.setBrush(QBrush(colorinfo[i]));
            painter.setPen(QPen(QColor(colorinfo[i]), 3));
            painter.fillPath(path[i], colorinfo[i]);
        } else if (i == sizeinfo.size() - 1) {
            path[sizeinfo.size() - 1].moveTo(paintRect.bottomRight() - QPoint(0, radius));
            path[sizeinfo.size() - 1].lineTo(paintRect.topRight() + QPoint(0, radius));
            path[sizeinfo.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                  QSize(radius * 2, radius * 2)),
                                            0, 90);
            path[sizeinfo.size() - 1].lineTo(path[sizeinfo.size() - 2].currentPosition() + QPoint((sizeinfo[sizeinfo.size() - 2] / m_totalsize) * paintRect.width(), 0));
            path[sizeinfo.size() - 1].lineTo(path[sizeinfo.size() - 2].currentPosition() + QPoint((sizeinfo[sizeinfo.size() - 2] / m_totalsize) * paintRect.width(), paintRect.height() - 1));
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
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType) {
            QRect Roundrect = QRect(rect.bottomLeft().x() - 5, rect.bottomLeft().y() - 77, 15, 15);
            //            rect.setWidth(rect.width() - 1);
            //            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(Roundrect, 3, 3);
            //            QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
            m_parentPb = DApplicationHelper::instance()->palette(this);
            QBrush brush1 = DApplicationHelper::instance()->palette(this).dark();
            QColor iconcolor = m_parentPb.color(DPalette::Normal, DPalette::Dark);
            painter.setBrush(brush1);
            painter.setPen(iconcolor);
            painter.fillRect(Roundrect, brush1);
            //            QRect Round1rect1(rect.bottomLeft().x() + paintRect.width() / 2, paintRect.bottomLeft().y() + 20, 15, 15);
            Roundrect.moveTo(paintRect.width() / 2, paintRect.bottomLeft().y() + 20);
            QPainterPath painterPath;
            painterPath.addRoundedRect(Roundrect, 3, 3);
            QBrush brush2 = DApplicationHelper::instance()->palette(this).lightLively();
            QColor icon2color = m_parentPb.color(DPalette::Normal, DPalette::LightLively);
            painter.setBrush(brush2);
            painter.setPen(icon2color);
            painter.drawPath(painterPath);
            painter.fillPath(painterPath, brush2);
            QRect recttext = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, 150, 70);
            QFont font;
            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textcolor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter.setFont(font);
            painter.setPen(textcolor);
            QTextOption option;
            option.setAlignment(Qt::AlignTop);
            painter.drawText(recttext, QString(m_partitionpath + tr(" Capacity:")), option);
            recttext.moveTo(paintRect.width() / 2 + 20, paintRect.bottomLeft().y() + 17);
            painter.drawText(recttext, QString(tr("Used:")), option);
            QRect rectsizenum = QRect(paintRect.bottomLeft().x() + 145, paintRect.bottomLeft().y() + 20, 100, 30);
            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QColor text1color = m_parentPb.color(DPalette::Normal, DPalette::TextTitle);
            painter.setFont(font);
            painter.setPen(text1color);
            QTextOption option1;
            option.setAlignment(Qt::AlignLeft);
            painter.drawText(rectsizenum, totalsize, option1);
            rectsizenum.moveTo(paintRect.width() / 2 + 115, paintRect.bottomLeft().y() + 20);
            if (usedsize.contains("-")) {
                usedsize = "-";
            }
            painter.drawText(rectsizenum, usedsize, option1);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            QRect Roundrect = QRect(rect.bottomLeft().x() - 5, rect.bottomLeft().y() - 77, 15, 15);
            //            rect.setWidth(rect.width() - 1);
            //            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(Roundrect, 3, 3);
            //            QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
            m_parentPb = DApplicationHelper::instance()->palette(this);
            QBrush brush1 = DApplicationHelper::instance()->palette(this).dark();
            QColor iconcolor = m_parentPb.color(DPalette::Normal, DPalette::Dark);
            painter.setBrush(brush1);
            painter.setPen(iconcolor);
            painter.fillRect(Roundrect, brush1);
            //            QRect Round1rect1(rect.bottomLeft().x() + paintRect.width() / 2, paintRect.bottomLeft().y() + 20, 15, 15);
            Roundrect.moveTo(paintRect.width() / 2, paintRect.bottomLeft().y() + 20);
            QPainterPath painterPath;
            painterPath.addRoundedRect(Roundrect, 3, 3);
            QBrush brush2 = DApplicationHelper::instance()->palette(this).lightLively();
            QColor icon2color = m_parentPb.color(DPalette::Normal, DPalette::LightLively);
            painter.setBrush(brush2);
            painter.setPen(icon2color);
            painter.drawPath(painterPath);
            painter.fillPath(painterPath, brush2);
            QRect recttext = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, 150, 70);
            QFont font;
            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textcolor = m_parentPb.color(DPalette::Normal, DPalette::ToolTipText);
            painter.setFont(font);
            painter.setPen(textcolor);
            QTextOption option;
            option.setAlignment(Qt::AlignTop);
            painter.drawText(recttext, QString(m_partitionpath + tr(" Capacity:")), option);
            recttext.moveTo(paintRect.width() / 2 + 20, paintRect.bottomLeft().y() + 17);
            painter.drawText(recttext, QString(tr("Used:")), option);
            QRect rectsizenum = QRect(paintRect.bottomLeft().x() + 145, paintRect.bottomLeft().y() + 20, 100, 30);
            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QColor text1color = m_parentPb.color(DPalette::Normal, DPalette::ToolTipText);
            painter.setFont(font);
            painter.setPen(text1color);
            QTextOption option1;
            option.setAlignment(Qt::AlignLeft);
            painter.drawText(rectsizenum, totalsize, option1);
            rectsizenum.moveTo(paintRect.width() / 2 + 115, paintRect.bottomLeft().y() + 20);
            if (usedsize.contains("-")) {
                usedsize = "-";
            }
            painter.drawText(rectsizenum, usedsize, option1);
        }
    }
    painter.restore();
}
void SizeInfoWidget::slothandleChangeTheme()
{
    m_parentPb = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
}
