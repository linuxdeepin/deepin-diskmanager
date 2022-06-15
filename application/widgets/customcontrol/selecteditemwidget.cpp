/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file selecteditemwidget.cpp
 *
 * @brief 已选分区信息或者磁盘信息展示窗口
 *
 * @date 2022-02-24 10:48
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
#include "selecteditemwidget.h"
#include "partedproxy/dmdbushandler.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QApplication>

SelectedItemWidget::SelectedItemWidget(PVInfoData pvInfoData, QWidget *parent)
    : RadiusWidget(parent)
    , m_pvInfoData(pvInfoData)
{
    initUi();
    initConnection();
}

void SelectedItemWidget::initUi()
{
    setFixedHeight(36);

    DPalette paletteName;
    paletteName.setColor(DPalette::TextTips, QColor("#2C4767"));

    DPalette paletteSize;
    paletteName.setColor(DPalette::TextTips, QColor("#526A7F"));

    QFont fontName;
    fontName.setWeight(QFont::Medium);
    fontName.setFamily("Source Han Sans");
    fontName.setPixelSize(14);

    QFont fontSize;
    fontSize.setWeight(QFont::Normal);
    fontSize.setFamily("Source Han Sans");
    fontSize.setPixelSize(12);

    m_nameInfo = new DLabel(this);
    m_nameInfo->setText("/dev/sda1");
    m_nameInfo->setFont(fontName);
    m_nameInfo->setPalette(paletteName);
    m_nameInfo->setAlignment(Qt::AlignVCenter);

    m_sizeInfo = new DLabel(this);
    m_sizeInfo->setText("1GB");
    m_sizeInfo->setFont(fontSize);
    m_sizeInfo->setPalette(paletteSize);
    m_sizeInfo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

//    m_buttonLabel = new ButtonLabel(this);
//    m_buttonLabel->setPixmap(DStyle::standardIcon(QApplication::style(), DStyle::SP_DeleteButton).pixmap(QSize(17, 17)));

    m_iconButton = new DIconButton(this);
    m_iconButton->setIcon(DStyle::standardIcon(QApplication::style(), DStyle::SP_DeleteButton));
    m_iconButton->setIconSize(QSize(17, 17));
    m_iconButton->setFixedSize(17, 17);

    if (m_pvInfoData.m_disktype == "unrecognized") {
        m_nameInfo->setText(m_pvInfoData.m_diskPath);
        m_sizeInfo->setText(m_pvInfoData.m_diskSize);
    } else {
        if (m_pvInfoData.m_partitionPath == "unallocated") {
            m_nameInfo->setText(m_pvInfoData.m_diskPath);
        } else {
            m_nameInfo->setText(m_pvInfoData.m_partitionPath);
        }

        m_sizeInfo->setText(m_pvInfoData.m_partitionSize);
    }

    if (m_pvInfoData.m_isReadOnly) {
        m_iconButton->setDisabled(true);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_nameInfo);
    mainLayout->addStretch();
    mainLayout->addWidget(m_sizeInfo);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(m_iconButton);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(10, 0, 10, 0);

    setLayout(mainLayout);
}

void SelectedItemWidget::initConnection()
{
    connect(m_iconButton, &DIconButton::clicked, this, &SelectedItemWidget::onDeleteClicked);
}

void SelectedItemWidget::onDeleteClicked()
{
    emit deleteItem(m_pvInfoData);
}


