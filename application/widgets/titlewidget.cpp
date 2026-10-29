// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "titlewidget.h"
#include "mountdialog.h"
#include "unmountdialog.h"
#include "formatedialog.h"
#include "resizedialog.h"
#include "createlvwidget.h"
#include "common.h"
#include "messagebox.h"
#include "createvgwidget.h"
#include "removepvwidget.h"
#include "decryptdialog.h"
#include <QDebug>

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QApplication>

TitleWidget::TitleWidget(DWidget *parent)
    : DWidget(parent)
{
    qCDebug(appLog) << "TitleWidget initialized";
    initUi();
    initConnection();
}

void TitleWidget::initUi()
{
    qCDebug(appLog) << "Initializing TitleWidget UI.";
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_btnParted = createBtn(tr("Partition"), "partition");
    m_btnFormat = createBtn(tr("Wipe"), "erase");
    m_btnMount = createBtn(tr("Mount"), "mount");
    m_btnUnmount = createBtn(tr("Unmount"), "unmount");
    m_btnResize = createBtn(tr("Resize"), "resize");
    m_btnCreateVG = createBtn(tr("Create volume group"), "add");
    m_btnCreateVG->setObjectName("createVG");
    m_btnCreateVG->setAccessibleName("createVG");

    m_btnDeleteVG = createBtn(tr("Delete volume group"), "reduce");
    m_btnDeleteVG->setObjectName("deleteVG");
    m_btnDeleteVG->setAccessibleName("deleteVG");

    m_btnDeleteLV = createBtn(tr("Delete logical volume"), "reduce");
    m_btnDeleteLV->setObjectName("deleteLV");
    m_btnDeleteLV->setAccessibleName("deleteLV");

    m_btnDeletePV = createBtn(tr("Delete physical volume"), "reduce");
    m_btnDeletePV->setObjectName("deletePV");
    m_btnDeletePV->setAccessibleName("deletePV");

    m_btnCreateLV = createBtn(tr("Create logical volume"), "partition");
    m_btnCreateLV->setObjectName("createLV");
    m_btnCreateLV->setAccessibleName("createLV");

    m_btnResizeVG = createBtn(tr("Resize"), "resize");
    m_btnResizeVG->setObjectName("vgResize");
    m_btnResizeVG->setAccessibleName("vgResize");

    m_btnResizeLV = createBtn(tr("Resize"), "resize");
    m_btnResizeLV->setObjectName("lvResize");
    m_btnResizeLV->setAccessibleName("lvResize");


    m_btnCreateVG->setDisabled(false);
    m_btnParted->setDisabled(true);
    m_btnFormat->setDisabled(true);
    m_btnMount->setDisabled(true);
    m_btnUnmount->setDisabled(true);
    m_btnResize->setDisabled(true);
    m_btnCreateLV->setDisabled(true);
    m_btnResizeVG->setDisabled(true);
    m_btnResizeLV->setDisabled(true);
    m_btnDeletePV->setDisabled(true);
    m_btnDeleteVG->setDisabled(true);
    m_btnDeleteLV->setDisabled(true);

    layout->addWidget(m_btnCreateVG);
    layout->addWidget(m_btnDeleteVG);
    layout->addWidget(m_btnDeleteLV);
    layout->addWidget(m_btnDeletePV);
    layout->addWidget(m_btnCreateLV);
    layout->addWidget(m_btnParted);
    layout->addWidget(m_btnFormat);
    layout->addWidget(m_btnMount);
    layout->addWidget(m_btnUnmount);
    layout->addWidget(m_btnResize);
    layout->addWidget(m_btnResizeVG);
    layout->addWidget(m_btnResizeLV);

    m_btnParted->show();
    m_btnDeletePV->show();
    m_btnResize->show();
    m_btnCreateLV->hide();
    m_btnDeleteVG->hide();
    m_btnDeleteLV->hide();
    m_btnResizeLV->hide();
    m_btnResizeVG->hide();

    resetButtonsSize();
    qCDebug(appLog) << "TitleWidget UI initialization completed.";
}

void TitleWidget::setSizebyMode(DPushButton *button)
{
    qCDebug(appLog) << "setSizebyMode called";
#ifdef DTKWIDGET_CLASS_DSizeMode
    qCDebug(appLog) << "DTKWIDGET_CLASS_DSizeMode is defined, setting size by DSizeModeHelper.";
    button->setFixedSize(QSize(DSizeModeHelper::element(24, 36), DSizeModeHelper::element(24, 36)));
    button->setIconSize(QSize(DSizeModeHelper::element(12, 18), DSizeModeHelper::element(12, 18)));
#else
    qCDebug(appLog) << "DTKWIDGET_CLASS_DSizeMode is not defined, setting fixed size (36x36) and icon size (18x18).";
    button->setFixedSize(QSize(36, 36));
    button->setIconSize(QSize(18, 18));
#endif
    qCDebug(appLog) << "setSizebyMode completed.";
}

