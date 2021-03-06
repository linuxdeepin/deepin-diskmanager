/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file devicelistwidget.cpp
 *
 * @brief 主要实现磁盘树结构展示以及右键磁盘信息、硬盘健康检测、分区表错误处理、隐藏分区、显示分区、删除分区等操作
 *
 * @date 2020-09-03 14:54
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
#include "devicelistwidget.h"
#include "customcontrol/dmdiskinfobox.h"
#include "diskinfodisplaydialog.h"
#include "diskhealthdetectiondialog.h"
#include "messagebox.h"
#include "partitiontableerrorsinfodialog.h"
#include "diskbadsectorsdialog.h"
#include "createpartitiontabledialog.h"

#include <DPalette>
#include <DMenu>
#include <DMessageManager>

#include <QVBoxLayout>
#include <QDebug>
#include <QApplication>

DeviceListWidget::DeviceListWidget(QWidget *parent)
    : DWidget(parent)
{
    m_curChooseDevicePath = "";

    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::white));
    setPalette(plt);
    setMaximumWidth(360);
    setMinimumWidth(100);
    initUi();
    initConnection();
}

DeviceListWidget::~DeviceListWidget()
{
    //    delete m_box;
    //    delete m_childbox;
}

void DeviceListWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
    m_treeView = new DmTreeview(this);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_treeView);
}

void DeviceListWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::updateDeviceInfo, this, &DeviceListWidget::onUpdateDeviceInfo);
    connect(m_treeView, &DmTreeview::curSelectChanged, DMDbusHandler::instance(), &DMDbusHandler::onSetCurSelect);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &DeviceListWidget::treeMenu);
    connect(DMDbusHandler::instance(), &DMDbusHandler::unmountPartitionMessage, this, &DeviceListWidget::onUnmountPartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::deletePartitionMessage, this, &DeviceListWidget::onDeletePartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::hidePartitionMessage, this, &DeviceListWidget::onHidePartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::showPartitionMessage, this, &DeviceListWidget::onShowPartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::createPartitionTableMessage, this, &DeviceListWidget::onCreatePartitionTableMessage);
    connect(DMDbusHandler::instance(), &DMDbusHandler::updateUsb, this, &DeviceListWidget::onUpdateUsb);
}

