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

#include <DPalette>
#include <DMenu>
#include <DMessageManager>

#include <QVBoxLayout>
#include <QDebug>

DeviceListWidget::DeviceListWidget(QWidget *parent)
    : DWidget(parent)
{
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
    this->setLayout(layout);
    m_treeview = new DmTreeview(this);
    m_treeview->setContextMenuPolicy(Qt::CustomContextMenu);
    layout->addWidget(m_treeview);
}

void DeviceListWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigUpdateDeviceInfo, this, &DeviceListWidget::slotUpdateDeviceInfo);
    connect(m_treeview, &DmTreeview::sigCurSelectChanged, DMDbusHandler::instance(), &DMDbusHandler::slotsetCurSelect);  
    connect(m_treeview, &QTreeView::customContextMenuRequested, this, &DeviceListWidget::treeMenu);
}

void DeviceListWidget::treeMenu(const QPoint &pos)
{
    QModelIndex curIndex = m_treeview->indexAt(pos);      //当前点击的元素的index
    QModelIndex index = curIndex.sibling(curIndex.row(),0); //该行的第1列元素的index
    qDebug() << curIndex << "--------" << index << "---------------" << m_treeview->getRootItem()->index()
             << m_treeview->getcuritem()->data();
    DiskInfoData data = m_treeview->getcuritem()->data().value<DiskInfoData>();
    qDebug() << data.diskpath << data.disksize << data.partitionsize << data.partitonpath << data.level << data.used << data.unused << data.start
             << data.end << "--------" << data.fstype << "------" << data.mountpoints << data.syslabel << data.iconImage;
    if (index.isValid()) {
        m_curDiskInfoData = m_treeview->getcuritem()->data().value<DiskInfoData>();
        if (data.level == 0) {
            QMenu *menu = new QMenu();

            QAction *actionInfo = new QAction();
            actionInfo->setText(tr("Disk info")); // 磁盘信息
            actionInfo->setObjectName(tr("Disk info"));
            menu->addAction(actionInfo);
            connect(actionInfo, &QAction::triggered, this, &DeviceListWidget::onTreeMenuClicked);
//            connect(actionInfo, &QAction::triggered, this, [=] (QVariant value) {
//                    if (!value.isNull()) {
//                        DiskInfoDisplayDialog *diskInfoDisplayDialog = new DiskInfoDisplayDialog(data.diskpath);
//                        diskInfoDisplayDialog->exec();
//                    }
//                });

    //        menu->addSeparator();    //添加一个分隔线
            QMenu *itemChildMenu = new QMenu(QStringLiteral("Health management")); // 健康管理
            menu->addMenu(itemChildMenu);

            QAction *actionHealthDetection = new QAction();
            actionHealthDetection->setText(tr("Check health")); // 硬盘健康检测
            actionHealthDetection->setObjectName("Check health");
            itemChildMenu->addAction(actionHealthDetection);
            connect(actionHealthDetection, &QAction::triggered, this, &DeviceListWidget::onTreeMenuClicked);
//            connect(actionHealthDetection, &QAction::triggered, this, [=] (QVariant value) {
//                    if (!value.isNull()) {
//                        DiskHealthDetectionDialog *diskHealthDetectionDialog = new DiskHealthDetectionDialog(data.diskpath);
//                        diskHealthDetectionDialog->exec();
//                    }
//                });

            QAction *actionCheckError = new QAction();
            actionCheckError->setText(tr("Check partition table error")); // 分区表错误检测
            actionCheckError->setObjectName("Check partition table error");
            itemChildMenu->addAction(actionCheckError);

            QAction *actionVerifyRepair = new QAction();
            actionVerifyRepair->setText(tr("Verify or repair bad sectors")); // 坏道检测与修复
            actionVerifyRepair->setObjectName("Verify or repair bad sectors");
            itemChildMenu->addAction(actionVerifyRepair);

            menu->exec(QCursor::pos());  //显示菜单
        } else {
            QMenu *menu = new QMenu();

            QAction *actionHidePartition = new QAction();
            actionHidePartition->setText(tr("Hide partition")); // 隐藏分区
            actionHidePartition->setObjectName("Hide partition");
            menu->addAction(actionHidePartition);
            connect(actionHidePartition, &QAction::triggered, this, &DeviceListWidget::onTreeMenuClicked);

            QAction *actionShowPartition = new QAction();
            actionShowPartition->setText(tr("Unhide partition")); // 显示分区
            actionShowPartition->setObjectName("Unhide partition");
            menu->addAction(actionShowPartition);
            connect(actionShowPartition, &QAction::triggered, this, &DeviceListWidget::onTreeMenuClicked);
//            actionShowPartition->setDisabled(true); // 将按钮置为不可选

            QAction *actionDelete = new QAction();
            actionDelete->setText(tr("Delete partition")); // 删除分区
            actionDelete->setObjectName("Delete partition");
            menu->addAction(actionDelete);
            connect(actionDelete, &QAction::triggered, this, &DeviceListWidget::onTreeMenuClicked);

            if (m_curDiskInfoData.fstype == "unallocated") {
                actionHidePartition->setDisabled(true);
                actionShowPartition->setDisabled(true);
                actionDelete->setDisabled(true);
            } else {
                int result = DMDbusHandler::instance()->getPartitionHiddenFlag(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);
                if (1 == result) {
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
}

void DeviceListWidget::onTreeMenuClicked()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action->objectName() == "Disk info") {
        DiskInfoDisplayDialog *diskInfoDisplayDialog = new DiskInfoDisplayDialog(m_curDiskInfoData.diskpath);
        diskInfoDisplayDialog->exec();
    } else if (action->objectName() == "Check health") {
        DiskHealthDetectionDialog *diskHealthDetectionDialog = new DiskHealthDetectionDialog(m_curDiskInfoData.diskpath);
        diskHealthDetectionDialog->exec();
    } else if (action->objectName() == "Hide partition") {
        MessageBox messageBox;
        // 您是否要隐藏该分区？ 隐藏  取消
        messageBox.setWarings(tr("Do you want to hide this partition?"), "", tr("Hide"), tr("Cancel"));
        if (messageBox.exec() == 1) {
            if (m_curDiskInfoData.mountpoints.isEmpty()) {
                bool result = DMDbusHandler::instance()->hidePartition(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);

                if (result) {
                    // 隐藏分区成功
                    DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Hide the partition successfully"));
                    DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
                } else {
                    // 隐藏分区失败
                    DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Failed to Hide the partition"));
                    DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
                }

            } else {
                DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
                floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
                floMsg->setMessage(tr("Hide failed: the partition is mounted")); // 隐藏分区失败！该分区未卸载
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), floMsg);
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            }
        }
    } else if (action->objectName() == "Unhide partition") {
        MessageBox messageBox;
        // 您是否要显示该隐藏分区？ 显示  取消
        messageBox.setWarings(tr("Do you want to unhide this partition?"), "", tr("Unhide"), tr("Cancel"));
        if (messageBox.exec() == 1) {
            bool result = DMDbusHandler::instance()->unhidePartition(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);

            if (result) {
                // 显示分区成功
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Unhide the partition successfully"));
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            } else {
                // 显示分区失败
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Failed to unhide the partition"));
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            }
        }
    } else if (action->objectName() == "Delete partition") {
        MessageBox messageBox;
        // 您确定要删除该分区吗？ 该分区内所有文件将会丢失  删除  取消
        messageBox.setWarings(tr("Are you sure you want to delete this partition?"), tr("You will lose all data in it"), tr("Delete"), DDialog::ButtonWarning, tr("Cancel"));
        if (messageBox.exec() == 1) {

            bool result = DMDbusHandler::instance()->deletePartition(m_curDiskInfoData.diskpath, m_curDiskInfoData.partitonpath);

            if (result) {
                // 删除分区成功
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Delete the partition successfully"));
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));


            } else {
                // 删除分区失败
                DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget(), QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Failed to delete the partition"));
                DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget(), QMargins(0, 0, 0, 20));
            }

            DMDbusHandler::instance()->getDeviceinfo();
        }
    }
}

