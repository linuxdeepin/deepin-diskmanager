// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "infotopframe.h"
#include "partedproxy/dmdbushandler.h"
#include "common.h"

#include <DPalette>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#endif
#include <DFontSizeManager>

InfoTopFrame::InfoTopFrame(DWidget *parent)
    : DFrame(parent)
{
    qDebug()  << "InfoTopFrame constructor";
    setFixedHeight(110);
    setBackgroundRole(DPalette::ItemBackground);

    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(20, 0, 20, 0);
    setLayout(m_mainLayout);

    m_pictureLabel = new DLabel(this);
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is PARTITION, setting pixmap to labeldisk";
        m_pictureLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
    } else if (DMDbusHandler::DISK == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is DISK, setting pixmap to disk";
        m_pictureLabel->setPixmap(Common::getIcon("disk").pixmap(85, 85));
    } else if (DMDbusHandler::VOLUMEGROUP == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is VOLUMEGROUP, setting pixmap to vg";
        m_pictureLabel->setPixmap(Common::getIcon("vg").pixmap(85, 85));
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is LOGICALVOLUME, setting pixmap to lv";
        m_pictureLabel->setPixmap(Common::getIcon("lv").pixmap(85, 85));
    }

    m_pictureLabel->setMinimumSize(85, 85);
    m_mainLayout->addWidget(m_pictureLabel);

    initLeftInfo();

    m_mainLayout->addStretch();

    initRightInfo();
    qDebug()  << "InfoTopFrame initialized";
}

void InfoTopFrame::initRightInfo()
{
    qDebug() << "InfoTopFrame::initRightInfo called";
    m_allNameLabel = new DLabel(tr("Capacity"));
    m_allNameLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(m_allNameLabel, DFontSizeManager::T6);

    m_allMemoryLabel = new DLabel;
    m_allMemoryLabel->setAlignment(Qt::AlignRight);
    DFontSizeManager::instance()->bind(m_allMemoryLabel, DFontSizeManager::T2);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addStretch();
    layout->addWidget(m_allNameLabel);
    layout->addWidget(m_allMemoryLabel);
    layout->addStretch();

    m_mainLayout->addLayout(layout);
    qDebug() << "InfoTopFrame::initRightInfo finished";
}

void InfoTopFrame::initLeftInfo()
{
    qDebug() << "InfoTopFrame::initLeftInfo called";
    m_nameLabel = new DLabel;
    QFont nameFont = DFontSizeManager::instance()->get(DFontSizeManager::T5);
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);

    m_typeLabel = new DLabel;
    DFontSizeManager::instance()->bind(m_typeLabel, DFontSizeManager::T8);

    /*DPalette palette = DApplicationHelper::instance()->palette(m_typeLabel);
    palette.setBrush(DPalette::Text, palette.textTips());
    DApplicationHelper::instance()->setPalette(m_typeLabel, palette);*/

    QVBoxLayout *layout = new QVBoxLayout();
//    layout->addStretch();
    layout->addSpacing(20);
    layout->addWidget(m_nameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_typeLabel);
    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);

    m_mainLayout->addLayout(layout);
    qDebug() << "InfoTopFrame::initLeftInfo finished";
}

void InfoTopFrame::updateDiskInfo()
{
    qDebug()  << "Updating disk info for level:"
                         << DMDbusHandler::instance()->getCurLevel();
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is PARTITION";
        auto info = DMDbusHandler::instance()->getCurPartititonInfo();

        if (info.m_vgFlag != LVMFlag::LVM_FLAG_NOT_PV) {
            qDebug() << "Partition is a PV, setting pixmap to lv";
            m_pictureLabel->setPixmap(Common::getIcon("lv").pixmap(85, 85));
        } else if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug() << "Partition is LUKS encrypted, setting pixmap to partitionlock";
            m_pictureLabel->setPixmap(Common::getIcon("partitionlock").pixmap(85, 85));
        } else {
            qDebug() << "Partition is neither PV nor LUKS encrypted, setting pixmap to labeldisk";
            m_pictureLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
        }

        QFontMetrics fmDevpath = m_nameLabel->fontMetrics();
        QString textDevpath = info.m_path;
