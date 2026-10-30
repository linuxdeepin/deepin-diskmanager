// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "createpartitiontabledialog.h"

#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QDir>
#include <QVBoxLayout>

CreatePartitionTableDialog::CreatePartitionTableDialog(QWidget *parent)
    : DDBase(parent)
{
    qCDebug(appLog) << "CreatePartitionTableDialog initializing...";
    initUi();
    initConnection();
    qCDebug(appLog) << "CreatePartitionTableDialog initialized";
}

void CreatePartitionTableDialog::initUi()
{
    qCDebug(appLog) << "CreatePartitionTableDialog::initUi called.";
    m_ComboBox = new DComboBox;
    m_ComboBox->setObjectName("CreatePartitionTableDialogComboBox");
    m_ComboBox->addItem("GPT");
    m_ComboBox->addItem("MSDOS");
    m_ComboBox->setAccessibleName("choosePartitionTable");
    qCDebug(appLog) << "Partition table combo box initialized.";

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->addWidget(m_ComboBox);

    int index = addButton(tr("Cancel"), true, ButtonNormal);
    getButton(index)->setAccessibleName("cancel");
    qCDebug(appLog) << "Cancel button added.";

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    qCDebug(appLog) << "Current device info - path:" << info.m_path << ", type:" << info.m_disktype;
    if (info.m_disktype == "unrecognized") {
        qCDebug(appLog) << "Disk type is unrecognized, setting type to 'create'.";
        m_curType = "create";
        // 当前磁盘无分区表，是否新建分区表?
        setTitle(tr("No partition table in this disk. Create a new one?"));
        m_okCode = addButton(tr("Create"), false, ButtonRecommend);
        getButton(m_okCode)->setAccessibleName("create");
        qCDebug(appLog) << "Dialog title and create button set for new partition table.";
    } else {
        qCDebug(appLog) << "Disk type is recognized, setting type to 'replace'.";
        m_curType = "replace";
        // 当前磁盘已有分区表，是否替换此分区表?
        setTitle(tr("The disk has a partition table already. Replace it?"));
        m_okCode = addButton(tr("Replace"), false, ButtonRecommend);
        getButton(m_okCode)->setAccessibleName("replace");
        qCDebug(appLog) << "Dialog title and replace button set for existing partition table.";
    }
    qCDebug(appLog) << "UI initialization finished.";
}

void CreatePartitionTableDialog::initConnection()
{
    qCDebug(appLog) << "CreatePartitionTableDialog::initConnection called.";
    connect(this, &CreatePartitionTableDialog::buttonClicked, this, &CreatePartitionTableDialog::onButtonClicked);
    qCDebug(appLog) << "Button clicked signal connected to onButtonClicked slot.";
}

void CreatePartitionTableDialog::onButtonClicked(int index, const QString &text)
{
    qCDebug(appLog) << "CreatePartitionTableDialog::onButtonClicked called. Index:" << index << ", Text:" << text;
    Q_UNUSED(text);
    if (index == m_okCode) {
        qCDebug(appLog) << "User confirmed partition table operation, type:" << m_curType;
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        DMDbusHandler::instance()->createPartitionTable(info.m_path, QString("%1").arg(info.m_length), QString("%1").arg(info.m_sectorSize), m_ComboBox->currentText(), m_curType);
        close();
        qCDebug(appLog) << "Partition table creation initiated and dialog closed.";
    } else {
        qCDebug(appLog) << "User canceled partition table operation, closing dialog.";
        close();
    }
}