void TitleWidget::resetButtonsSize()
{
    qCDebug(appLog) << "resetButtonsSize called.";
    setSizebyMode(m_btnCreateVG);
    setSizebyMode(m_btnParted);
    setSizebyMode(m_btnFormat);
    setSizebyMode(m_btnMount);
    setSizebyMode(m_btnUnmount);
    setSizebyMode(m_btnResize);
    setSizebyMode(m_btnCreateLV);
    setSizebyMode(m_btnResizeVG);
    setSizebyMode(m_btnResizeLV);
    setSizebyMode(m_btnDeletePV);
    setSizebyMode(m_btnDeleteVG);
    setSizebyMode(m_btnDeleteLV);
    qCDebug(appLog) << "resetButtonsSize completed.";
}

void TitleWidget::initConnection()
{
    qCDebug(appLog) << "Initializing TitleWidget connections.";
    connect(DMDbusHandler::instance(), &DMDbusHandler::curSelectChanged, this, &TitleWidget::onCurSelectChanged);
    connect(m_btnParted, &DPushButton::clicked, this, &TitleWidget::showPartInfoWidget);
    connect(m_btnFormat, &DPushButton::clicked, this, &TitleWidget::showFormateInfoWidget);
    connect(m_btnMount, &DPushButton::clicked, this, &TitleWidget::showMountInfoWidget);
    connect(m_btnUnmount, &DPushButton::clicked, this, &TitleWidget::showUnmountInfoWidget);
    connect(m_btnResize, &DPushButton::clicked, this, &TitleWidget::showResizeInfoWidget);
    connect(m_btnCreateLV, &DPushButton::clicked, this, &TitleWidget::onCreateLVClicked);
    connect(m_btnDeleteLV, &DPushButton::clicked, this, &TitleWidget::onDeleteLVClicked);
    connect(m_btnResizeLV, &DPushButton::clicked, this, &TitleWidget::onResizeLVClicked);
    connect(m_btnCreateVG, &DPushButton::clicked, this, &TitleWidget::onCreateVGClicked);
    connect(m_btnDeleteVG, &DPushButton::clicked, this, &TitleWidget::onDeleteVGClicked);
    connect(m_btnResizeVG, &DPushButton::clicked, this, &TitleWidget::onResizeVGClicked);
    connect(m_btnDeletePV, &DPushButton::clicked, this, &TitleWidget::onDeletePVClicked);
    connect(DMDbusHandler::instance(), &DMDbusHandler::updateUsb, this, &TitleWidget::onUpdateUsb);

#ifdef DTKWIDGET_CLASS_DSizeMode
    qCDebug(appLog) << "Connecting sizeModeChanged signal for DTKWIDGET_CLASS_DSizeMode.";
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [this]() {
        resetButtonsSize();
        qCDebug(appLog) << "Size mode changed, resetButtonsSize called.";
    });
#endif
    qCDebug(appLog) << "TitleWidget connections initialized.";
}

DPushButton *TitleWidget::createBtn(const QString &btnName, const QString &objName, bool bCheckable)
{
    qCDebug(appLog) << "createBtn called for btnName:" << btnName << ", objName:" << objName << ", bCheckable:" << bCheckable;
    auto btn = new DPushButton(this);
    //  wzx 设置图标icon    2020-05-19
    QIcon icon = Common::getIcon(objName);

    btn->setIcon(icon);
#ifdef DTKWIDGET_CLASS_DSizeMode
    qCDebug(appLog) << "DTKWIDGET_CLASS_DSizeMode is defined, setting size by DSizeModeHelper.";
    btn->setFixedSize(QSize(DSizeModeHelper::element(24, 36), DSizeModeHelper::element(24, 36)));
    btn->setIconSize(QSize(DSizeModeHelper::element(12, 18), DSizeModeHelper::element(12, 18)));
#else
    qCDebug(appLog) << "DTKWIDGET_CLASS_DSizeMode is not defined, setting fixed size (36x36) and icon size (18x18).";
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(18, 18));
#endif
    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    btn->setObjectName(objName);
    btn->setAccessibleName(objName);

    if (bCheckable) {
        qCDebug(appLog) << "Button is checkable, setting checked to false.";
        btn->setChecked(false);
    }

    qCDebug(appLog) << "createBtn completed, returning button:" << btn->objectName();
    return btn;
}

void TitleWidget::showPartInfoWidget()
{
    qCDebug(appLog) << "Show partition info widget";
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    PartitionDialog dlg(this);
    dlg.setObjectName("partitionDialog");
    dlg.setAccessibleName("partitionDialog");

    setCurDevicePath(info.m_devicePath);

    if (dlg.exec() == 1) {
        qCDebug(appLog) << "PartitionDialog accepted.";
        if (TYPE_UNPARTITIONED == info.m_type && FS_UNALLOCATED == info.m_fileSystemType) {
            qCDebug(appLog) << "Device is unpartitioned and unallocated, showing warning.";
            setCurDevicePath("");
            qCDebug(appLog) << QString("No partition table found on device %1").arg(info.m_devicePath);
            qCDebug(appLog) << "A partition table is required before partitions can be added";
            //ToDo:empty device create partition table
            // 无法识别当前设备分区表
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget()->parentWidget(),
                                                     QIcon::fromTheme("://icons/deepin/builtin/warning.svg"),
                                                     tr("Cannot recognize its partition table"));
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            return;
        }

        PartitionWidget partitionWidget(this);
        partitionWidget.setObjectName("partitionWidget");
        partitionWidget.setAccessibleName("partitionWidget");
        partitionWidget.exec();

        setCurDevicePath("");
    } else {
        qCDebug(appLog) << "PartitionDialog rejected.";
    }
    qCDebug(appLog) << "showPartInfoWidget completed.";
}