#if QT_VERSION_MAJOR > 5
        qDebug() << "QT_VERSION_MAJOR > 5, getting devpathWidth";
        int devpathWidth = fmDevpath.boundingRect(textDevpath).width();
#else
        qDebug() << "QT_VERSION_MAJOR <= 5, getting devpathWidth";
        int devpathWidth = fmDevpath.width(textDevpath);
#endif
        if (devpathWidth > width() / 2) {
            qDebug() << "Devpath width exceeds half of widget width, eliding text";
            devpathWidth = width() / 2;
            textDevpath = m_nameLabel->fontMetrics().elidedText(textDevpath, Qt::ElideMiddle, devpathWidth);
        }
        m_nameLabel->setText(textDevpath);
    //    if ("unallocated" == info.m_path) {
    //        m_nameLabel->setText("ocated");
    //    }

        QString diskSize = Utils::formatSize(info.m_sectorEnd - info.m_sectorStart + 1,
                                                info.m_sectorSize);
        m_allMemoryLabel->setText(diskSize);

        QString diskType = Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType));
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug() << "Partition is LUKS encrypted, checking decryption status";
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            if (luksInfo.isDecrypt) {
                qDebug() << "LUKS partition is decrypted, getting encryption file system type";
                diskType = DMDbusHandler::instance()->getEncryptionFsType(luksInfo);
            }
        }
        m_typeLabel->setText(tr("File system") + ": " + diskType);
    } else if (DMDbusHandler::DISK == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is DISK";
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

        if (DMDbusHandler::instance()->getIsJoinAllVG().value(info.m_path) == "true") {
            qDebug() << "Disk is joined to all VG, setting pixmap to vg";
            m_pictureLabel->setPixmap(Common::getIcon("vg").pixmap(85, 85));
        } else if (DMDbusHandler::instance()->getIsAllEncryption().value(info.m_path) == "true") {
            qDebug() << "Disk is all encrypted, setting pixmap to disklock";
            m_pictureLabel->setPixmap(Common::getIcon("disklock").pixmap(85, 85));
        } else {
            qDebug() << "Disk is neither joined to all VG nor all encrypted, setting pixmap to disk";
            m_pictureLabel->setPixmap(Common::getIcon("disk").pixmap(85, 85));
        }

        QString diskSize = Utils::formatSize(info.m_length, info.m_sectorSize);
        m_allMemoryLabel->setText(diskSize);

        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS && info.m_partition.size() == 0) {
            qDebug() << "Disk is LUKS encrypted and has no partitions, handling as full disk encryption";
            // 处理磁盘是整盘加密页面显示
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            m_nameLabel->setText(info.m_path);
            if (luksInfo.isDecrypt) {
                qDebug() << "LUKS disk is decrypted, setting type label to encryption file system type";
                m_typeLabel->setText(tr("File system") + ": " + DMDbusHandler::instance()->getEncryptionFsType(luksInfo));
            } else {
                qDebug() << "LUKS disk is not decrypted, setting type label to FS_LUKS";
                m_typeLabel->setText(tr("File system") + ": " + Utils::fileSystemTypeToString(FSType::FS_LUKS));
            }
        } else {
            qDebug() << "Disk is not fully LUKS encrypted or has partitions, setting name and partition table type";
            m_nameLabel->setText(info.m_model);
            QString partitionTable;
            if (info.m_disktype == "gpt") {
                qDebug() << "Disk type is gpt";
                partitionTable = "GPT";
            } else if (info.m_disktype == "msdos") {
                qDebug() << "Disk type is msdos";
                partitionTable = "MBR";
            } else {
                qDebug() << "Disk type is other:" << info.m_disktype;
                partitionTable = info.m_disktype;
            }
            m_typeLabel->setText(tr("%1 partition table").arg(partitionTable));
        }

    } else if (DMDbusHandler::VOLUMEGROUP == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is VOLUMEGROUP";
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();

        if (DMDbusHandler::instance()->getIsAllEncryption().value(vgInfo.m_vgName) == "true") {
            qDebug() << "Volume group is all encrypted, setting pixmap to vglock";
            m_pictureLabel->setPixmap(Common::getIcon("vglock").pixmap(85, 85));
        } else {
            qDebug() << "Volume group is not all encrypted, setting pixmap to vg";
            m_pictureLabel->setPixmap(Common::getIcon("vg").pixmap(85, 85));
        }

        m_nameLabel->setText(vgInfo.m_vgName);
        QString vgSize = vgInfo.m_vgSize;
        if (vgSize.contains("1024")) {
            qDebug() << "VG size contains 1024, reformatting size";
            vgSize = Utils::LVMFormatSize(vgInfo.m_peCount * vgInfo.m_PESize + vgInfo.m_PESize);
        }
        m_allMemoryLabel->setText(vgSize);

        m_typeLabel->setText(tr("Volume group"));
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is LOGICALVOLUME";
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();

        if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug() << "Logical volume is LUKS encrypted, setting pixmap to lvlock";
            m_pictureLabel->setPixmap(Common::getIcon("lvlock").pixmap(85, 85));
        } else {
            qDebug() << "Logical volume is not LUKS encrypted, setting pixmap to lv";
            m_pictureLabel->setPixmap(Common::getIcon("lv").pixmap(85, 85));
        }

        m_nameLabel->setText(lvInfo.m_lvPath);
        QString lvSize = lvInfo.m_lvSize;
        if (lvSize.contains("1024")) {
            qDebug() << "LV size contains 1024, reformatting size";
            lvSize = Utils::LVMFormatSize(lvInfo.m_lvLECount * lvInfo.m_LESize + lvInfo.m_LESize);
        }
        m_allMemoryLabel->setText(lvSize);

        QString fstypeName = Utils::fileSystemTypeToString(static_cast<FSType>(lvInfo.m_lvFsType));
        if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug() << "LV is LUKS encrypted, checking decryption status";
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath);
            if (luksInfo.isDecrypt) {
                qDebug() << "LUKS LV is decrypted, getting encryption file system type";
                fstypeName = DMDbusHandler::instance()->getEncryptionFsType(luksInfo);
            }
            qDebug()  << "Disk info updated successfully";
        }
        m_typeLabel->setText(tr("File system") + ": " + fstypeName);
    }
    qDebug() << "InfoTopFrame::updateDiskInfo finished";
}

