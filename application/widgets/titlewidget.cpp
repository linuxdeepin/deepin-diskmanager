/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file titlewidget.cpp
 *
 * @brief 标题功能图标按钮类，主要实现功能图标按钮展示以及点击操作
 *
 * @date 2020-09-04 14:06
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
#include "titlewidget.h"
#include "mountdialog.h"
#include "unmountdialog.h"
#include "formatedialog.h"
#include "resizedialog.h"
#include "createlvwidget.h"
#include "common.h"
#include "messagebox.h"

#include <QMouseEvent>
#include <QHBoxLayout>
#include <QApplication>

TitleWidget::TitleWidget(DWidget *parent)
    : DWidget(parent)
{
    initUi();
    initConnection();
}

void TitleWidget::initUi()
{
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

    m_btnCreateVG->setIconSize(QSize(16, 16));
    m_btnParted->setIconSize(QSize(16, 16));
    m_btnFormat->setIconSize(QSize(16, 16));
    m_btnMount->setIconSize(QSize(14, 17));
    m_btnUnmount->setIconSize(QSize(14, 15));
    m_btnResize->setIconSize(QSize(16, 16));
    m_btnCreateLV->setIconSize(QSize(16, 16));
    m_btnResizeVG->setIconSize(QSize(16, 16));
    m_btnResizeLV->setIconSize(QSize(16, 16));
    m_btnDeletePV->setIconSize(QSize(16, 16));
    m_btnDeleteVG->setIconSize(QSize(16, 16));
    m_btnDeleteLV->setIconSize(QSize(16, 16));

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
}

void TitleWidget::initConnection()
{
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
}

DPushButton *TitleWidget::createBtn(const QString &btnName, const QString &objName, bool bCheckable)
{
    auto btn = new DPushButton(this);
    //  wzx 设置图标icon    2020-05-19
    QIcon icon = Common::getIcon(objName);

    btn->setIcon(icon);
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(18, 18));
    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    btn->setObjectName(objName);
    btn->setAccessibleName(objName);

    if (bCheckable) {
        btn->setChecked(false);
    }

    return btn;
}

void TitleWidget::showPartInfoWidget()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    PartitionDialog dlg(this);
    dlg.setObjectName("partitionDialog");
    dlg.setAccessibleName("partitionDialog");

    setCurDevicePath(info.m_devicePath);

    if (dlg.exec() == 1) {
        if (TYPE_UNPARTITIONED == info.m_type && FS_UNALLOCATED == info.m_fileSystemType) {
            setCurDevicePath("");
            qDebug() << QString("No partition table found on device %1").arg(info.m_devicePath);
            qDebug() << "A partition table is required before partitions can be added";
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
    }
}

void TitleWidget::showFormateInfoWidget()
{
    setCurDevicePath(DMDbusHandler::instance()->getCurDevicePath());

    FormateDialog dlg(this);
    dlg.setObjectName("wipeDialog");
    dlg.setAccessibleName("wipeDialog");
    dlg.exec();

    setCurDevicePath("");
}

void TitleWidget::showMountInfoWidget()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setCurDevicePath(info.m_devicePath);

    MountDialog dlg(this);
    dlg.setObjectName("mountDialog");
    dlg.setAccessibleName("mountDialog");
    dlg.exec();

    setCurDevicePath("");
}

void TitleWidget::showUnmountInfoWidget()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setCurDevicePath(info.m_devicePath);

    UnmountDialog dlg(this);
    dlg.setObjectName("unmountDialog");
    dlg.setAccessibleName("unmountDialog");
    dlg.exec();

    setCurDevicePath("");
}

void TitleWidget::showResizeInfoWidget()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setCurDevicePath(info.m_devicePath);

    ResizeDialog dlg(this);
    dlg.setObjectName("resizeDialog");
    dlg.setAccessibleName("resizeDialog");
    dlg.exec();

    setCurDevicePath("");
}

