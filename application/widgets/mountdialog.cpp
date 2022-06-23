/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mountdialog.cpp
 *
 * @brief 实现分区挂载
 *
 * @date 2020-09-16 17:03
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
#include "mountdialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QDir>
#include <QVBoxLayout>

MountDialog::MountDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void MountDialog::initUi()
{
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        setTitle(tr("Mount %1").arg(info.m_path));
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        setTitle(tr("Mount %1").arg(lvInfo.m_lvPath));
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        setTitle(tr("Mount %1").arg(info.m_path));
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    DLabel *tipLabel = new DLabel(tr("Choose a mount point please"), this);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    DLabel *mountLabel = new DLabel(tr("Mount point:"));
    DFontSizeManager::instance()->bind(mountLabel, DFontSizeManager::T6);
    m_fileChooserEdit = new DFileChooserEdit(this);
    m_fileChooserEdit->setDirectoryUrl(QUrl("file:///mnt"));
    m_fileChooserEdit->setFileMode(QFileDialog::Directory);
    m_fileChooserEdit->fileDialog()->setOption(QFileDialog::ShowDirsOnly);
    m_fileChooserEdit->fileDialog()->setAcceptMode(QFileDialog::AcceptOpen);
    m_fileChooserEdit->setAccessibleName("mountPointComboBox");
    m_fileChooserEdit->setText("/mnt");

//    m_ComboBox = new DComboBox;
//    m_ComboBox->setEditable(true);
//    m_ComboBox->addItem("/mnt");
//    m_ComboBox->addItem("/boot");
//    m_ComboBox->addItem("/");
//    m_ComboBox->addItem("/tmp");
//    m_ComboBox->addItem("/var");
//    m_ComboBox->addItem("/srv");
//    m_ComboBox->addItem("/opt");
//    m_ComboBox->addItem("/usr");
//    m_ComboBox->addItem("/local");
//    m_ComboBox->addItem("/media");
//    m_ComboBox->setAccessibleName("mountPointComboBox");

    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(mountLabel);
    mainLayout->addWidget(m_fileChooserEdit);

    int index = addButton(tr("Cancel"), true, ButtonNormal);
    m_okCode = addButton(tr("Mount"), false, ButtonRecommend);
//    getButton(okcode)->setDisabled(true);
    setOnButtonClickedClose(false);

    getButton(index)->setAccessibleName("cancel");
    getButton(m_okCode)->setAccessibleName("mountButton");
}

void MountDialog::initConnection()
{
    connect(m_fileChooserEdit->lineEdit(), &QLineEdit::textChanged, this, &MountDialog::onEditContentChanged);
    connect(this, &MountDialog::buttonClicked, this, &MountDialog::onButtonClicked);
}

void MountDialog::onEditContentChanged(const QString &content)
{
    if (content.isEmpty()) {
        getButton(m_okCode)->setDisabled(true);
    } else {
        QDir dir(content);
        if (dir.exists()) {
            getButton(m_okCode)->setDisabled(false);
//            m_fileChooserEdit->setAlert(false);
        } else {
            getButton(m_okCode)->setDisabled(true);
//            m_fileChooserEdit->setAlert(true);
        }
    }
}

