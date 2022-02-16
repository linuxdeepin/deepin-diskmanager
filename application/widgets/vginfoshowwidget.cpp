/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file vginfoshowwidget.cpp
 *
 * @brief 逻辑卷组下的逻辑卷信息展示类
 *
 * @date 2022-01-24 13:33
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
#include "vginfoshowwidget.h"

#include <DFontSizeManager>
#include <DLabel>
#include <DGuiApplicationHelper>
#include <DScrollArea>
#include <DScrollBar>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QApplication>
#include <QScrollArea>

VGInfoShowWidget::VGInfoShowWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
    initConnection();
}

void VGInfoShowWidget::initUi()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
}

void VGInfoShowWidget::initConnection()
{

}

void VGInfoShowWidget::setData(const QList< QMap<QString, QVariant> > &lstInfo)
{
    QList<QObject *> lstCylinderWidget = children();
    for (int i = 0; i < lstCylinderWidget.count(); i ++) {
        QObject *obj = lstCylinderWidget.at(i);
        delete obj;
        obj = nullptr;
    }

    QFont fontName;
    fontName.setWeight(QFont::Medium);
    fontName.setFamily("Source Han Sans");
    fontName.setPixelSize(14);

    QFont fontSize;
    fontSize.setWeight(QFont::Normal);
    fontSize.setFamily("Source Han Sans");
    fontSize.setPixelSize(12);

    DPalette paletteText;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        QColor color("#F7F7F7");
        color.setAlphaF(0.6);
        DPalette paletteBack;
        paletteBack.setColor(DPalette::Background, color);
        setPalette(paletteBack);

        paletteText.setColor(DPalette::Text, QColor("#000000"));
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        QColor color("#000000");
        color.setAlphaF(0.6);
        DPalette paletteBack;
        paletteBack.setColor(DPalette::Background, color);
        setPalette(paletteBack);

        paletteText.setColor(DPalette::Text, QColor("#C0C6D4"));
    }

    QGridLayout *m_gridLayout = new QGridLayout;
    for (int i = 0; i< lstInfo.count(); i++) {
        QMap<QString, QVariant> mapInfo = lstInfo.at(i);
        QString path = mapInfo["name"].toString();
        QString size = mapInfo["size"].toString();
        QColor color = mapInfo["color"].value<QColor>();

        DLabel *m_label = new DLabel(this);
        m_label->setFixedSize(16, 16);
        m_label->setFrameShape(QFrame::Box);
        m_label->setObjectName(QString("%1").arg(i));
        QString strColor = QString("rgba(%1, %2, %3, %4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alphaF());
        m_label->setStyleSheet(QString("background:%1;border:0px;border-radius:4px;").arg(strColor));

        DLabel *m_nameLabel = new DLabel(path, this);
        m_nameLabel->setFont(fontName);
        m_nameLabel->setPalette(paletteText);
        DLabel *m_sizeLabel = new DLabel(size, this);
        m_sizeLabel->setFont(fontSize);
        m_sizeLabel->setPalette(paletteText);

        QVBoxLayout *layoutLeft = new QVBoxLayout;
        layoutLeft->addSpacing(2);
        layoutLeft->addWidget(m_label);
        layoutLeft->addStretch();
        layoutLeft->setSpacing(0);
        layoutLeft->setContentsMargins(0, 0, 0, 0);

        QVBoxLayout *layoutRight = new QVBoxLayout;
        layoutRight->addWidget(m_nameLabel);
        layoutRight->addWidget(m_sizeLabel);
        layoutRight->setSpacing(0);
        layoutRight->setContentsMargins(0, 0, 0, 0);

        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addLayout(layoutLeft);
        hLayout->addLayout(layoutRight);
        hLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget = new QWidget;
        widget->setLayout(hLayout);

        m_gridLayout->addWidget(widget, i / 2, i % 2);
    }

    int height = lstInfo.count() / 2;
    if (lstInfo.count() % 2 != 0) {
        height += 1;
    }

    DFrame *frame = new DFrame;
    if (height > 4) {
        setFixedSize(340, 192);

        m_gridLayout->setSpacing(8);
        m_gridLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget1 = new QWidget;
        widget1->setLayout(m_gridLayout);
        widget1->setFixedSize(318, height * 48 - 20);

        DScrollArea *scrollArea = new DScrollArea;
        scrollArea->setFrameShadow(QFrame::Plain);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
//        m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖向滚动条
        scrollArea->setWidget(widget1);

        QVBoxLayout *scrollAreaLayout = new QVBoxLayout;
        scrollAreaLayout->addWidget(scrollArea);
        scrollAreaLayout->setSpacing(0);
        scrollAreaLayout->setContentsMargins(10, 10, 10, 10);

        frame->setLayout(scrollAreaLayout);
    } else {
        int width = 340;
        if (lstInfo.count() == 1) {
            width = 170;
        }
        setFixedSize(width, height * 48);

        m_gridLayout->setSpacing(8);
        m_gridLayout->setContentsMargins(10, 10, 10, 10);

        frame->setLayout(m_gridLayout);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(frame);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}


