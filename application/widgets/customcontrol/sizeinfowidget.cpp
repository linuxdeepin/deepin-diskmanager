/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file sizeinfowidget.cpp
 *
 * @brief 主界面右侧信息展示中间部分实现类
 *
 * @date 2020-09-17 14:35
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "sizeinfowidget.h"

#include <DFontSizeManager>

#include <QDebug>

SizeInfoWidget::SizeInfoWidget(QWidget *parent)
    : QWidget(parent)
{
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::onHandleChangeTheme);
}
SizeInfoWidget::SizeInfoWidget(double used, double unused, bool flag, QWidget *parent)
    : QWidget(parent)
    , m_used(used)
    , m_noused(unused)
    , m_flag(flag)
{
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::onHandleChangeTheme);
}

void SizeInfoWidget::setData(PartitionInfo info, QVector<QColor> color, QVector<double> size, bool flag)
{
    m_sizeInfo = size;
    m_colorInfo = color;
    m_flag = flag;

    m_noused = Utils::sectorToUnit(info.m_sectorsUnused, info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sectorToUnit(info.m_sectorsUsed, info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_totalSpaceSize = Utils::formatSize(info.m_sectorEnd - info.m_sectorStart, info.m_sectorSize);
    m_usedSize = Utils::formatSize(info.m_sectorsUsed, info.m_sectorSize);
    m_totalSize = m_noused + m_used;
    m_partitionPath = info.m_path.remove(0, 5);
    if (size.at(0) < 0.00 || size.at(1) < 0.00) {
        m_sizeInfo = QVector<double> {0.00, 0.00};
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
    rect.setX(x());
    rect.setY(y());
    rect.setWidth(width());
    rect.setHeight(height());
    QRect paintRect = QRect(0, 60, rect.width() + 1, rect.height() - 170);
    QPainterPath paintpath0;
    int radius = 8;
    //根据color和size数据遍历绘制矩形
    for (int i = 0; i < m_sizeInfo.size(); i++) {
        if (i == 0) {
            path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
            path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
            path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
            path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                QSize(radius * 2, radius * 2)),
                          180, 90);
            path[0].lineTo(paintRect.bottomLeft() + QPoint(static_cast<int>((m_sizeInfo[0] / m_totalSize) * paintRect.width() + radius), 0));
            path[0].lineTo(paintRect.topLeft() + QPoint(static_cast<int>((m_sizeInfo[0] / m_totalSize) * paintRect.width() + radius), 0));
            path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
            if (m_sizeInfo.at(i) == 0.00) {
                painter.setBrush(QBrush(m_colorInfo[1]));
                painter.setPen(QPen(QColor(m_colorInfo[1]), 3));
                painter.fillPath(path[0], m_colorInfo[1]);
            } else {
                painter.setBrush(QBrush(m_colorInfo[0]));
                painter.setPen(QPen(QColor(m_colorInfo[0]), 3));
                painter.fillPath(path[0], m_colorInfo[0]);
            }
        } else if (i > 0 && i < m_sizeInfo.size() - 1) {
            path[i].moveTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width() + (m_sizeInfo[i] / m_totalSize) * paintRect.width()), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width() + (m_sizeInfo[i] / m_totalSize) * paintRect.width()), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), 0));
            painter.setBrush(QBrush(m_colorInfo[i]));
            painter.setPen(QPen(QColor(m_colorInfo[i]), 3));
            painter.fillPath(path[i], m_colorInfo[i]);
        } else if (i == m_sizeInfo.size() - 1) {
            path[m_sizeInfo.size() - 1].moveTo(paintRect.bottomRight() - QPoint(0, radius));
            path[m_sizeInfo.size() - 1].lineTo(paintRect.topRight() + QPoint(0, radius));
            path[m_sizeInfo.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                  QSize(radius * 2, radius * 2)),
                                            0, 90);
            path[m_sizeInfo.size() - 1].lineTo(path[m_sizeInfo.size() - 2].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[m_sizeInfo.size() - 2] / m_totalSize) * paintRect.width()), 0));
            path[m_sizeInfo.size() - 1].lineTo(path[m_sizeInfo.size() - 2].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[m_sizeInfo.size() - 2] / m_totalSize) * paintRect.width()), paintRect.height() - 1));
            path[m_sizeInfo.size() - 1].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
            path[m_sizeInfo.size() - 1].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                                  QSize(radius * 2, radius * 2)),
                                            270, 90);
            painter.setBrush(QBrush(m_colorInfo[m_sizeInfo.size() - 1]));
            painter.setPen(QPen(QColor(m_colorInfo[m_sizeInfo.size() - 1]), 3));
            painter.fillPath(path[m_sizeInfo.size() - 1], m_colorInfo[m_sizeInfo.size() - 1]);
        }
    }
    //绘制首页下方标注
    if (m_flag) {
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType) {
            QRect roundRect = QRect(rect.bottomLeft().x() + 2, rect.bottomLeft().y() - 90, 15, 15);
            //            rect.setWidth(rect.width() - 1);
            //            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(roundRect, 3, 3);
            //            QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
            m_parentPb = DApplicationHelper::instance()->palette(this);
            QBrush brush1 = DApplicationHelper::instance()->palette(this).dark();
            QColor iconColor = m_parentPb.color(DPalette::Normal, DPalette::Dark);
            painter.setBrush(brush1);
            painter.setPen(iconColor);
            painter.fillRect(roundRect, brush1);
            //            QRect Round1rect1(rect.bottomLeft().x() + paintRect.width() / 2, paintRect.bottomLeft().y() + 20, 15, 15);
            roundRect.moveTo(paintRect.width() / 2, paintRect.bottomLeft().y() + 20);
            QPainterPath painterPath;
            painterPath.addRoundedRect(roundRect, 3, 3);
            QBrush brush2 = DApplicationHelper::instance()->palette(this).lightLively();
            QColor icon2Color = m_parentPb.color(DPalette::Normal, DPalette::LightLively);
            painter.setBrush(brush2);
            painter.setPen(icon2Color);
            painter.drawPath(painterPath);
            painter.fillPath(painterPath, brush2);
            QRect rectText = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, 300, 70);
            QFont font;
            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textColor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter.setFont(font);
            painter.setPen(textColor);
            QTextOption option;
            option.setAlignment(Qt::AlignTop);
            painter.drawText(rectText, QString(m_partitionPath + tr(" Capacity:") + " "), option);
            // 获取总容量字符串显示的宽度
            QFontMetrics fmCapacity = painter.fontMetrics();
            int capacityWidth = fmCapacity.width(QString(m_partitionPath + tr(" Capacity:") + " "));
            int capacityNum = rectText.x() + capacityWidth;

            rectText.moveTo(paintRect.width() / 2 + 25, paintRect.bottomLeft().y() + 17);
            painter.drawText(rectText, QString(tr("Used:") + " "), option);
            // 获取已用空间字符串显示的宽度
            QFontMetrics fmUsed = painter.fontMetrics();
            int usedWidth = fmCapacity.width(QString(tr("Used:") + " "));
            int usedNum = rectText.x() + usedWidth;