void TitleWidget::onCreateLVClicked()
{
    PartitionDialog dlg(this);
    dlg.setTitleText(tr("Create logical volume"), tr("The disks will be formatted if you create a logical volume"));
    dlg.setObjectName("createLVDialog");
    dlg.setAccessibleName("createLVDialog");

    if (dlg.exec() == 1) {
        CreateLVWidget createLVWidget(this);
        createLVWidget.setObjectName("createLVWidget");
        createLVWidget.setAccessibleName("createLVWidget");
        createLVWidget.exec();
    }
}

void TitleWidget::onDeleteLVClicked()
{
    LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
    QString mountPoint = "";
    for (int i = 0; i < lvInfo.m_mountPoints.size(); i++) {
        mountPoint += lvInfo.m_mountPoints[i];
    }

    if (!mountPoint.isEmpty()){
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("messageBox");
        // 请先手动卸载XXX（逻辑卷名称）  确定
        warningBox.setWarings(tr("Unmount %1 first").arg(lvInfo.m_lvName), "", tr("OK"), "ok");
        warningBox.exec();

        return;
    }

    MessageBox messageBox(this);
    messageBox.setObjectName("messageBox");
    messageBox.setAccessibleName("deleteLVMessageBox");
    // 删除后数据将无法恢复，请确认  删除  取消
    messageBox.setWarings(tr("Data cannot be recovered if deleted, please confirm before proceeding"), "",
                          tr("Delete"), DDialog::ButtonWarning, "delete", tr("Cancel"), "cancel");
    if (messageBox.exec() == DDialog::Accepted) {
        QStringList lvNameList;
        lvNameList << lvInfo.m_lvPath;

        DMDbusHandler::instance()->deleteLV(lvNameList);
    }
}

void TitleWidget::onResizeLVClicked()
{

}

void TitleWidget::onCreateVGClicked()
{
    PartitionDialog dlg(this);
    dlg.setTitleText(tr("Create volume group"), tr("The disks will be formatted if you create a volume group"));
    dlg.setObjectName("createVGDialog");
    dlg.setAccessibleName("createVGDialog");

    if (dlg.exec() == 1) {

    }
}

void TitleWidget::onDeleteVGClicked()
{
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    if (DMDbusHandler::instance()->isExistMountLV()){
        MessageBox warningBox(this);
        warningBox.setObjectName("messageBox");
        warningBox.setAccessibleName("messageBox");
        // 请先手动卸载XXX（逻辑卷组名称）  确定
        warningBox.setWarings(tr("Unmount %1 first").arg(vgInfo.m_vgName), "", tr("OK"), "ok");
        warningBox.exec();

        return;
    }

    MessageBox messageBox(this);
    messageBox.setObjectName("messageBox");
    messageBox.setAccessibleName("deleteVGMessageBox");
    // 删除后数据将无法恢复，请确认  删除  取消
    messageBox.setWarings(tr("Data cannot be recovered if deleted, please confirm before proceeding"), "",
                          tr("Delete"), DDialog::ButtonWarning, "delete", tr("Cancel"), "cancel");
    if (messageBox.exec() == DDialog::Accepted) {
        QStringList vgNameList;
        vgNameList << vgInfo.m_vgName;

        DMDbusHandler::instance()->deleteVG(vgNameList);
    }
}

void TitleWidget::onResizeVGClicked()
{

}

void TitleWidget::onDeletePVClicked()
{
    MessageBox messageBox(this);
    messageBox.setObjectName("messageBox");
    messageBox.setAccessibleName("deletePVMessageBox");
    // 您确定要删除该物理卷吗？ 其包含的所有文件将会丢失  删除  取消
    messageBox.setWarings(tr("Are you sure you want to delete the physical volume?"), tr("You will lose all data in it"),
                          tr("Delete"), DDialog::ButtonWarning, "delete", tr("Cancel"), "cancel");
    if (messageBox.exec() == DDialog::Accepted) {

    }
}

bool TitleWidget::isExistMountPartition()
{
    bool isExist = false;

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    for (int i = 0; i < info.m_partition.size(); i++) {
        PartitionInfo partitionInfo = info.m_partition.at(i);

        QString mountpoints;
        for (int j = 0; j < partitionInfo.m_mountPoints.size(); j++) {
            mountpoints += partitionInfo.m_mountPoints[j];
        }

        if (!mountpoints.isEmpty()) {
            isExist = true;
            break;
        }
    }

    return isExist;
}

