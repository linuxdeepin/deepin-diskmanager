/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "devicelistwidget.h"
#include "customcontrol/dmdiskinfobox.h"
#include "diskinfodisplaydialog.h"
#include "diskhealthdetectiondialog.h"
#include "messagebox.h"
#include "partitiontableerrorsinfodialog.h"

#include <DPalette>
#include <DMenu>
#include <DMessageManager>

#include <QVBoxLayout>
#include <QDebug>

DeviceListWidget::DeviceListWidget(QWidget *parent)
    : DWidget(parent)
{
    m_curChooseDevicePath = "";
    m_diskInfoDisplayDialog = nullptr;
    m_diskHealthDetectionDialog = nullptr;
    m_partitionTableErrorsInfoDialog = nullptr;

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
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigUpdateDeviceInfo, this, &DeviceListWidget::onUpdateDeviceInfo);
    connect(m_treeView, &DmTreeview::sigCurSelectChanged, DMDbusHandler::instance(), &DMDbusHandler::slotsetCurSelect);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &DeviceListWidget::treeMenu);
    connect(DMDbusHandler::instance(), &DMDbusHandler::deletePartitionMessage, this, &DeviceListWidget::onDeletePartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::hidePartitionMessage, this, &DeviceListWidget::onHidePartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::showPartitionMessage, this, &DeviceListWidget::onShowPartition);
    connect(DMDbusHandler::instance(), &DMDbusHandler::updateUsb, this, &DeviceListWidget::onUpdateUsb);
}

