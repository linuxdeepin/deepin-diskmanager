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
#include "common.h"

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

    m_btnParted->setIconSize(QSize(16, 16));
    m_btnFormat->setIconSize(QSize(16, 16));
    m_btnMount->setIconSize(QSize(14, 17));
    m_btnUnmount->setIconSize(QSize(14, 15));
    m_btnResize->setIconSize(QSize(16, 16));

    m_btnParted->setDisabled(true);
    m_btnFormat->setDisabled(true);
    m_btnMount->setDisabled(true);
    m_btnUnmount->setDisabled(true);
    m_btnResize->setDisabled(true);

    layout->addWidget(m_btnParted);
    layout->addWidget(m_btnFormat);
    layout->addWidget(m_btnMount);
    layout->addWidget(m_btnUnmount);
    layout->addWidget(m_btnResize);
}

void TitleWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::curSelectChanged, this, &TitleWidget::onCurSelectChanged);
    connect(m_btnParted, &DPushButton::clicked, this, &TitleWidget::showPartInfoWidget);
    connect(m_btnFormat, &DPushButton::clicked, this, &TitleWidget::showFormateInfoWidget);
    connect(m_btnMount, &DPushButton::clicked, this, &TitleWidget::showMountInfoWidget);
    connect(m_btnUnmount, &DPushButton::clicked, this, &TitleWidget::showUnmountInfoWidget);
    connect(m_btnResize, &DPushButton::clicked, this, &TitleWidget::showResizeInfoWidget);
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
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setCurDevicePath(info.m_devicePath);

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
    if (DMDbusHandler::instance()->getCurLevel() == 1) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
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
    } else {
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
                widget->objectName() == "formateDialog" || widget->objectName() == "mountDialog" ||
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