void InfoTopFrame::resizeEvent(QResizeEvent *event)
{
    // qDebug() << "InfoTopFrame::resizeEvent called";
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is PARTITION, adjusting textDevpath";
        auto info = DMDbusHandler::instance()->getCurPartititonInfo();

        QFontMetrics fmDevpath = m_nameLabel->fontMetrics();
        QString textDevpath = info.m_path;
#if QT_VERSION_MAJOR > 5
        // qDebug() << "QT_VERSION_MAJOR > 5, getting devpathWidth for resizeEvent";
        int devpathWidth = fmDevpath.boundingRect(textDevpath).width();
#else
        // qDebug() << "QT_VERSION_MAJOR <= 5, getting devpathWidth for resizeEvent";
        int devpathWidth = fmDevpath.width(textDevpath);
#endif
        if (devpathWidth > width() / 2) {
            qDebug() << "Devpath width exceeds half of widget width in resizeEvent, eliding text";
            devpathWidth = width() / 2;
            textDevpath = m_nameLabel->fontMetrics().elidedText(textDevpath, Qt::ElideMiddle, devpathWidth);
        }
        m_nameLabel->setText(textDevpath);
    }

    DFrame::resizeEvent(event);
    // qDebug() << "InfoTopFrame::resizeEvent finished";
}
