/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskinfodisplaydialog.cpp
 *
 * @brief 磁盘信息类
 *
 * @date 2020-08-19 17:01
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
#include "diskinfodisplaydialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"
#include "common.h"

#include <DLabel>
#include <DFrame>
#include <DMessageManager>
#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

DiskInfoDisplayDialog::DiskInfoDisplayDialog(const QString &devicePath, QWidget *parent)
    : DDialog(parent)
    , m_devicePath(devicePath)
{
    initUI();
    initConnections();
}

void DiskInfoDisplayDialog::initUI()
{
//    setWindowTitle("磁盘信息展示");
    setIcon(QIcon::fromTheme(appName));
    setTitle(tr("Disk Info")); // 磁盘信息
    setMinimumSize(500, 474);

    DFrame *infoWidget = new DFrame;
    infoWidget->setBackgroundRole(DPalette::ItemBackground);
    infoWidget->setFixedSize(480, 414);

    HardDiskInfo hardDiskInfo = DMDbusHandler::instance()->getHardDiskInfo(m_devicePath);

//    m_diskInfoNameList << tr("序列号：") << tr("设备模型：") << tr("LU WWN 设备Id：") << tr("固件版本：") << tr("用户容量：")
//                       << tr("扇区大小：") << tr("转速：") << tr("外形系数：") << tr("设备：") << tr("ATA版本为：")
//                       << tr("SATA版本为：") << tr("当地时间为：") << tr("智能支持：");

    m_diskInfoNameList.clear();
    m_diskInfoNameList << tr("Serial Number:") << tr("Device Model:") << tr("LU WWN Device Id:") << tr("Firmware Version:")
                       << tr("User Capacity:") << tr("Sector Size:") << tr("Rotation Rate:") << tr("Form Factor:")
                       << tr("Device is:") << tr("ATA Version is:") << tr("SATA Version is:")
                       << tr("Local Time is:") << tr("SMART support is:");

    QString deviceis = hardDiskInfo.m_deviceis;
    QStringList deviceNameList = deviceis.split("[");

    m_diskInfoValueList.clear();
    m_diskInfoValueList << tr("%1").arg(hardDiskInfo.m_serialNumber) << tr("%1").arg(hardDiskInfo.m_deviceModel)
                        << tr("%1").arg(hardDiskInfo.m_deviceId) << tr("%1").arg(hardDiskInfo.m_firmwareVersion)
                        << tr("%1").arg(hardDiskInfo.m_userCapacity) << tr("%1").arg(hardDiskInfo.m_sectorSize)
                        << tr("%1").arg(hardDiskInfo.m_rotationRate) << tr("%1").arg(hardDiskInfo.m_formFactor)
                        << tr("%1").arg(deviceNameList.at(0)) << tr("%1").arg(hardDiskInfo.m_ataVersionIs)
                        << tr("%1").arg(hardDiskInfo.m_sataVersionIs) << tr("%1").arg(hardDiskInfo.m_localTime)
                        << tr("%1").arg(hardDiskInfo.m_smartSupport);

    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);

    DPalette palette1;
    QColor color1("#000000");
    color1.setAlphaF(0.85);
    palette1.setColor(DPalette::WindowText, color1);

    DPalette palette2;
    QColor color2("#000000");
    color2.setAlphaF(0.7);
    palette2.setColor(DPalette::WindowText, color2);

    for (int i = 0; i < m_diskInfoNameList.count(); i++) {
        DLabel *nameLabel = new DLabel;
        nameLabel->setText(m_diskInfoNameList.at(i));
        DFontSizeManager::instance()->bind(nameLabel, DFontSizeManager::T7, QFont::Medium);
        nameLabel->setFixedWidth(160);
        nameLabel->setPalette(palette1);

        DLabel *valueLabel = new DLabel;
        valueLabel->setText(m_diskInfoValueList.at(i));
        DFontSizeManager::instance()->bind(valueLabel, DFontSizeManager::T7, QFont::Normal);
        valueLabel->setPalette(palette2);

        QHBoxLayout *labelLayout = new QHBoxLayout;
        labelLayout->addWidget(nameLabel);
        labelLayout->addWidget(valueLabel);
        labelLayout->setSpacing(0);
        labelLayout->setContentsMargins(0, 0, 0, 0);

        infoLayout->addLayout(labelLayout);
    }

    m_linkButton = new DCommandLinkButton(tr("Export")); // 导出

    infoLayout->addWidget(m_linkButton);
    infoLayout->setContentsMargins(10, 10, 10, 10);

    addSpacing(10);
    addContent(infoWidget);
}

void DiskInfoDisplayDialog::initConnections()
{
    connect(m_linkButton, &DCommandLinkButton::clicked, this, &DiskInfoDisplayDialog::onExportButtonClicked);
}

void DiskInfoDisplayDialog::onExportButtonClicked()
{
    //文件保存路径
    QString fileDirPath = QFileDialog::getSaveFileName(this, tr("Save File"), tr("DiskInfo.txt"), tr("Text files (*.txt)"));// 文件保存   磁盘信息   文件类型
    if (fileDirPath.isEmpty()) {
        return;
    }

    QString strFileName = "/" + fileDirPath.split("/").last();
    QString fileDir = fileDirPath.split(strFileName).first();

    QFileInfo fileInfo;
    fileInfo.setFile(fileDir);
    QDir dir(fileDir);

    if (!dir.exists()) {
        DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
        floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
        floMsg->setMessage(tr("Wrong path")); // 路径错误
        DMessageManager::instance()->sendMessage(this, floMsg);
        DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));

        return;
    }

    if (!fileInfo.isWritable()) {
        DFloatingMessage *floMsg = new DFloatingMessage(DFloatingMessage::ResidentType);
        floMsg->setIcon(QIcon::fromTheme("://icons/deepin/builtin/warning.svg"));
        floMsg->setMessage(tr("You do not have permission to access this path")); // 您无权访问该路径
        DMessageManager::instance()->sendMessage(this, floMsg);
        DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));
    } else {
        if (!fileDirPath.contains(".txt")) {
            fileDirPath = fileDirPath + ".txt";
        }

        QFile file(fileDirPath);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            QTextStream out(&file);
            for (int i = 0; i < m_diskInfoNameList.count(); i++) {
                QString strInfo = m_diskInfoNameList.at(i) + m_diskInfoValueList.at(i) + "\n";
                out << strInfo;
                out.flush();
            }

            file.close();

            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Export successful")); // 导出成功
            DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));
        } else {
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Export failed")); // 导出失败
            DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));
        }
    }
}