void DeviceListWidget::treeMenu(const QPoint &pos)
{
    QModelIndex curIndex = m_treeView->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(),0); //该行的第1列元素的index

    if (!index.isValid()) {
        return;
    }

    m_curDiskInfoData = m_treeView->getCurItem()->data().value<DiskInfoData>();
    if (m_curDiskInfoData.m_level == 0) {
        QMenu *menu = new QMenu();
        menu->setObjectName("treeMenu");
        menu->setAccessibleName("menu");

        QAction *actionInfo = new QAction();
        actionInfo->setText(tr("Disk info")); // 磁盘信息
        actionInfo->setObjectName("Disk info");
        menu->addAction(actionInfo);
        connect(actionInfo, &QAction::triggered, this, &DeviceListWidget::onDiskInfoClicked);

        //        menu->addSeparator();    //添加一个分隔线
        QMenu *itemChildMenu = new QMenu();
        itemChildMenu->setTitle(tr("Health management")); // 健康管理
        itemChildMenu->setAccessibleName("Health management");
        menu->addMenu(itemChildMenu);

        QAction *actionHealthDetection = new QAction();
        actionHealthDetection->setText(tr("Check health")); // 硬盘健康检测
        actionHealthDetection->setObjectName("Check health");
        itemChildMenu->addAction(actionHealthDetection);
        connect(actionHealthDetection, &QAction::triggered, this, &DeviceListWidget::onDiskCheckHealthClicked);

        QAction *actionCheckError = new QAction();
        actionCheckError->setText(tr("Check partition table error")); // 分区表错误检测
        actionCheckError->setObjectName("Check partition table error");
        itemChildMenu->addAction(actionCheckError);
        connect(actionCheckError, &QAction::triggered, this, &DeviceListWidget::onPartitionErrorCheckClicked);

        QAction *actionVerifyRepair = new QAction();
        actionVerifyRepair->setText(tr("Verify or repair bad sectors")); // 坏道检测与修复
        actionVerifyRepair->setObjectName("Verify or repair bad sectors");
        itemChildMenu->addAction(actionVerifyRepair);
        connect(actionVerifyRepair, &QAction::triggered, this, &DeviceListWidget::onDiskBadSectorsClicked);

        QAction *createPartitionTable = new QAction();
        createPartitionTable->setText(tr("Create partition table")); // 新建分区表
        createPartitionTable->setObjectName("Create partition table");
        menu->addAction(createPartitionTable);
        connect(createPartitionTable, &QAction::triggered, this, &DeviceListWidget::onCreatePartitionTableClicked);

        menu->exec(QCursor::pos());  //显示菜单
    } else {
        QMenu *menu = new QMenu();
        menu->setObjectName("treeMenu");
        menu->setAccessibleName("menu");

        QAction *actionHidePartition = new QAction();
        actionHidePartition->setText(tr("Hide partition")); // 隐藏分区
        actionHidePartition->setObjectName("Hide partition");
        menu->addAction(actionHidePartition);
        connect(actionHidePartition, &QAction::triggered, this, &DeviceListWidget::onHidePartitionClicked);

        QAction *actionShowPartition = new QAction();
        actionShowPartition->setText(tr("Unhide partition")); // 显示分区
        actionShowPartition->setObjectName("Unhide partition");
        menu->addAction(actionShowPartition);
        connect(actionShowPartition, &QAction::triggered, this, &DeviceListWidget::onShowPartitionClicked);
        //            actionShowPartition->setDisabled(true); // 将按钮置为不可选

        QAction *actionDelete = new QAction();
        actionDelete->setText(tr("Delete partition")); // 删除分区
        actionDelete->setObjectName("Delete partition");
        menu->addAction(actionDelete);
        connect(actionDelete, &QAction::triggered, this, &DeviceListWidget::onDeletePartitionClicked);

        if (!m_curDiskInfoData.m_mountpoints.isEmpty()) {
            actionDelete->setDisabled(true);
        }

        if (m_curDiskInfoData.m_fstype == "unallocated" || m_curDiskInfoData.m_fstype == "linux-swap") {
            actionHidePartition->setDisabled(true);
            actionShowPartition->setDisabled(true);
            actionDelete->setDisabled(true);
        } else {
            if (1 == m_curDiskInfoData.m_flag) {
                actionHidePartition->setDisabled(true);
                actionShowPartition->setDisabled(false);
            } else {
                actionHidePartition->setDisabled(false);
                actionShowPartition->setDisabled(true);
            }
        }

        menu->exec(QCursor::pos());  //显示菜单
    }
}

void DeviceListWidget::onDiskInfoClicked()
{
    m_curChooseDevicePath = m_curDiskInfoData.m_diskPath;

    DiskInfoDisplayDialog diskInfoDisplayDialog(m_curDiskInfoData.m_diskPath);
    diskInfoDisplayDialog.setObjectName("diskInfoDisplayDialog");
    diskInfoDisplayDialog.exec();

    m_curChooseDevicePath = "";
}

void DeviceListWidget::onDiskCheckHealthClicked()
{
    m_curChooseDevicePath = m_curDiskInfoData.m_diskPath;

    DiskHealthDetectionDialog diskHealthDetectionDialog(m_curDiskInfoData.m_diskPath);
    diskHealthDetectionDialog.setObjectName("diskHealthDetectionDialog");
    diskHealthDetectionDialog.exec();

    m_curChooseDevicePath = "";
}

void DeviceListWidget::onDiskBadSectorsClicked()
{
    m_curChooseDevicePath = m_curDiskInfoData.m_diskPath;

    DiskBadSectorsDialog diskBadSectorsDialog;
    diskBadSectorsDialog.setObjectName("diskBadSectorsDialog");
    diskBadSectorsDialog.exec();

    m_curChooseDevicePath = "";
}