//            int num = 0;
//            if (m_partitionPath == "ocated") {
//                num = 135;
//            } else {
//                num = 120;
//            }
            QRect rectSizeNum = QRect(paintRect.bottomLeft().x() + capacityNum, paintRect.bottomLeft().y() + 20, 100, 30);
            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QColor text1Color = m_parentPb.color(DPalette::Normal, DPalette::TextTitle);
            painter.setFont(font);
            painter.setPen(text1Color);
            QTextOption option1;
            option.setAlignment(Qt::AlignLeft);
            painter.drawText(rectSizeNum, m_totalSpaceSize, option1);
            rectSizeNum.moveTo(paintRect.bottomLeft().x() + usedNum, paintRect.bottomLeft().y() + 20);
            if (m_usedSize.contains("-")) {
                m_usedSize = "-";
            }
            painter.drawText(rectSizeNum, m_usedSize, option1);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            QRect roundRect = QRect(rect.bottomLeft().x() + 2, rect.bottomLeft().y() - 90, 15, 15);
            //            rect.setWidth(rect.width() - 1);
            //            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(roundRect, 3, 3);
            //            QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
            m_parentPb = DApplicationHelper::instance()->palette(this);
            QBrush brush1 = DApplicationHelper::instance()->palette(this).dark();
            QColor iconColor = m_parentPb.color(DPalette::Normal, DPalette::Dark);
            painter.setBrush(brush1);
            painter.setPen(iconColor);
            painter.fillRect(roundRect, brush1);
            //            QRect Round1rect1(rect.bottomLeft().x() + paintRect.width() / 2, paintRect.bottomLeft().y() + 20, 15, 15);
            roundRect.moveTo(paintRect.width() / 2, paintRect.bottomLeft().y() + 20);
            QPainterPath painterPath;
            painterPath.addRoundedRect(roundRect, 3, 3);
            QBrush brush2 = DApplicationHelper::instance()->palette(this).lightLively();
            QColor icon2Color = m_parentPb.color(DPalette::Normal, DPalette::LightLively);
            painter.setBrush(brush2);
            painter.setPen(icon2Color);
            painter.drawPath(painterPath);
            painter.fillPath(painterPath, brush2);
            QRect rectText = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, 300, 70);
            QFont font;
            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textColor = m_parentPb.color(DPalette::Normal, DPalette::ToolTipText);
            painter.setFont(font);
            painter.setPen(textColor);
            QTextOption option;
            option.setAlignment(Qt::AlignTop);
            painter.drawText(rectText, QString(m_partitionPath + tr(" Capacity:") + " "), option);
            // 获取总容量字符串显示的宽度
            QFontMetrics fmCapacity = painter.fontMetrics();
            int capacityWidth = fmCapacity.width(QString(m_partitionPath + tr(" Capacity:") + " "));
            int capacityNum = rectText.x() + capacityWidth;

            rectText.moveTo(paintRect.width() / 2 + 25, paintRect.bottomLeft().y() + 17);
            painter.drawText(rectText, QString(tr("Used:") + " "), option);
            // 获取已用空间字符串显示的宽度
            QFontMetrics fmUsed = painter.fontMetrics();
            int usedWidth = fmCapacity.width(QString(tr("Used:") + " "));
            int usedNum = rectText.x() + usedWidth;

//            int num = 0;
//            if (m_partitionPath == "ocated") {
//                num = 135;
//            } else {
//                num = 120;
//            }
            QRect rectSizeNum = QRect(paintRect.bottomLeft().x() + capacityNum, paintRect.bottomLeft().y() + 20, 100, 30);
            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QColor text1Color = m_parentPb.color(DPalette::Normal, DPalette::ToolTipText);
            painter.setFont(font);
            painter.setPen(text1Color);
            QTextOption option1;
            option.setAlignment(Qt::AlignLeft);
            painter.drawText(rectSizeNum, m_totalSpaceSize, option1);
            rectSizeNum.moveTo(paintRect.bottomLeft().x() + usedNum, paintRect.bottomLeft().y() + 20);
            if (m_usedSize.contains("-")) {
                m_usedSize = "-";
            }
            painter.drawText(rectSizeNum, m_usedSize, option1);
        }
    }
    painter.restore();
}
void SizeInfoWidget::onHandleChangeTheme()
{
    m_parentPb = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
}