void TitleWidget::showFormateInfoWidget()
{
    qCDebug(appLog) << "Show format dialog";
    setCurDevicePath(DMDbusHandler::instance()->getCurDevicePath());

    FormateDialog dlg(this);
    dlg.setObjectName("wipeDialog");
    dlg.setAccessibleName("wipeDialog");
    dlg.exec();

    setCurDevicePath("");
}

bool TitleWidget::showDecryptDialog()
{
    qCDebug(appLog) << "Show decrypt dialog";
    DecryptDialog decryptDialog(this);
    decryptDialog.setObjectName("decryptDialog");
    decryptDialog.setAccessibleName("decryptDialog");
    if (decryptDialog.exec() != DDialog::Accepted) {
        qCDebug(appLog) << "Decrypt dialog not accepted.";
        return false;
    }

    qCDebug(appLog) << "Decrypt dialog accepted.";
    return true;
}

bool TitleWidget::showNoFileSystemWarningDialog(const LUKS_INFO &luksInfo)
{
    qCDebug(appLog) << "Show no file system warning dialog";
    if (luksInfo.m_mapper.m_luksFs == FSType::FS_UNKNOWN || luksInfo.m_mapper.m_luksFs == FSType::FS_UNALLOCATED
            || luksInfo.m_mapper.m_luksFs == FSType::FS_UNSUPPORTED || luksInfo.m_mapper.m_luksFs == FSType::FS_UNFORMATTED) {
        qCDebug(appLog) << "No file system found, showing warning.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("partNoSupportFSWidget");
        // 该设备没有文件系统，或文件系统不被支持，无法挂载  确定
        warningBox.setWarings(tr("Unable to mount the device: no file system is found, "
                                 "or the file system is not supported"), "", tr("OK"), "ok");
        warningBox.exec();
        return true;
    }

    qCDebug(appLog) << "No file system warning dialog not shown.";
    return false;
}

void TitleWidget::showMountInfoWidget()
{
    qCDebug(appLog) << "Show mount dialog";
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qCDebug(appLog) << "Current level is partition.";
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        setCurDevicePath(info.m_devicePath);
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            if (!showDecryptDialog()) {
                setCurDevicePath("");
                qCDebug(appLog) << "Decrypt dialog not accepted, returning.";
                return;
            }

            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            if (showNoFileSystemWarningDialog(luksInfo)) {
                setCurDevicePath("");
                qCDebug(appLog) << "No file system warning dialog shown, returning.";
                return;
            }
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        qCDebug(appLog) << "Current level is logical volume.";
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        setCurVGName(lvInfo.m_vgName);
        if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            if (!showDecryptDialog()) {
                setCurVGName("");
                qCDebug(appLog) << "Decrypt dialog not accepted, returning.";
                return;
            }

            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath);
            if (showNoFileSystemWarningDialog(luksInfo)) {
                setCurVGName("");
                qCDebug(appLog) << "No file system warning dialog shown, returning.";
                return;
            }
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        qCDebug(appLog) << "Current level is disk.";
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        setCurDevicePath(info.m_path);
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            if (!showDecryptDialog()) {
                setCurDevicePath("");
                qCDebug(appLog) << "Decrypt dialog not accepted, returning.";
                return;
            }

            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            if (showNoFileSystemWarningDialog(luksInfo)) {
                setCurDevicePath("");
                qCDebug(appLog) << "No file system warning dialog shown, returning.";
                return;
            }
        }
    }

    MountDialog dlg(this);
    dlg.setObjectName("mountDialog");
    dlg.setAccessibleName("mountDialog");
    dlg.exec();

    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qCDebug(appLog) << "Current level is partition.";
        setCurDevicePath("");
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        qCDebug(appLog) << "Current level is logical volume.";
        setCurVGName("");
    }
}

void TitleWidget::showUnmountInfoWidget()
{
    qCDebug(appLog) << "Show unmount dialog";
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qCDebug(appLog) << "Current level is partition.";
        setCurDevicePath(DMDbusHandler::instance()->getCurPartititonInfo().m_devicePath);
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        qCDebug(appLog) << "Current level is logical volume.";
        setCurVGName(DMDbusHandler::instance()->getCurLVInfo().m_vgName);
    }

    UnmountDialog dlg(this);
    dlg.setObjectName("unmountDialog");
    dlg.setAccessibleName("unmountDialog");
    dlg.exec();

    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qCDebug(appLog) << "Current level is partition.";
        setCurDevicePath("");
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        qCDebug(appLog) << "Current level is logical volume.";
        setCurVGName("");
    }
}

