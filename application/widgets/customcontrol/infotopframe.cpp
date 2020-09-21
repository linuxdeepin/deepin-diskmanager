/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file infotopframe.cpp
 *
 * @brief 主界面右侧信息展示上部分实现类
 *
 * @date 2020-09-17 14:05
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
#include "infotopframe.h"
#include "partedproxy/dmdbushandler.h"
#include "common.h"

#include <DPalette>
#include <DApplicationHelper>
#include <DFontSizeManager>

InfoTopFrame::InfoTopFrame(DWidget *parent)
    : DFrame(parent)
{
    setFixedHeight(110);
    setBackgroundRole(DPalette::ItemBackground);

    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(30, 0, 20, 0);
    setLayout(m_mainLayout);

    m_pictureLabel = new DLabel(this);
    m_pictureLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
    m_pictureLabel->setMinimumSize(85, 85);
    m_mainLayout->addWidget(m_pictureLabel);

    initLeftInfo();

    m_mainLayout->addStretch();

    initRightInfo();
}

void InfoTopFrame::initRightInfo()
{
    m_allNameLabel = new DLabel(tr("Capacity"), this);
    m_allNameLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(m_allNameLabel, DFontSizeManager::T6);

    m_allMemoryLabel = new DLabel(this);
    m_allMemoryLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(m_allMemoryLabel, DFontSizeManager::T1);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget(m_allNameLabel);
    layout->addWidget(m_allMemoryLabel);
    layout->addStretch();

    m_mainLayout->addLayout(layout);
}

void InfoTopFrame::initLeftInfo()
{
    m_nameLabel = new DLabel(this);
    QFont nameFont = DFontSizeManager::instance()->get(DFontSizeManager::T5);
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);

    m_typeLabel = new DLabel(this);
    DFontSizeManager::instance()->bind(m_typeLabel, DFontSizeManager::T8);

    DPalette palette = DApplicationHelper::instance()->palette(m_typeLabel);
    palette.setBrush(DPalette::Text, palette.textTips());
    DApplicationHelper::instance()->setPalette(m_typeLabel, palette);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_typeLabel);
    layout->addStretch();

    m_mainLayout->addLayout(layout);
}

void InfoTopFrame::setShowDiskInfo()
{
    auto info = DMDbusHandler::instance()->getCurPartititonInfo();

    m_nameLabel->setText(info.path);
    if ("unallocated" == info.path) {
        m_nameLabel->setText("ocated");
    }

    QString diskSize = Utils::formatSize(info.sector_end - info.sector_start,
                                            info.sector_size);
    m_allMemoryLabel->setText(diskSize);

    QString diskType = Utils::fileSystemTypeToString(static_cast<FSType>(info.fstype));
    m_typeLabel->setText(tr("File system") + ": " + diskType);
}