bool MountDialog::isExistMountPoint(const QString &mountPoint)
{
    // 判断是否有分区已经挂载在该路径
    DeviceInfoMap infoMap = DMDbusHandler::instance()->probDeviceInfo();
    for (auto devInfo = infoMap.begin(); devInfo != infoMap.end(); devInfo++) {
        DeviceInfo info = devInfo.value();
        for (auto it = info.m_partition.begin(); it != info.m_partition.end(); it++) {
            QString mountpoints;
            for (int i = 0; i < it->m_mountPoints.size(); i++) {
                mountpoints += it->m_mountPoints[i];
                if (it->m_mountPoints.size() > 1 && mountPoint == it->m_mountPoints[i]) {
                    return true;
                }
            }

            if (mountPoint == mountpoints) {
                return true;
            }
        }
    }

    // 判断是否有逻辑卷已经挂载在该路径
    LVMInfo lvmInfo = DMDbusHandler::instance()->probLVMInfo();
    QMap<QString, VGInfo> lstVGInfo = lvmInfo.m_vgInfo;
    for (auto vgInfo = lstVGInfo.begin(); vgInfo != lstVGInfo.end(); vgInfo++) {
        VGInfo vgInformation = vgInfo.value();
        for (int i = 0; i < vgInformation.m_lvlist.count(); i++) {
            LVInfo info = vgInformation.m_lvlist.at(i);
            QString mountpoints;
            for (int j = 0; j < info.m_mountPoints.size(); j++) {
                mountpoints += info.m_mountPoints[j];
                if (info.m_mountPoints.size() > 1 && mountPoint == info.m_mountPoints[j]) {
                    return true;
                }
            }

            if (mountPoint == mountpoints) {
                return true;
            }
        }
    }

    return false;
}

bool MountDialog::isSystemDirectory(const QString &directory)
{
    bool isSysDir = false;

    QStringList lst;
    lst << "/bin" << "/boot" << "/dev" << "/etc" << "/home" << "/root"
        << "/run" << "/sbin" << "/tmp" << "/usr" << "/var";

    if (-1 != lst.indexOf(directory)) {
        isSysDir = true;
    }

    qDebug() << __FUNCTION__ << directory << " " << lst.indexOf(directory) << " And " << isSysDir;
    return isSysDir;
}

void MountDialog::mountCurPath()
{
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            QVector<QString> mountPoints;
            mountPoints.append(m_fileChooserEdit->text());
            luksInfo.m_mapper.m_mountPoints = mountPoints;

            DMDbusHandler::instance()->cryptMount(luksInfo, info.m_path);
        } else {
            DMDbusHandler::instance()->mount(m_fileChooserEdit->text());
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        if (lvInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(lvInfo.m_lvPath);
            QVector<QString> mountPoints;
            mountPoints.append(m_fileChooserEdit->text());
            luksInfo.m_mapper.m_mountPoints = mountPoints;

            DMDbusHandler::instance()->cryptMount(luksInfo, lvInfo.m_lvName);
        } else {
            LVAction lvAction;
            lvAction.m_vgName = lvInfo.m_vgName;
            lvAction.m_lvName = lvInfo.m_lvName;
            lvAction.m_lvFs = lvInfo.m_lvFsType;
            lvAction.m_lvSize = lvInfo.m_lvSize;
            lvAction.m_lvByteSize = lvInfo.m_lvLECount * lvInfo.m_LESize;
            lvAction.m_lvAct = LVMAction::LVM_ACT_LV_MOUNT;
            lvAction.m_mountPoint = m_fileChooserEdit->text();

            DMDbusHandler::instance()->onMountLV(lvAction);
        }
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
            QVector<QString> mountPoints;
            mountPoints.append(m_fileChooserEdit->text());
            luksInfo.m_mapper.m_mountPoints = mountPoints;

            DMDbusHandler::instance()->cryptMount(luksInfo, info.m_path);
        }
    }
}

void MountDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == m_okCode) {
        QString mountPath = m_fileChooserEdit->text();
        if (mountPath.endsWith("/")) {
            mountPath.chop(1);
        }

        QDir dir(mountPath);
        if (!dir.isEmpty() || isSystemDirectory(mountPath) || isExistMountPoint(mountPath)) {
            MessageBox messageBox(this);
            messageBox.setObjectName("mountMessageBox");
            messageBox.setAccessibleName("messageBox");
            messageBox.setWarings(tr("The data under this mount point would be lost, please mount the directory to another location"), "",
                                  tr("Continue", "button"), "ok", tr("Cancel"), "cancelBtn");
            if (messageBox.exec() == 1) {
                mountCurPath();
                close();
            }
        } else {
            mountCurPath();
            close();
        }
    } else {
        close();
    }
}