void TitleWidget::showResizeInfoWidget()
{
    qCDebug(appLog) << "Show resize dialog";
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setCurDevicePath(info.m_devicePath);
    FS_Limits limits = info.m_fsLimits;
    if (limits.min_size == -1 && limits.max_size == -1) {
        qCDebug(appLog) << "Partition does not support resizing.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("partNoSupportFSWidget");
        // 该文件系统不支持调整空间  确定
        warningBox.setWarings(tr("The file system does not support space adjustment"), "", tr("OK"), "ok");
        warningBox.exec();

        setCurDevicePath("");

        return;
    }

    ResizeDialog dlg(this);
    dlg.setObjectName("resizeDialog");
    dlg.setAccessibleName("resizeDialog");
    dlg.exec();

    setCurDevicePath("");
}

void TitleWidget::onCreateLVClicked()
{
    qCDebug(appLog) << "Create LV button clicked";
    setCurVGName(DMDbusHandler::instance()->getCurLVInfo().m_vgName);
    PartitionDialog dlg(this);
    dlg.setTitleText(tr("Create logical volume"), tr("The disks will be formatted if you create a logical volume"));
    dlg.setObjectName("createLVDialog");
    dlg.setAccessibleName("createLVDialog");

    if (dlg.exec() == 1) {
        qCDebug(appLog) << "Create LV dialog accepted.";
        CreateLVWidget createLVWidget(this);
        createLVWidget.setObjectName("createLVWidget");
        createLVWidget.setAccessibleName("createLVWidget");
        createLVWidget.exec();
    }

    setCurVGName("");
}

void TitleWidget::onDeleteLVClicked()
{
    qCDebug(appLog) << "Delete LV button clicked";
    LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
    setCurVGName(lvInfo.m_vgName);

    if (DMDbusHandler::instance()->lvIsMount(lvInfo)) {
        qCDebug(appLog) << "LV is mounted.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("messageBox");
        // 请先手动卸载XXX（逻辑卷名称）  确定
        warningBox.setWarings(tr("Unmount %1 first").arg(lvInfo.m_lvName), "", tr("OK"), "ok");
        warningBox.exec();

        setCurVGName("");

        return;
    }

    MessageBox messageBox(this);
    messageBox.setObjectName("messageBox");
    messageBox.setAccessibleName("deleteLVMessageBox");
    // 删除后数据将无法恢复，请确认  删除  取消
    messageBox.setWarings(tr("Data cannot be recovered if deleted, please confirm before proceeding"), "",
                          tr("Delete"), DDialog::ButtonWarning, "delete", tr("Cancel"), "cancel");
    if (messageBox.exec() == DDialog::Accepted) {
        qCDebug(appLog) << "Delete LV dialog accepted.";
        QStringList lvNameList;
        lvNameList << lvInfo.m_lvPath;

        DMDbusHandler::instance()->deleteLV(lvNameList);
    }

    setCurVGName("");
    qCDebug(appLog) << "Delete LV dialog closed.";
}

void TitleWidget::onResizeLVClicked()
{
    qCDebug(appLog) << "Resize LV button clicked";
    LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
    setCurVGName(lvInfo.m_vgName);
    if(lvInfo.m_lvFsType == FS_NTFS){
        qCDebug(appLog) << "LV is NTFS.";
        if (DMDbusHandler::instance()->lvIsMount(lvInfo)){
            qCDebug(appLog) << "LV is mounted.";
            MessageBox warningBox(this);
            warningBox.setObjectName("messageBox");
            warningBox.setAccessibleName("messageBox");
            // 请先手动卸载XXX（逻辑卷名称）  确定
            warningBox.setWarings(tr("Unmount %1 first").arg(lvInfo.m_lvName), "", tr("OK"), "ok");
            warningBox.exec();

            setCurVGName("");

            return;
        }
    }

    FS_Limits limits = lvInfo.m_fsLimits;
    if ((limits.min_size == -1 && limits.max_size == -1)
            || lvInfo.m_lvFsType == FS_FAT16 || lvInfo.m_lvFsType == FS_FAT32) { //fat32 or fat16 lvm情况下不支持调整
        qCDebug(appLog) << "LV does not support resizing.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("LVNoSupportFSWidget");
        // 该文件系统不支持调整空间  确定
        warningBox.setWarings(tr("The file system does not support space adjustment"), "", tr("OK"), "ok");
        warningBox.exec();

        setCurVGName("");

        return;
    }

    if (lvInfo.m_fileSystemLabel == "Roota" || lvInfo.m_fileSystemLabel == "Rootb") {
        qCDebug(appLog) << "LV is Roota or Rootb.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("messageBox");
        // 为保证正常使用备份还原功能，rootA跟rootB需同步扩容，确保空间一样大  确定  取消
        warningBox.setWarings(tr("To ensure the normal use of system backup and restore, \n"
                                 "rootA and rootB should be resized to the same value"), "", tr("OK"), "ok");
        warningBox.exec();
    }

    ResizeDialog dlg(this);
    dlg.setObjectName("resizeLVDialog");
    dlg.setAccessibleName("resizeLVDialog");
    dlg.exec();

    setCurVGName("");
    qCDebug(appLog) << "Resize LV dialog closed.";
}

