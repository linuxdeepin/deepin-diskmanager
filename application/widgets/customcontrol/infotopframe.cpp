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
    m_mainLayout->setContentsMargins(20, 0, 20, 0);
    setLayout(m_mainLayout);

    m_pictureLabel = new DLabel(this);
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        m_pictureLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
    } else if (DMDbusHandler::DISK == DMDbusHandler::instance()->getCurLevel()) {
        m_pictureLabel->setPixmap(Common::getIcon("disk").pixmap(85, 85));
    } else if (DMDbusHandler::VOLUMEGROUP == DMDbusHandler::instance()->getCurLevel()) {
        m_pictureLabel->setPixmap(Common::getIcon("disk").pixmap(85, 85));
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        m_pictureLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
    }

    m_pictureLabel->setMinimumSize(85, 85);
    m_mainLayout->addWidget(m_pictureLabel);

    initLeftInfo();

    m_mainLayout->addStretch();

    initRightInfo();
}

void InfoTopFrame::initRightInfo()
{
    m_allNameLabel = new DLabel(tr("Capacity"));
    m_allNameLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(m_allNameLabel, DFontSizeManager::T6);

    m_allMemoryLabel = new DLabel;
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
    m_nameLabel = new DLabel;
    QFont nameFont = DFontSizeManager::instance()->get(DFontSizeManager::T5);
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);

    m_typeLabel = new DLabel;
    DFontSizeManager::instance()->bind(m_typeLabel, DFontSizeManager::T8);

    DPalette palette = DApplicationHelper::instance()->palette(m_typeLabel);
    palette.setBrush(DPalette::Text, palette.textTips());
    DApplicationHelper::instance()->setPalette(m_typeLabel, palette);

    QVBoxLayout *layout = new QVBoxLayout();
//    layout->addStretch();
    layout->addSpacing(20);
    layout->addWidget(m_nameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_typeLabel);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);

    m_mainLayout->addLayout(layout);
}

void InfoTopFrame::setShowDiskInfo()
{
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        auto info = DMDbusHandler::instance()->getCurPartititonInfo();

        if (1 == info.m_vgFlag) {
            m_pictureLabel->setPixmap(Common::getIcon("lv").pixmap(85, 85));
        } else {
            m_pictureLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
        }

        QFontMetrics fmDevpath = m_nameLabel->fontMetrics();
        QString textDevpath = info.m_path;
        int devpathWidth = fmDevpath.width(textDevpath);
        if (devpathWidth > width() / 2) {
            devpathWidth = width() / 2;
            textDevpath = m_nameLabel->fontMetrics().elidedText(textDevpath, Qt::ElideMiddle, devpathWidth);
        }
        m_nameLabel->setText(textDevpath);
    //    if ("unallocated" == info.m_path) {
    //        m_nameLabel->setText("ocated");
    //    }

        QString diskSize = Utils::formatSize(info.m_sectorEnd - info.m_sectorStart,
                                                info.m_sectorSize);
        m_allMemoryLabel->setText(diskSize);

        QString diskType = Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType));
        m_typeLabel->setText(tr("File system") + ": " + diskType);
    } else if (DMDbusHandler::DISK == DMDbusHandler::instance()->getCurLevel()) {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        QMap<QString, QString> isJoinAllVG = DMDbusHandler::instance()->getIsJoinAllVG();
        if (isJoinAllVG.value(info.m_path) == "true") {
            m_pictureLabel->setPixmap(Common::getIcon("vg").pixmap(85, 85));
        } else {
            m_pictureLabel->setPixmap(Common::getIcon("disk").pixmap(85, 85));
        }

        m_nameLabel->setText(info.m_model);
        QString diskSize = Utils::formatSize(info.m_length, info.m_sectorSize);
        m_allMemoryLabel->setText(diskSize);

        QString partitionTable;
        if (info.m_disktype == "gpt") {
            partitionTable = "GPT";
        } else if (info.m_disktype == "msdos") {
            partitionTable = "MBR";
        } else {
            partitionTable = info.m_disktype;
        }
        m_typeLabel->setText(tr("%1 partition table").arg(partitionTable));
    } else if (DMDbusHandler::VOLUMEGROUP == DMDbusHandler::instance()->getCurLevel()) {
        m_pictureLabel->setPixmap(Common::getIcon("vg").pixmap(85, 85));

        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        m_nameLabel->setText(vgInfo.m_vgName);
        m_allMemoryLabel->setText(vgInfo.m_vgSize);

        m_typeLabel->setText(tr("Volume group"));
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        m_pictureLabel->setPixmap(Common::getIcon("lv").pixmap(85, 85));

        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        m_nameLabel->setText(lvInfo.m_lvPath);
        m_allMemoryLabel->setText(lvInfo.m_lvSize);

        FSType fstype = static_cast<FSType>(lvInfo.m_lvFsType);
        QString fstypeName = Utils::fileSystemTypeToString(fstype);
        m_typeLabel->setText(tr("File system") + ": " + fstypeName);
    }
}

void InfoTopFrame::resizeEvent(QResizeEvent *event)
{
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        auto info = DMDbusHandler::instance()->getCurPartititonInfo();

        QFontMetrics fmDevpath = m_nameLabel->fontMetrics();
        QString textDevpath = info.m_path;
        int devpathWidth = fmDevpath.width(textDevpath);
        if (devpathWidth > width() / 2) {
            devpathWidth = width() / 2;
            textDevpath = m_nameLabel->fontMetrics().elidedText(textDevpath, Qt::ElideMiddle, devpathWidth);
        }
        m_nameLabel->setText(textDevpath);
    }

    DFrame::resizeEvent(event);
}