void TitleWidget::updateBtnStatus()
{
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::Partition) {
        m_btnDeletePV->show();
        m_btnResize->show();
        m_btnDeleteVG->hide();
        m_btnDeleteLV->hide();
        m_btnResizeLV->hide();
        m_btnResizeVG->hide();

        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        if (1 == info.m_vgFlag) {
            m_btnParted->hide();
            m_btnCreateLV->show();

            m_btnDeletePV->setDisabled(false);
            m_btnCreateLV->setDisabled(true);
            m_btnFormat->setDisabled(true);
            m_btnMount->setDisabled(true);
            m_btnUnmount->setDisabled(true);
            m_btnResize->setDisabled(true);
        } else {
            m_btnDeletePV->setDisabled(true);
            m_btnParted->show();
            m_btnCreateLV->hide();

            //已挂载
            if (info.m_mountPoints.size() > 0 && info.m_busy) {
                m_btnParted->setDisabled(true);
                m_btnFormat->setDisabled(true);
                m_btnMount->setDisabled(true);
                m_btnUnmount->setDisabled(false);
                m_btnResize->setDisabled(true);

                if (1 == info.m_flag) {
                    m_btnUnmount->setDisabled(true);
                }
            } else {
                int result = info.m_flag;
                if (1 == result) {
                    m_btnParted->setDisabled(true);
                    m_btnFormat->setDisabled(true);
                    m_btnMount->setDisabled(true);
                    m_btnUnmount->setDisabled(true);
                    m_btnResize->setDisabled(true);
                } else {
                    //需判断扩展分区上是否无分区，否则认为不可操作，此处省略操作
                    if (FS_EXTENDED == info.m_fileSystemType) {
                        m_btnParted->setDisabled(true);
                        m_btnFormat->setDisabled(true);
                        m_btnMount->setDisabled(true);
                        m_btnUnmount->setDisabled(true);
                        m_btnResize->setDisabled(true);
                    } else {
                        m_btnUnmount->setDisabled(true);
                        if (info.m_fileSystemType == FS_UNALLOCATED) {
                            m_btnParted->setDisabled(false);
                            m_btnFormat->setDisabled(false);
                            m_btnMount->setDisabled(true);
                            m_btnResize->setDisabled(true);
                        } else if (info.m_fileSystemType == FS_UNKNOWN) {
                            m_btnParted->setDisabled(true);
                            m_btnFormat->setDisabled(false);
                            m_btnMount->setDisabled(true);
                            m_btnResize->setDisabled(true);
                        } else {
                            m_btnParted->setDisabled(true);
                            m_btnFormat->setDisabled(false);
                            m_btnMount->setDisabled(false);
                            m_btnResize->setDisabled(false);
                        }
                    }
                }
            }

            qDebug() << info.m_type << info.m_fileSystemType;
            if (info.m_fileSystemType == FSType::FS_LINUX_SWAP) {
                m_btnParted->setDisabled(true);
                m_btnFormat->setDisabled(true);
                m_btnMount->setDisabled(true);
                m_btnUnmount->setDisabled(true);
                m_btnResize->setDisabled(true);
            }

            QMap<QString, QString> isExistUnallocated = DMDbusHandler::instance()->getIsExistUnallocated();
            if (isExistUnallocated.value(info.m_devicePath) == "false") {
                m_btnResize->setDisabled(true);
            }
        }

    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::Disk) {
        m_btnParted->show();
        m_btnDeletePV->show();
        m_btnResize->show();
        m_btnCreateLV->hide();
        m_btnDeleteVG->hide();
        m_btnDeleteLV->hide();
        m_btnResizeLV->hide();
        m_btnResizeVG->hide();

        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        if (1 == info.m_vgFlag) {
            m_btnParted->hide();
            m_btnCreateLV->show();

            m_btnDeletePV->setDisabled(false);
            m_btnCreateLV->setDisabled(true);
            m_btnFormat->setDisabled(true);
            m_btnMount->setDisabled(true);
            m_btnUnmount->setDisabled(true);
            m_btnResize->setDisabled(true);
        } else {
            m_btnDeletePV->setDisabled(true);
            m_btnParted->show();
            m_btnCreateLV->hide();

            if (isExistMountPartition()) {
                m_btnParted->setDisabled(true);
                m_btnFormat->setDisabled(true);
                m_btnMount->setDisabled(true);
                m_btnUnmount->setDisabled(true);
                m_btnResize->setDisabled(true);
            } else {
                m_btnParted->setDisabled(true);
                m_btnFormat->setDisabled(false);
                m_btnMount->setDisabled(true);
                m_btnUnmount->setDisabled(true);
                m_btnResize->setDisabled(true);
            }
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::VolumeGroup) {
        m_btnParted->hide();
        m_btnDeleteLV->hide();
        m_btnDeletePV->hide();
        m_btnResize->hide();
        m_btnResizeLV->hide();
        m_btnDeleteVG->show();
        m_btnCreateLV->show();
        m_btnResizeVG->show();

        m_btnFormat->setDisabled(true);
        m_btnMount->setDisabled(true);
        m_btnUnmount->setDisabled(true);
        m_btnDeleteVG->setDisabled(false);
        m_btnCreateLV->setDisabled(false);
        m_btnResizeVG->setDisabled(false);

        QMap<QString, QString> isExistUnallocated = DMDbusHandler::instance()->getIsExistUnallocated();
        if (isExistUnallocated.value(DMDbusHandler::instance()->getCurVGInfo().m_vgName) == "false") {
            m_btnCreateLV->setDisabled(true);
        }

    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LogicalVolume) {
        m_btnParted->hide();
        m_btnDeleteVG->hide();
        m_btnDeletePV->hide();
        m_btnResize->hide();
        m_btnResizeVG->hide();
        m_btnDeleteLV->show();
        m_btnCreateLV->show();
        m_btnResizeLV->show();

        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        QString mountPoint = "";
        for (int i = 0; i < lvInfo.m_mountPoints.size(); i++) {
            mountPoint += lvInfo.m_mountPoints[i];
        }

        if (mountPoint.isEmpty()) {
            m_btnFormat->setDisabled(false);
            m_btnMount->setDisabled(false);
            m_btnUnmount->setDisabled(true);
        } else {
            m_btnFormat->setDisabled(true);
            m_btnMount->setDisabled(true);
            m_btnUnmount->setDisabled(false);
        }

        if (lvInfo.m_lvName.isEmpty() && lvInfo.m_lvUuid.isEmpty()) {
            m_btnCreateLV->setDisabled(false);
            m_btnFormat->setDisabled(true);
            m_btnMount->setDisabled(true);
            m_btnUnmount->setDisabled(true);
            m_btnDeleteLV->setDisabled(true);
            m_btnResizeLV->setDisabled(true);
        } else {
            m_btnCreateLV->setDisabled(true);
            m_btnDeleteLV->setDisabled(false);
            m_btnResizeLV->setDisabled(false);
        }
    }
}