void TitleWidget::onCreateVGClicked()
{
    qCDebug(appLog) << "Create VG button clicked";
    PartitionDialog dlg(this);
    dlg.setTitleText(tr("Create volume group"), tr("The disks will be formatted if you create a volume group"));
    dlg.setObjectName("createVGDialog");
    dlg.setAccessibleName("createVGDialog");

    if (dlg.exec() == 1) {
        qCDebug(appLog) << "Create VG dialog accepted.";
        CreateVGWidget createVGWidget(CreateVGWidget::CREATE, this);
        createVGWidget.setObjectName("createVGWidget");
        createVGWidget.setAccessibleName("createVGWidget");
        createVGWidget.exec();
    }
}

void TitleWidget::onDeleteVGClicked()
{
    qCDebug(appLog) << "Delete VG button clicked";
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    setCurVGName(vgInfo.m_vgName);
    if (DMDbusHandler::instance()->isExistMountLV(vgInfo)) {
        qCDebug(appLog) << "VG has mounted LV.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("messageBox");
        // 请先手动卸载XXX（逻辑卷组的名称）下所有的逻辑卷  确定
        warningBox.setWarings(tr("Unmount all logical volumes in %1 first").arg(vgInfo.m_vgName), "", tr("OK"), "ok");
        warningBox.exec();

        setCurVGName("");

        return;
    }

    MessageBox messageBox(this);
    messageBox.setObjectName("messageBox");
    messageBox.setAccessibleName("deleteVGMessageBox");
    // 删除后数据将无法恢复，请确认  删除  取消
    messageBox.setWarings(tr("Data cannot be recovered if deleted, please confirm before proceeding"), "",
                          tr("Delete"), DDialog::ButtonWarning, "delete", tr("Cancel"), "cancel");
    if (messageBox.exec() == DDialog::Accepted) {
        qCDebug(appLog) << "Delete VG dialog accepted.";
        QStringList vgNameList;
        vgNameList << vgInfo.m_vgName;

        DMDbusHandler::instance()->deleteVG(vgNameList);
    }

    setCurVGName("");
    qCDebug(appLog) << "Delete VG dialog closed.";
}

void TitleWidget::onResizeVGClicked()
{
    qCDebug(appLog) << "Resize VG button clicked";
    setCurVGName(DMDbusHandler::instance()->getCurVGInfo().m_vgName);

    CreateVGWidget createVGWidget(CreateVGWidget::RESIZE, this);
    createVGWidget.setObjectName("ResizeVGWidget");
    createVGWidget.setAccessibleName("ResizeVGWidget");
    createVGWidget.exec();

    setCurVGName("");
    qCDebug(appLog) << "Resize VG dialog closed.";
}

void TitleWidget::onDeletePVClicked()
{
    qCDebug(appLog) << "Delete PV button clicked";
    QMap<QString, VGInfo> mapVGInfo = DMDbusHandler::instance()->probLVMInfo().m_vgInfo;
    bool vgIsMount = false;
    QStringList lstVGName;
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qCDebug(appLog) << "Current level is partition.";
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        setCurDevicePath(info.m_devicePath);
        VGData vgData = info.m_vgData;
        lstVGName.clear();
        if (!vgData.m_vgName.isEmpty()) {
            VGInfo vgInfo = mapVGInfo.value(vgData.m_vgName);
            QMap<QString, PVInfo> mapPvInfo = vgInfo.m_pvInfo;
            QStringList lstKeys = mapPvInfo.keys();
            if (lstKeys.size() == 1                 // 判断VG是否由一个PV组成
                    && lstKeys.at(0) == info.m_path // 判断VG下的PV是否为当前分区
                    && vgInfo.m_lvlist.size() > 0   // 判断VG下是否有LV
                    && DMDbusHandler::instance()->isExistMountLV(vgInfo)) {  // 判断是否有LV被挂载
                lstVGName.append(vgInfo.m_vgName);
                vgIsMount = true;
            }
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        qCDebug(appLog) << "Current level is disk.";
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        setCurDevicePath(info.m_path);
        QStringList pvStrList;
        // 获取将要被删除的PV
        if (info.m_disktype == "none") {
            pvStrList.append(info.m_path);
        } else {
            for (int i = 0; i < info.m_partition.size(); i++) {
                PartitionInfo partInfo = info.m_partition.at(i);
                if (partInfo.m_vgFlag != LVMFlag::LVM_FLAG_NOT_PV) {
                    pvStrList.append(partInfo.m_path);
                }
            }
        }

        QVector<VGData> vglist = info.m_vglist;
        lstVGName.clear();
        for (int i = 0; i < vglist.count(); i++) {
            VGData vgData = vglist.at(i);
            if (!vgData.m_vgName.isEmpty()) {
                VGInfo vgInfo = mapVGInfo.value(vgData.m_vgName);
                QMap<QString, PVInfo> mapPvInfo = vgInfo.m_pvInfo;
                QStringList lstKeys = mapPvInfo.keys();
                bool isDelete = true;
                // 判断VG下的PV是否全部将被删除
                for (int j = 0; j < lstKeys.count(); j++) {
                    if (pvStrList.indexOf(lstKeys.at(j)) == -1) {
                        isDelete = false;
                        break;
                    }
                }

                if (isDelete) {
                    // 判断VG下是否有LV
                    if (vgInfo.m_lvlist.size() > 0) {
                        // 判断是否有LV被挂载
                        if (DMDbusHandler::instance()->isExistMountLV(vgInfo) && lstVGName.indexOf(vgData.m_vgName) == -1) {
                            lstVGName.append(vgData.m_vgName);
                        }
                    }
                }
            }
        }

        if (!lstVGName.isEmpty()) {
            qCDebug(appLog) << "VG has mounted LV.";
            vgIsMount = true;
        }
    }

    if (vgIsMount) {
        qCDebug(appLog) << "PV has mounted LV.";
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("deletePVMessageBox");
        // 请先手动卸载XXX（逻辑卷组名称）  确定
        warningBox.setWarings(tr("Unmount %1 first").arg(lstVGName.join(',')), "", tr("OK"), "ok");
        warningBox.exec();

        setCurDevicePath("");

        return;
    }

    RemovePVWidget removePVWidget(this);
    removePVWidget.setObjectName("RemovePVWidget");
    removePVWidget.setAccessibleName("RemovePVWidget");
    removePVWidget.exec();

    setCurDevicePath("");
    qCDebug(appLog) << "Delete PV dialog closed.";
}

