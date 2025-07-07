// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "selecteditemwidget.h"
#include "partedproxy/dmdbushandler.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QApplication>

SelectedItemWidget::SelectedItemWidget(PVInfoData pvInfoData, QWidget *parent)
    : RadiusWidget(parent)
    , m_pvInfoData(pvInfoData)
{
    qDebug() << "Creating selected item for:"
                             << (m_pvInfoData.m_disktype == "unrecognized" ? m_pvInfoData.m_diskPath : m_pvInfoData.m_partitionPath);
    initUi();
    initConnection();
}

void SelectedItemWidget::initUi()
{
    qDebug() << "SelectedItemWidget::initUi called.";
    setFixedHeight(36);

    DPalette paletteName;
    paletteName.setColor(DPalette::TextTips, QColor("#2C4767"));

    DPalette paletteSize;
    paletteName.setColor(DPalette::TextTips, QColor("#526A7F"));

    QFont fontName;
    fontName.setWeight(QFont::Medium);
    fontName.setFamily("Source Han Sans");
    fontName.setPixelSize(14);

    QFont fontSize;
    fontSize.setWeight(QFont::Normal);
    fontSize.setFamily("Source Han Sans");
    fontSize.setPixelSize(12);

    m_nameInfo = new DLabel(this);
    m_nameInfo->setText("/dev/sda1");
    m_nameInfo->setFont(fontName);
    m_nameInfo->setPalette(paletteName);
    m_nameInfo->setAlignment(Qt::AlignVCenter);

    m_sizeInfo = new DLabel(this);
    m_sizeInfo->setText("1GB");
    m_sizeInfo->setFont(fontSize);
    m_sizeInfo->setPalette(paletteSize);
    m_sizeInfo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

//    m_buttonLabel = new ButtonLabel(this);
//    m_buttonLabel->setPixmap(DStyle::standardIcon(QApplication::style(), DStyle::SP_DeleteButton).pixmap(QSize(17, 17)));

    m_iconButton = new DIconButton(this);
    m_iconButton->setIcon(DStyle::standardIcon(QApplication::style(), DStyle::SP_DeleteButton));
    m_iconButton->setIconSize(QSize(17, 17));
    m_iconButton->setFixedSize(17, 17);

    if (m_pvInfoData.m_disktype == "unrecognized") {
        qDebug() << "Setting unrecognized disk info:" << m_pvInfoData.m_diskPath << m_pvInfoData.m_diskSize;
        m_nameInfo->setText(m_pvInfoData.m_diskPath);
        m_sizeInfo->setText(m_pvInfoData.m_diskSize);
    } else {
        if (m_pvInfoData.m_partitionPath == "unallocated") {
            qDebug() << "Setting unallocated partition info:" << m_pvInfoData.m_diskPath;
            m_nameInfo->setText(m_pvInfoData.m_diskPath);
        } else {
            qDebug() << "Setting partition info:" << m_pvInfoData.m_partitionPath << m_pvInfoData.m_partitionSize;
            m_nameInfo->setText(m_pvInfoData.m_partitionPath);
        }

        m_sizeInfo->setText(m_pvInfoData.m_partitionSize);
        qDebug() << "Setting partition size:" << m_pvInfoData.m_partitionSize;
    }

    if (m_pvInfoData.m_isReadOnly) {
        qDebug() << "Disk is read-only, disabling icon button.";
        m_iconButton->setDisabled(true);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_nameInfo);
    mainLayout->addStretch();
    mainLayout->addWidget(m_sizeInfo);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(m_iconButton);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(10, 0, 10, 0);
    qDebug() << "Main layout configured.";

    setLayout(mainLayout);
    qDebug() << "Layout set.";
}

void SelectedItemWidget::initConnection()
{
    qDebug() << "SelectedItemWidget::initConnection called.";
    connect(m_iconButton, &DIconButton::clicked, this, &SelectedItemWidget::onDeleteClicked);
}

void SelectedItemWidget::onDeleteClicked()
{
    qDebug() << "SelectedItemWidget::onDeleteClicked called.";
    qDebug() << "Delete clicked for:"
                           << (m_pvInfoData.m_disktype == "unrecognized" ? m_pvInfoData.m_diskPath : m_pvInfoData.m_partitionPath);
    emit deleteItem(m_pvInfoData);
}


