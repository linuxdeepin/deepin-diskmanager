/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     wangzhixuan <wangzhixuan@uniontech.com>
* Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
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
#include "infotopframe.h"

#include <DPalette>
#include <DApplicationHelper>
#include <DFontSizeManager>

#include "partedproxy/dmdbushandler.h"
#include "widgets/widgetdeclare.h"

InfoTopFrame::InfoTopFrame(DWidget *parent)
    : DFrame(parent)
{
    setFixedHeight(110);

    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(30, 0, 20, 0);
    this->setLayout(mainLayout);

    picLabel = new DLabel(this);
    picLabel->setPixmap(getIcon("labeldisk").pixmap(85, 85));
    picLabel->setMinimumSize(85, 85);
    mainLayout->addWidget(picLabel);

    InitLeftInfo();

    mainLayout->addStretch();

    InitRightInfo();
}

void InfoTopFrame::InitRightInfo()
{
    QVBoxLayout *tLayout = new QVBoxLayout();

    allnameLabel = new DLabel(tr("Total capacity"), this);
    allnameLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(allnameLabel, DFontSizeManager::T6);

    allmemoryLabel = new DLabel(this);
    allmemoryLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(allmemoryLabel, DFontSizeManager::T1);

    tLayout->addWidget(allnameLabel);
    tLayout->addWidget(allmemoryLabel);

    mainLayout->addLayout(tLayout);
}

void InfoTopFrame::InitLeftInfo()
{
    QVBoxLayout *tLayout = new QVBoxLayout();

    nameLabel = new DLabel(this);
    QFont nameFont = DFontSizeManager::instance()->get(DFontSizeManager::T5);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    tLayout->addWidget(nameLabel);

    typeLabel = new DLabel(this);
    DFontSizeManager::instance()->bind(typeLabel, DFontSizeManager::T8);
    tLayout->addWidget(typeLabel);

    DPalette pa = DApplicationHelper::instance()->palette(typeLabel);
    pa.setBrush(DPalette::Text, pa.textTips());
    DApplicationHelper::instance()->setPalette(typeLabel, pa);

    mainLayout->addLayout(tLayout);
}

void InfoTopFrame::setShowDiskInfo()
{
    auto info = DMDbusHandler::instance()->getCurPartititonInfo();
    nameLabel->setText(info.path);

    QString s_disksize = QString::number(Utils::sector_to_unit(info.sector_end - info.sector_start,
                                                               info.sector_size, SIZE_UNIT::UNIT_GIB)) + "GB";
    allmemoryLabel->setText(s_disksize);

    QString s_diskType = Utils::FSTypeToString(static_cast<FSType>(info.fstype));
    typeLabel->setText(tr("format") + ": " + s_diskType);
}