void TitleWidget::updateBtnStatus()
{
    qCDebug(appLog) << "Update button status.";
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qCDebug(appLog) << "Current level is partition.";
        updateBtnShowStatus(true, true, false, false, true, false, true, true);
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        if (info.m_vgFlag != LVMFlag::LVM_FLAG_NOT_PV) {
            qCDebug(appLog) << "Partition is in PV.";
            m_btnParted->hide();
            m_btnCreateLV->show();

            QMap<QString, VGInfo> mapVGInfo = DMDbusHandler::instance()->probLVMInfo().m_vgInfo;
            VGInfo vgInfo = mapVGInfo.value(info.m_vgData.m_vgName);
            m_btnDeletePV->setDisabled(vgInfo.isPartial());
            m_btnCreateLV->setDisabled(true);
            updatePartitionBtnStatus(true, true, true, true, true);
        } else if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qCDebug(appLog) << "Partition is in LUKS.";
            m_btnResize->setDisabled(true);
            m_btnDeletePV->setDisabled(true);
            m_btnParted->setDisabled(true);

            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            updateEncryptDeviceBtnStatus(luksInfo);
        } else {
            qCDebug(appLog) << "Partition is not in PV or LUKS.";
            m_btnDeletePV->setDisabled(true);
            m_btnParted->show();
            m_btnCreateLV->hide();

            //已挂载
            if (info.m_mountPoints.size() > 0 && info.m_busy) {
                qCDebug(appLog) << "Partition is mounted.";
                updatePartitionBtnStatus(true, true, true, false, true);

                if (1 == info.m_flag) {
                    m_btnUnmount->setDisabled(true);
                }
            } else {
                qCDebug(appLog) << "Partition is not mounted.";
                if (1 == info.m_flag) {
                    qCDebug(appLog) << "Partition is not mounted.";
                    updatePartitionBtnStatus(true, true, true, true, true);
                } else {
                    //需判断扩展分区上是否无分区，否则认为不可操作，此处省略操作
                    if (FS_EXTENDED == info.m_fileSystemType) {
                        updatePartitionBtnStatus(true, true, true, true, true);
                        return ;
                    } else {
                        if (info.m_fileSystemType == FS_UNALLOCATED) {
                            updatePartitionBtnStatus(false, false, true, true, true);
                        } else if (info.m_fileSystemType == FS_UNKNOWN) {
                            updatePartitionBtnStatus(true, false, true, true, true);
                        } else {
                            updatePartitionBtnStatus(true, false, false, true, false);
                        }
                    }
                }
            }

            qCDebug(appLog) << info.m_type << info.m_fileSystemType;
            if (info.m_fileSystemType == FSType::FS_LINUX_SWAP) {
                qCDebug(appLog) << "Partition is swap.";
                updatePartitionBtnStatus(true, true, true, true, true);
            }
            //CD,DVD disable format
            if (info.m_devicePath.startsWith("/dev/sr") || info.m_devicePath.startsWith("/dev/cdrom")) {
                qCDebug(appLog) << "Partition is CD or DVD.";
                updatePartitionBtnStatus(true, true, true, true, true);
            }
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        qCDebug(appLog) << "Current level is disk.";
        updateBtnShowStatus(true, true, false, false, true, false, true, true);
        updatePartitionBtnStatus(true, true, true, true, true);

        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        if (info.m_vgFlag != LVMFlag::LVM_FLAG_NOT_PV) { //有分区创建为pv
            qCDebug(appLog) << "Disk is in PV.";
            m_btnParted->hide();
            m_btnCreateLV->show();

            bool isVGNormal = true;
            QMap<QString, VGInfo> mapVGInfo = DMDbusHandler::instance()->probLVMInfo().m_vgInfo;
            foreach (const VGDATA &vgdata, info.m_vglist) {
                VGInfo vgInfo = mapVGInfo.value(vgdata.m_vgName);
                if (vgInfo.isPartial()) {
                    isVGNormal = false;
                    break;
                }
            }

            m_btnDeletePV->setEnabled(isVGNormal);
            m_btnCreateLV->setDisabled(true);
            m_btnFormat->setDisabled(true);
        } else if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS && info.m_partition.size() == 0) {
            qCDebug(appLog) << "Disk is in LUKS.";
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            m_btnDeletePV->setDisabled(true);
            if (luksInfo.isDecrypt) {
                if (luksInfo.m_mapper.m_mountPoints.isEmpty()) {
                    updatePartitionBtnStatus(true, false, false, true, true);
                } else {
                    updatePartitionBtnStatus(true, true, true, false, true);
                }
            } else {
                updatePartitionBtnStatus(true, false, false, true, true);
            }
        } else {
            qCDebug(appLog) << "Disk is not in PV or LUKS.";
            m_btnParted->show();
            m_btnCreateLV->hide();

            m_btnDeletePV->setDisabled(true);
            m_btnParted->setDisabled(true);
            if (info.m_path.startsWith("/dev/sr") || info.m_path.startsWith("/dev/cdrom")) {
                // CD,DVD disable format
                m_btnFormat->setDisabled(true);
            } else {
                bool existMounted = DMDbusHandler::instance()->isExistMountPartition(info);
                m_btnFormat->setDisabled(existMounted);
            }
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::VOLUMEGROUP) {
        qCDebug(appLog) << "Current level is VG.";
        updateBtnShowStatus(false, true, true, true, false, true, false, true);

        // 如果VG异常，所有相关操作禁用
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        if (vgInfo.isPartial()) {
            updateVGBtnStatus(true, true, true, true, true, true);
            return;
        }

        updateVGBtnStatus(false, false, true, true, true, false);

        QMap<QString, QString> isExistUnallocated = DMDbusHandler::instance()->getIsExistUnallocated();
        if (isExistUnallocated.value(vgInfo.m_vgName) == "false") {
            m_btnCreateLV->setDisabled(true);
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        qCDebug(appLog) << "Current level is LV.";
        updateBtnShowStatus(true, false, true, true, false, true, true, false);

        // 如果VG异常，所有相关操作禁用
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        if (vgInfo.isPartial()) {
            qCDebug(appLog) << "VG is partial.";
            updateLVBtnStatus(true, true, true, true, true, true);
            return;
        }

        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qCDebug(appLog) << "LV is in LUKS.";
            m_btnResizeLV->setDisabled(true);
            m_btnCreateLV->setDisabled(true);

            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath);
            updateEncryptDeviceBtnStatus(luksInfo);
            return;
        }

        if (lvInfo.m_lvName.isEmpty() && lvInfo.m_lvUuid.isEmpty()) {
            qCDebug(appLog) << "LV is not exist.";
            updateLVBtnStatus(true, false, true, true, true, true);
        } else if (lvInfo.m_lvFsType == FSType::FS_LINUX_SWAP) {
            qCDebug(appLog) << "LV is swap.";
            updateLVBtnStatus(true, true, true, true, true, false);
        } else {
            qCDebug(appLog) << "LV is not swap.";
            bool noMountPoint = lvInfo.m_mountPoints.isEmpty();
            updateLVBtnStatus(false, true, !noMountPoint, !noMountPoint, noMountPoint, false);
        }
    }
    qCDebug(appLog) << "TitleWidget::updateLVBtnStatus completed.";
}