bool DeviceListWidget::isExistMountPartition()
{
    bool isExist = false;

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    for (int i = 0; i < info.partition.size(); i++) {
        PartitionInfo partitionInfo = info.partition.at(i);

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

void DeviceListWidget::onCreatePartitionTableClicked()
{
    if (isExistMountPartition()) {
        MessageBox warningBox;
        // 请先卸载当前磁盘中的所有分区  确定
        warningBox.setWarings(tr("Please unmount all partitions in the disk first"), "", tr("OK"));
        warningBox.exec();
        
        return;
    }

    MessageBox messageBox;
    messageBox.setObjectName("messageBox");
    // 新建分区表之后将会合并当前磁盘所有分区，丢失所有数据，请谨慎使用  继续  取消
    messageBox.setWarings(tr("All partitions in this disk will be merged and all data\n will be lost if creating a new partition table,\n please take it carefully"), "", tr("Proceed"), tr("Cancel"));
    if (messageBox.exec() == DDialog::Accepted) {
        CreatePartitionTableDialog createPartitionTableDialog;
        createPartitionTableDialog.setObjectName("createPartitionTable");
        createPartitionTableDialog.exec();
    }
}

void DeviceListWidget::onPartitionErrorCheckClicked()
{
    bool result = DMDbusHandler::instance()->detectionPartitionTableError(m_curDiskInfoData.m_diskPath);
    if (result) {
        QString deviceInfo = QString("%1(%2)").arg(m_curDiskInfoData.m_diskPath).arg(m_curDiskInfoData.m_diskSize);
        m_curChooseDevicePath = m_curDiskInfoData.m_diskPath;

        PartitionTableErrorsInfoDialog partitionTableErrorsInfoDialog(deviceInfo);
        partitionTableErrorsInfoDialog.setObjectName("partitionErrorCheck");
        partitionTableErrorsInfoDialog.exec();

        m_curChooseDevicePath = "";
    } else {
        // 分区表检测正常
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("No errors found in the partition table"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onHidePartitionClicked()
{
    MessageBox messageBox;
    messageBox.setObjectName("messageBox");
    // 您是否要隐藏该分区？ 隐藏  取消
    messageBox.setWarings(tr("Do you want to hide this partition?"), "", tr("Hide"), tr("Cancel"));
    if (messageBox.exec() == DDialog::Accepted) {
        if (m_curDiskInfoData.m_mountpoints == "/boot/efi" || m_curDiskInfoData.m_mountpoints == "/boot"
                || m_curDiskInfoData.m_mountpoints == "/" || m_curDiskInfoData.m_mountpoints.contains("/data")) {
            isHideSuccess = false;
            // 隐藏分区失败！无法锁定该分区
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Failed to hide the partition: unable to lock it"));
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
        } else {
            if (m_curDiskInfoData.m_mountpoints.isEmpty()) {
                DMDbusHandler::instance()->hidePartition();
            } else {
                isHideSuccess = false;
                // 只有处于卸载状态的分区才能被隐藏
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("You can only hide the unmounted partition"));
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            }
        }
    }
}

void DeviceListWidget::onShowPartitionClicked()
{
    MessageBox messageBox;
    messageBox.setObjectName("showMessageBox");
    // 您是否要显示该隐藏分区？ 显示  取消
    messageBox.setWarings(tr("Do you want to unhide this partition?"), "", tr("Unhide"), tr("Cancel"));
    if (messageBox.exec() == DDialog::Accepted) {
        DMDbusHandler::instance()->unhidePartition();
    }
}

void DeviceListWidget::onDeletePartitionClicked()
{
    MessageBox messageBox;
    // 您确定要删除该分区吗？ 该分区内所有文件将会丢失  删除  取消
    messageBox.setWarings(tr("Are you sure you want to delete this partition?"), tr("You will lose all data in it"), tr("Delete"), DDialog::ButtonWarning, tr("Cancel"));
    if (messageBox.exec() == DDialog::Accepted) {
        if (m_curDiskInfoData.m_mountpoints == "/boot/efi" || m_curDiskInfoData.m_mountpoints == "/boot"
                || m_curDiskInfoData.m_mountpoints == "/" || m_curDiskInfoData.m_mountpoints.contains("/data")
                || m_curDiskInfoData.m_fstype == "linux-swap") {
            isDeleteSuccess = false;
            // 删除分区失败！无法锁定该分区
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Failed to delete the partition: unable to lock it"));
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
        } else {
            DMDbusHandler::instance()->deletePartition();
        }
    }
}

void DeviceListWidget::onHidePartition(const QString &hideMessage)
{
    if ("1" == hideMessage) {
        // 隐藏分区成功
        isHideSuccess = true;
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Hide the partition successfully"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        isHideSuccess = false;
        // 隐藏分区失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Failed to hide the partition"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onShowPartition(const QString &showMessage)
{
    if ("1" == showMessage) {
        // 显示分区成功
        isShowSuccess = true;
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Unhide the partition successfully"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        isShowSuccess = false;
        // 显示分区失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Failed to unhide the partition"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onDeletePartition(const QString &deleteMessage)
{
    QStringList infoList = deleteMessage.split(":");

    if ("1" == infoList.at(0)) {
        // 删除分区成功
        isDeleteSuccess = true;
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Delete the partition successfully"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        isDeleteSuccess = false;
        // 删除分区失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Failed to delete the partition: %1").arg(infoList.at(1)));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onUnmountPartition(const QString &unmountMessage)
{
    if ("1" == unmountMessage) {
        isUnmountSuccess = true;
        // 卸载成功
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Unmounting successful"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        isUnmountSuccess = false;
        // 卸载失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Unmounting failed"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onCreatePartitionTableMessage(const bool &flag)
{
    if (flag) {
        isCreateTableSuccess = true;
        // 新建分区表成功
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Creating partition table successful"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        isCreateTableSuccess = false;
        // 新建分区表失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Creating partition table failed"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onUpdateUsb()
{
    if (m_curChooseDevicePath == "")
        return;

    QStringList deviceNameList = DMDbusHandler::instance()->getDeviceNameList();
    if (deviceNameList.indexOf(m_curChooseDevicePath) != -1)
        return;

    QWidgetList widgetList = QApplication::topLevelWidgets();
    for (int i = 0; i < widgetList.count(); i++) {
        QWidget *widget = widgetList.at(i);
        if (widget->objectName() == "diskBadSectorsDialog" ||  widget->objectName() == "diskInfoDisplayDialog" ||
                widget->objectName() == "diskHealthDetectionDialog" || widget->objectName() == "partitionErrorCheck") {
            widget->close();
            break;
        }
    }
}

void DeviceListWidget::onUpdateDeviceInfo()
{
    //更新DmTreeview  lx
    //设置当前选项
    auto handler = DMDbusHandler::instance();
    m_num = handler->getCurPartititonInfo().m_partitionNumber;
    m_deviceNum = m_treeView->getCurrentTopNum();
    m_devicePath = handler->getCurPartititonInfo().m_devicePath;
    m_treeView->m_model->clear();
    DeviceInfoMap infoMap = DMDbusHandler::instance()->probDeviceInfo();
    QMap<int, int> countMap;
    int deviceCount = 0;

    for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
        DeviceInfo info = it.value();
        QString diskSize = Utils::formatSize(info.length, info.sector_size);
        auto diskinfoBox = new DmDiskinfoBox(0, this, info.m_path, diskSize);
        int partitionCount = 0;

        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            QString partitionSize = Utils::formatSize(it->m_sectorEnd - it->m_sectorStart + 1, it->m_sectorSize);
            QString partitionPath = it->m_path.remove(0, 5);
            QString unused = Utils::formatSize(it->m_sectorsUsed, it->m_sectorSize);
            QString used = Utils::formatSize(it->m_sectorsUnused, it->m_sectorSize);
            FSType fstype = static_cast<FSType>(it->m_fileSystemType);
            QString fstypeName = Utils::fileSystemTypeToString(fstype);
            QString mountpoint;
            QString mountpoints;

            for (int i = 0; i < it->m_mountPoints.size(); i++) {
                mountpoint = it->m_mountPoints[i];
                mountpoints += it->m_mountPoints[i];
            }

            QString fileSystemLabel = it->m_fileSystemLabel;
            auto childDiskinfoBox = new DmDiskinfoBox(1, this, it->m_devicePath, "", partitionPath, partitionSize, used, unused, it->m_sectorsUnallocated,
                                                it->m_sectorStart, it->m_sectorEnd, fstypeName, mountpoints, fileSystemLabel, it->m_flag);
            diskinfoBox->m_childs.append(childDiskinfoBox);

            partitionCount ++;
        }

        m_treeView->addTopItem(diskinfoBox, m_addItem);

        countMap[deviceCount] = partitionCount;
        deviceCount ++;
    }

    QStringList deviceNameList = DMDbusHandler::instance()->getDeviceNameList();

    if (deviceNameList.indexOf(DMDbusHandler::instance()->getCurPartititonInfo().m_devicePath) == -1) {
        m_flag = 0;
    } else {
        m_deviceNum = deviceNameList.indexOf(DMDbusHandler::instance()->getCurPartititonInfo().m_devicePath);
    }

    m_addItem = 1;

    if (m_flag == 0) {
        m_treeView->setDefaultdmItem();
    } else {
        int index = m_treeView->getCurrentNum();
        if (countMap.value(m_deviceNum) == index) {
            index = m_treeView->getCurrentNum() - 1;
        }

        m_treeView->setRefreshItem(m_deviceNum, index);
    }

    m_flag += 1;
}