void TitleWidget::onCurSelectChanged()
{
    updateBtnStatus();
    qDebug() << __FUNCTION__ << "-1--1-";
}

void TitleWidget::onUpdateUsb()
{
    if (m_curChooseDevicePath == "") {
        return;
    }

    QStringList deviceNameList = DMDbusHandler::instance()->getDeviceNameList();
    if (deviceNameList.indexOf(m_curChooseDevicePath) != -1)
        return;

    QWidgetList widgetList = QApplication::topLevelWidgets();
    for (int i = 0; i < widgetList.count(); i++) {
        QWidget *widget = widgetList.at(i);

        if (widget->objectName() == "partitionDialog" || widget->objectName() == "partitionWidget" ||
                widget->objectName() == "wipeDialog" || widget->objectName() == "mountDialog" ||
                widget->objectName() == "unmountDialog" || widget->objectName() == "resizeDialog" ||
                widget->objectName() == "mountMessageBox" || widget->objectName() == "firstWarning" ||
                widget->objectName() == "secondWarning") {

            widget->close();
//            break;
        }
    }

    setCurDevicePath("");
}

void TitleWidget::setCurDevicePath(const QString &devPath)
{
    m_curChooseDevicePath = devPath;
}

void TitleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }

    QWidget::mousePressEvent(event);
}