void TitleWidget::updatePartitionBtnStatus(const bool &isCreate, const bool &isWipe, const bool &isMount,
                                           const bool &isUnmount, const bool &isResize)
{
    qCDebug(appLog) << "Update partition button status.";
    m_btnParted->setDisabled(isCreate);
    m_btnFormat->setDisabled(isWipe);
    m_btnMount->setDisabled(isMount);
    m_btnUnmount->setDisabled(isUnmount);
    m_btnResize->setDisabled(isResize);
}

void TitleWidget::updateVGBtnStatus(const bool &isDeleteVG, const bool &isCreateLV, const bool &isWipe,
                                    const bool &isMount, const bool &isUnmount, const bool &isResize)
{
    qCDebug(appLog) << "Update VG button status.";
    m_btnDeleteVG->setDisabled(isDeleteVG);
    m_btnCreateLV->setDisabled(isCreateLV);
    m_btnFormat->setDisabled(isWipe);
    m_btnMount->setDisabled(isMount);
    m_btnUnmount->setDisabled(isUnmount);
    m_btnResizeVG->setDisabled(isResize);
}

void TitleWidget::updateLVBtnStatus(const bool &isDeleteLV, const bool &isCreateLV, const bool &isWipe,
                       const bool &isMount, const bool &isUnmount, const bool &isResize)
{
    qCDebug(appLog) << "Update LV button status.";
    m_btnDeleteLV->setDisabled(isDeleteLV);
    m_btnCreateLV->setDisabled(isCreateLV);
    m_btnFormat->setDisabled(isWipe);
    m_btnMount->setDisabled(isMount);
    m_btnUnmount->setDisabled(isUnmount);
    m_btnResizeLV->setDisabled(isResize);
}

