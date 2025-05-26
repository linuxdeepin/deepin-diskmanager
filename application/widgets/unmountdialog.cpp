// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "unmountdialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"
#include "unmountwarningdialog.h"
#include <QDebug>

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QVBoxLayout>

UnmountDialog::UnmountDialog(QWidget *parent)
    : DDBase(parent)
{
    qDebug() << "UnmountDialog initialized";
    initUi();
    initConnection();
}

void UnmountDialog::initUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    DLabel *tipLabel = new DLabel(tr("Make sure there are no programs running on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    mainLayout->addWidget(tipLabel);

    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        setTitle(tr("Unmount %1").arg(info.m_path));
        qDebug()  << "Preparing to unmount partition:" << info.m_path;
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        setTitle(tr("Unmount %1").arg(lvInfo.m_lvName));
        qDebug()  << "Preparing to unmount logical volume:" << lvInfo.m_lvName;

        tipLabel->setText(tr("Make sure there are no programs running on the logical volume"));
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        setTitle(tr("Unmount %1").arg(info.m_path));
        qDebug()  << "Preparing to unmount disk:" << info.m_path;
    }

    int index = addButton(tr("Cancel"), false, ButtonNormal);
    m_okCode = addButton(tr("Unmount"), false, ButtonWarning);

    getButton(index)->setAccessibleName("cancel");
    getButton(m_okCode)->setAccessibleName("unmountButton");
    setOnButtonClickedClose(false);
}

void UnmountDialog::initConnection()
{
    connect(this, &UnmountDialog::buttonClicked, this, &UnmountDialog::onButtonClicked);
}

void UnmountDialog::umountCurMountPoints()
{
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug()  << "Unmounting encrypted partition:" << info.m_path;
            DMDbusHandler::instance()->cryptUmount(DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path), info.m_path);
        } else {
            qDebug()  << "Unmounting partition:" << info.m_path;
            DMDbusHandler::instance()->unmount();
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug()  << "Unmounting encrypted logical volume:" << lvInfo.m_lvName;
            DMDbusHandler::instance()->cryptUmount(DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath), lvInfo.m_lvName);
        } else {
            qDebug()  << "Unmounting logical volume:" << lvInfo.m_lvName;
            LVAction lvAction;
            lvAction.m_vgName = lvInfo.m_vgName;
            lvAction.m_lvName = lvInfo.m_lvName;
            lvAction.m_lvFs = lvInfo.m_lvFsType;
            lvAction.m_lvSize = lvInfo.m_lvSize;
            lvAction.m_lvByteSize = lvInfo.m_lvLECount * lvInfo.m_LESize;
            lvAction.m_lvAct = LVMAction::LVM_ACT_LV_UMOUNT;

            DMDbusHandler::instance()->onUmountLV(lvAction);
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug()  << "Unmounting encrypted disk:" << info.m_path;
            DMDbusHandler::instance()->cryptUmount(DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path), info.m_path);
        }
    }
}

void UnmountDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (m_okCode == index) {
        int partitionFlag = 0;
        bool isSysPath = false;
        bool lvFlag = false;
        if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
            partitionFlag = info.m_flag;

            for (int i = 0; i < info.m_mountPoints.size(); i++) {
                if (info.m_mountPoints[i] == "/boot/efi" || info.m_mountPoints[i] == "/boot"
                        || info.m_mountPoints[i] == "/" || info.m_mountPoints[i] == "/recovery") {
                    isSysPath = true;
                    break;
                }
            }
        } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            lvFlag = lvInfo.m_dataFlag;
            for (int i = 0; i < lvInfo.m_mountPoints.size(); i++) {
                if (lvInfo.m_mountPoints[i] == "/boot/efi" || lvInfo.m_mountPoints[i] == "/boot"
                        || lvInfo.m_mountPoints[i] == "/" || lvInfo.m_mountPoints[i] == "/recovery") {
                    isSysPath = true;
                    break;
                }
            }
        }

        qDebug() << "Checking system path status";
        if (isSysPath || partitionFlag == 4 || lvFlag) {
            qWarning() << "Attempting to unmount system path - showing warning dialog";
            UnmountWarningDialog unmountWarningDialog;
            unmountWarningDialog.setObjectName("firstWarning");
            unmountWarningDialog.setAccessibleName("firstWarning");
            if (unmountWarningDialog.exec() == DDialog::Accepted) {
                umountCurMountPoints();
                qDebug() << "User canceled unmount operation";
                close();
            }
        } else {
            umountCurMountPoints();
            close();
        }
    } else {
        close();
    }
}