void DeviceListWidget::slotUpdateDeviceInfo()
{
    qDebug() << __FUNCTION__ << "               1";
    //更新DmTreeview  lx
    //设置当前选项
    auto handler = DMDbusHandler::instance();
    num = handler->getCurPartititonInfo().partition_number;
    devicenum_ = m_treeview->currentTopNum();
    qDebug() << flag << m_treeview->currentTopNum();
    devicepath_ = handler->getCurPartititonInfo().device_path;
    m_treeview->m_model->clear();
    DeviceInfoMap infomap = DMDbusHandler::instance()->probDeviceInfo();

    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
        QString s_disksize = Utils::format_size(info.length, info.sector_size);
        auto m_box = new DmDiskinfoBox(0, this, info.m_path, s_disksize);
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            QString s_pdisksize = Utils::format_size(it->sector_end - it->sector_start, it->sector_size);
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
                                                it->sector_start, it->sector_end, s_fstype, s_mountpoints, s_filesystem_label);
            m_box->childs.append(m_childbox);
        }

        m_treeview->addTopItem(m_box, additem);
    }
    additem = 1;
    if (flag == 0) {
        m_treeview->setDefaultdmItem();
    } else {
        m_treeview->setRefreshItem(devicenum_, m_treeview->currentNum());
    }
    flag += 1;
    qDebug() << flag << m_treeview->currentNum();
}