void TitleWidget::updateBtnShowStatus(const bool &isDeleteVGShow, const bool &isDeleteLVShow, const bool &isDeletePVShow,
                                      const bool &isPartedShow, const bool &isCreateLVShow, const bool &isResizShow,
                                      const bool &isResizeVGShow, const bool &isResizeLVShow)
{
    qCDebug(appLog) << "Update button show status.";
    m_btnDeleteVG->setHidden(isDeleteVGShow);
    m_btnDeleteLV->setHidden(isDeleteLVShow);
    m_btnDeletePV->setHidden(isDeletePVShow);
    m_btnParted->setHidden(isPartedShow);
    m_btnCreateLV->setHidden(isCreateLVShow);
    m_btnResize->setHidden(isResizShow);
    m_btnResizeVG->setHidden(isResizeVGShow);
    m_btnResizeLV->setHidden(isResizeLVShow);
}

void TitleWidget::updateEncryptDeviceBtnStatus(const LUKS_INFO &luksInfo)
{
    qCDebug(appLog) << "Update encrypt device button status.";
    if (luksInfo.isDecrypt) {
        qCDebug(appLog) << "Device is decrypt.";
        if (luksInfo.m_mapper.m_luksFs == FSType::FS_LVM2_PV) {
            qCDebug(appLog) << "Device is PV.";
            m_btnFormat->setDisabled(true);
            m_btnMount->setDisabled(true);
            m_btnUnmount->setDisabled(true);
        } else if (luksInfo.m_mapper.m_mountPoints.isEmpty()) {
            qCDebug(appLog) << "Device is not mount.";
            m_btnFormat->setDisabled(false);
            m_btnMount->setDisabled(false);
            m_btnUnmount->setDisabled(true);
        } else {
            qCDebug(appLog) << "Device is mount.";
            m_btnFormat->setDisabled(true);
            m_btnMount->setDisabled(true);
            m_btnUnmount->setDisabled(false);
        }
    } else {
        qCDebug(appLog) << "Device is not decrypt.";
        m_btnFormat->setDisabled(false);
        m_btnMount->setDisabled(false);
        m_btnUnmount->setDisabled(true);
    }
}

void TitleWidget::onCurSelectChanged()
{
    qCDebug(appLog) << "TitleWidget::onCurSelectChanged";
    updateBtnStatus();
    qCDebug(appLog) << "Current selection changed";
}

void TitleWidget::onUpdateUsb()
{
    qCDebug(appLog) << "TitleWidget::onUpdateUsb";
    if (m_curChooseDevicePath.isEmpty() && m_curChooseVGName.isEmpty()) {
        qCDebug(appLog) << "No device or VG selected.";
        return;
    }

    if (!m_curChooseDevicePath.isEmpty()) {
        qCDebug(appLog) << "Update device: " << m_curChooseDevicePath;
        QStringList deviceNameList = DMDbusHandler::instance()->getDeviceNameList();
        if (deviceNameList.indexOf(m_curChooseDevicePath) != -1) {
            qCDebug(appLog) << "Device is already in list.";
            return;
        }

        QWidgetList widgetList = QApplication::topLevelWidgets();
        for (int i = 0; i < widgetList.count(); i++) {
            QWidget *widget = widgetList.at(i);

            if (widget->objectName() == "partitionDialog" || widget->objectName() == "partitionWidget" ||
                    widget->objectName() == "wipeDialog" || widget->objectName() == "mountDialog" ||
                    widget->objectName() == "unmountDialog" || widget->objectName() == "resizeDialog" ||
                    widget->objectName() == "mountMessageBox" || widget->objectName() == "firstWarning" ||
                    widget->objectName() == "secondWarning" || widget->objectName() == "RemovePVWidget" ||
                    widget->objectName() == "messageBox" || widget->objectName() == "decryptDialog" ||
                    widget->objectName() == "passwordInputDialog") {

                widget->close();
                //            break;
            }
        }

        setCurDevicePath("");
    } else if (!m_curChooseVGName.isEmpty()) {
        qCDebug(appLog) << "Update VG: " << m_curChooseVGName;
        QStringList vgNameList = DMDbusHandler::instance()->getVGNameList();
        if (vgNameList.indexOf(m_curChooseVGName) != -1) {
            qCDebug(appLog) << "VG is already in list.";
            return;
        }

        QWidgetList widgetList = QApplication::topLevelWidgets();
        for (int i = 0; i < widgetList.count(); i++) {
            QWidget *widget = widgetList.at(i);
            if (widget->objectName() == "ResizeVGWidget" || widget->objectName() == "messageBox" ||
                    widget->objectName() == "resizeLVDialog" || widget->objectName() == "createLVDialog" ||
                    widget->objectName() == "createLVWidget" || widget->objectName() == "decryptDialog" ||
                    widget->objectName() == "passwordInputDialog") {
                widget->close();
            }
        }

        setCurVGName("");
    }
    qCDebug(appLog) << "Update VG completed.";
}

void TitleWidget::setCurDevicePath(const QString &devPath)
{
    qCDebug(appLog) << "TitleWidget::setCurDevicePath";
    m_curChooseDevicePath = devPath;
}

void TitleWidget::setCurVGName(const QString &vgName)
{
    qCDebug(appLog) << "TitleWidget::setCurVGName";
    m_curChooseVGName = vgName;
}

void TitleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        qCDebug(appLog) << "TitleWidget::mousePressEvent";
        return;
    }

    QWidget::mousePressEvent(event);
}