void DeviceListWidget::treeMenu(const QPoint &pos)
{
    QModelIndex curIndex = m_treeView->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(),0); //该行的第1列元素的index

    if (!index.isValid()) {
        return;
    }

    m_curDiskInfoData = m_treeView->getcuritem()->data().value<DiskInfoData>();
    if (m_curDiskInfoData.level == 0) {
        QMenu *menu = new QMenu();

        QAction *actionInfo = new QAction();
        actionInfo->setText(tr("Disk info")); // 磁盘信息
        actionInfo->setObjectName("Disk info");
        menu->addAction(actionInfo);
        connect(actionInfo, &QAction::triggered, this, &DeviceListWidget::onDiskInfoClicked);

        //        menu->addSeparator();    //添加一个分隔线
        QMenu *itemChildMenu = new QMenu();
        itemChildMenu->setTitle(tr("Health management")); // 健康管理
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

        //            QAction *actionVerifyRepair = new QAction();
        //            actionVerifyRepair->setText(tr("Verify or repair bad sectors")); // 坏道检测与修复
        //            actionVerifyRepair->setObjectName("Verify or repair bad sectors");
        //            itemChildMenu->addAction(actionVerifyRepair);

        menu->exec(QCursor::pos());  //显示菜单
    } else {
        QMenu *menu = new QMenu();

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

        if (!m_curDiskInfoData.mountpoints.isEmpty()) {
            actionDelete->setDisabled(true);
        }

        if (m_curDiskInfoData.fstype == "unallocated") {
            actionHidePartition->setDisabled(true);
            actionShowPartition->setDisabled(true);
            actionDelete->setDisabled(true);
        } else {
            if (1 == m_curDiskInfoData.flag) {
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
    m_curChooseDevicePath = m_curDiskInfoData.diskpath;

    m_diskInfoDisplayDialog = new DiskInfoDisplayDialog(m_curDiskInfoData.diskpath);
//        m_diskInfoDisplayDialog = diskInfoDisplayDialog;
    m_diskInfoDisplayDialog->exec();

    m_curChooseDevicePath = "";
    m_diskInfoDisplayDialog = nullptr;
}

void DeviceListWidget::onDiskCheckHealthClicked()
{
    m_curChooseDevicePath = m_curDiskInfoData.diskpath;

    m_diskHealthDetectionDialog = new DiskHealthDetectionDialog(m_curDiskInfoData.diskpath);
//        m_diskHealthDetectionDialog = diskHealthDetectionDialog;
    m_diskHealthDetectionDialog->exec();

    m_curChooseDevicePath = "";
    m_diskHealthDetectionDialog = nullptr;
}

void DeviceListWidget::onPartitionErrorCheckClicked()
{
    bool result = DMDbusHandler::instance()->detectionPartitionTableError(m_curDiskInfoData.diskpath);
    if (result) {
        QString deviceInfo = QString("%1(%2)").arg(m_curDiskInfoData.diskpath).arg(m_curDiskInfoData.disksize);
        m_curChooseDevicePath = m_curDiskInfoData.diskpath;

        m_partitionTableErrorsInfoDialog = new PartitionTableErrorsInfoDialog(deviceInfo);
//            m_partitionTableErrorsInfoDialog = partitionTableErrorsInfoDialog;
        m_partitionTableErrorsInfoDialog->exec();

        m_curChooseDevicePath = "";
        m_partitionTableErrorsInfoDialog = nullptr;
    } else {
        // 分区表检测正常
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("No errors found in the partition table"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onHidePartitionClicked()
{
    MessageBox messageBox;
    // 您是否要隐藏该分区？ 隐藏  取消
    messageBox.setWarings(tr("Do you want to hide this partition?"), "", tr("Hide"), tr("Cancel"));
    if (messageBox.exec() == 1) {
        if (m_curDiskInfoData.mountpoints == "/boot/efi" || m_curDiskInfoData.mountpoints == "/boot"
                || m_curDiskInfoData.mountpoints == "/" || m_curDiskInfoData.mountpoints.contains("/data")) {
            DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
            floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
            floMsg->setMessage(tr("Failed to hide the partition: unable to lock it")); // 隐藏分区失败！无法锁定该分区
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
        } else {
            if (m_curDiskInfoData.mountpoints.isEmpty()) {
                DMDbusHandler::instance()->hidePartition(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);
            } else {
                DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
                floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
                floMsg->setMessage(tr("You can only hide the unmounted partition")); // 只有处于卸载状态的分区才能被隐藏
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            }
        }
    }
}

void DeviceListWidget::onShowPartitionClicked()
{
    MessageBox messageBox;
    // 您是否要显示该隐藏分区？ 显示  取消
    messageBox.setWarings(tr("Do you want to unhide this partition?"), "", tr("Unhide"), tr("Cancel"));
    if (messageBox.exec() == 1) {
        DMDbusHandler::instance()->unhidePartition(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);
    }
}

void DeviceListWidget::onDeletePartitionClicked()
{
    MessageBox messageBox;
    // 您确定要删除该分区吗？ 该分区内所有文件将会丢失  删除  取消
    messageBox.setWarings(tr("Are you sure you want to delete this partition?"), tr("You will lose all data in it"), tr("Delete"), DDialog::ButtonWarning, tr("Cancel"));
    int ret = messageBox.exec();
    if (ret == 1) {
        if (m_curDiskInfoData.mountpoints == "/boot/efi" || m_curDiskInfoData.mountpoints == "/boot"
                || m_curDiskInfoData.mountpoints == "/" || m_curDiskInfoData.mountpoints.contains("/data")
                || m_curDiskInfoData.fstype == "linux-swap") {
            DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
            floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
            floMsg->setMessage(tr("Failed to delete the partition: unable to lock it")); // 删除分区失败！无法锁定该分区
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
        } else {
            DMDbusHandler::instance()->deletePartition(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);
        }
    }
}

void DeviceListWidget::onHidePartition(const QString &hideMessage)
{
    if ("1" == hideMessage) {
        // 隐藏分区成功
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Hide the partition successfully"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
        floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
        floMsg->setMessage(tr("Failed to hide the partition")); // 隐藏分区失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onShowPartition(const QString &showMessage)
{
    if ("1" == showMessage) {
        // 显示分区成功
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Unhide the partition successfully"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
        floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
        floMsg->setMessage(tr("Failed to unhide the partition")); // 显示分区失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    }
}

void DeviceListWidget::onDeletePartition(const QString &deleteMessage)
{
    QStringList infoList = deleteMessage.split(":");

    if ("1" == infoList.at(0)) {
        // 删除分区成功
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Delete the partition successfully"));
        DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
    } else {
        DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
        floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
        floMsg->setMessage(tr("Failed to delete the partition:%1").arg(infoList.at(1))); // 删除分区失败
        DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
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

    if (m_diskInfoDisplayDialog != nullptr) {
        m_diskInfoDisplayDialog->close();
        m_diskInfoDisplayDialog = nullptr;
    } else if (m_diskHealthDetectionDialog != nullptr) {
        m_diskHealthDetectionDialog->close();
        m_diskHealthDetectionDialog = nullptr;
    } else if (m_partitionTableErrorsInfoDialog != nullptr) {
        m_partitionTableErrorsInfoDialog->close();
        m_partitionTableErrorsInfoDialog = nullptr;
    }
}

void DeviceListWidget::onUpdateDeviceInfo()
{
    qDebug() << __FUNCTION__ << "               1";
    //更新DmTreeview  lx
    //设置当前选项
    auto handler = DMDbusHandler::instance();
    m_num = handler->getCurPartititonInfo().partition_number;
    m_deviceNum = m_treeView->currentTopNum();
    qDebug() << m_flag << m_treeView->currentTopNum();
    m_devicePath = handler->getCurPartititonInfo().device_path;
    m_treeView->m_model->clear();
    DeviceInfoMap infoMap = DMDbusHandler::instance()->probDeviceInfo();

    for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
        DeviceInfo info = it.value();
        QString s_disksize = Utils::format_size(info.length, info.sector_size);
        auto m_box = new DmDiskinfoBox(0, this, info.m_path, s_disksize);
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            QString s_pdisksize = Utils::format_size(it->sector_end - it->sector_start + 1, it->sector_size);
            QString s_partitionpath = it->path.remove(0, 5);
            QString s_unusedstr = Utils::format_size(it->sectors_used, it->sector_size);
            QString s_usedstr = Utils::format_size(it->sectors_unused, it->sector_size);
            qDebug() << s_pdisksize;
            qDebug() << it->partition_number;
            qDebug() << s_unusedstr << "unused";
            qDebug() << s_usedstr << "used";
            qDebug() << it->fstype;
            FSType fstype1 = static_cast<FSType>(it->fstype);
            QString s_fstype = Utils::FSTypeToString(fstype1);
            qDebug() << it->mountpoints;
            QString s_mountpoint;
            QString s_mountpoints;
            for (int o = 0; o < it->mountpoints.size(); o++) {
                s_mountpoint = it->mountpoints[o];
                s_mountpoints += it->mountpoints[o];
                qDebug() << it->mountpoints[o];
            }
            qDebug() << s_mountpoints;
            qDebug() << s_fstype;
            qDebug() << it->filesystem_label;
            QString s_filesystem_label = it->filesystem_label;
            auto m_childbox = new DmDiskinfoBox(1, this, it->device_path, "", s_partitionpath, s_pdisksize, s_usedstr, s_unusedstr, it->sectors_unallocated,
                                                it->sector_start, it->sector_end, s_fstype, s_mountpoints, s_filesystem_label, it->flag);
            m_box->childs.append(m_childbox);
        }

        m_treeView->addTopItem(m_box, m_additem);
    }

    QStringList deviceNameList = DMDbusHandler::instance()->getDeviceNameList();
    if (deviceNameList.indexOf(DMDbusHandler::instance()->getCurPartititonInfo().device_path) == -1) {
        m_flag = 0;
    } else {
        m_deviceNum = deviceNameList.indexOf(DMDbusHandler::instance()->getCurPartititonInfo().device_path);
    }

    m_additem = 1;
    if (m_flag == 0) {
        m_treeView->setDefaultdmItem();
    } else {
        m_treeView->setRefreshItem(m_deviceNum, m_treeView->currentNum());
    }
    m_flag += 1;
    qDebug() << m_flag << m_treeView->currentNum();
}
