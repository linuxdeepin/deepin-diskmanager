// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

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
    qDebug() << "CreatePartitionTableDialog initializing...";
    initUi();
    initConnection();
    qDebug() << "CreatePartitionTableDialog initialized";
}

void CreatePartitionTableDialog::initUi()
{
    qDebug() << "CreatePartitionTableDialog::initUi called.";
    m_ComboBox = new DComboBox;
    m_ComboBox->addItem("GPT");
    m_ComboBox->addItem("MSDOS");
    m_ComboBox->setAccessibleName("choosePartitionTable");
    qDebug() << "Partition table combo box initialized.";

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->addWidget(m_ComboBox);

    int index = addButton(tr("Cancel"), true, ButtonNormal);
    getButton(index)->setAccessibleName("cancel");
    qDebug() << "Cancel button added.";

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    qDebug() << "Current device info - path:" << info.m_path << ", type:" << info.m_disktype;
    if (info.m_disktype == "unrecognized") {
        qDebug() << "Disk type is unrecognized, setting type to 'create'.";
        m_curType = "create";
        // 当前磁盘无分区表，是否新建分区表?
        setTitle(tr("No partition table in this disk. Create a new one?"));
        m_okCode = addButton(tr("Create"), false, ButtonRecommend);
        getButton(m_okCode)->setAccessibleName("create");
        qDebug() << "Dialog title and create button set for new partition table.";
    } else {
        qDebug() << "Disk type is recognized, setting type to 'replace'.";
        m_curType = "replace";
        // 当前磁盘已有分区表，是否替换此分区表?
        setTitle(tr("The disk has a partition table already. Replace it?"));
        m_okCode = addButton(tr("Replace"), false, ButtonRecommend);
        getButton(m_okCode)->setAccessibleName("replace");
        qDebug() << "Dialog title and replace button set for existing partition table.";
    }
    qDebug() << "UI initialization finished.";
}

void CreatePartitionTableDialog::initConnection()
{
    qDebug() << "CreatePartitionTableDialog::initConnection called.";
    connect(this, &CreatePartitionTableDialog::buttonClicked, this, &CreatePartitionTableDialog::onButtonClicked);
    qDebug() << "Button clicked signal connected to onButtonClicked slot.";
}

void CreatePartitionTableDialog::onButtonClicked(int index, const QString &text)
{
    qDebug() << "CreatePartitionTableDialog::onButtonClicked called. Index:" << index << ", Text:" << text;
    Q_UNUSED(text);
    if (index == m_okCode) {
        qDebug() << "User confirmed partition table operation, type:" << m_curType;
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        DMDbusHandler::instance()->createPartitionTable(info.m_path, QString("%1").arg(info.m_length), QString("%1").arg(info.m_sectorSize), m_ComboBox->currentText(), m_curType);
        close();
        qDebug() << "Partition table creation initiated and dialog closed.";
    } else {
        qDebug() << "User canceled partition table operation, closing dialog.";
        close();
    }
}

