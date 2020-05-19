/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     zhangkai <zhangkai@uniontech.com>
* Maintainer: zhangkai <zhangkai@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "partitionwidget.h"
#include <QDebug>

PartitionWidget::PartitionWidget(QWidget *parent) : DDialog(parent)
{
    initUi();
    initConnection();
}

void PartitionWidget::initUi()
{
    this->setModal(true);
    this->setFixedSize(800, 600);
    mainFrame = new QWidget(this);

//    mainFrame->setAttribute(Qt::WA_TranslucentBackground, true);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setSpacing(5);
    titleLabel = new DLabel(tr("Will XXX system space be partitioned?"), mainFrame);
    QFont font;
    font.setBold(true);
    font.setPointSize(11);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignCenter);
    tipLabel = new DLabel(tr("Click the '+' button to increase the number of partitions on the device,and click each partition to change the name formate"), mainFrame);
    tipLabel->setAlignment(Qt::AlignCenter);
    topFrame = new DFrame(mainFrame);
    topFrameSetting();
    initTopFrameData();
    topFrame->setFrameRounded(true);

    DPalette pa = DApplicationHelper::instance()->palette(topFrame);
    pa.setBrush(DPalette::Base, pa.itemBackground());
    DApplicationHelper::instance()->setPalette(topFrame, pa);
    topFrame->setAutoFillBackground(true);

    midFrame = new DFrame(mainFrame);
    midFrameSetting();
    midFrame->setFrameStyle(DFrame::NoFrame);

    botFrame = new DFrame(mainFrame);
    botFrameSetting();
    botFrame->setFrameStyle(DFrame::NoFrame);

    mainLayout->addWidget(titleLabel, 1);
    mainLayout->addWidget(tipLabel, 1);
    mainLayout->addWidget(topFrame, 1);
    mainLayout->addWidget(midFrame, 3);
    mainLayout->addWidget(botFrame, 10);

    this->addContent(mainFrame);
}

void PartitionWidget::topFrameSetting()
{

    picLabel = new DLabel(topFrame);
    picLabel->setMaximumSize(60, 70);
    picLabel->setStyleSheet("border:1px solid gray");


    QHBoxLayout *hLayout = new QHBoxLayout(topFrame);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(30, 0, 0, 0);

    QVBoxLayout *vLayout = new QVBoxLayout(topFrame);
    vLayout->setContentsMargins(10, 20, 0, 20);
    QFont font;
    font.setPointSize(11);
    deviceInfoLabel = new DLabel(tr("Device Information"), topFrame);
    deviceInfoLabel->setFont(font);
    QHBoxLayout *line1Layout = new QHBoxLayout();

    line1Layout->addWidget(deviceInfoLabel);
    line1Layout->addStretch();
//
    QHBoxLayout *line4Layout = new QHBoxLayout();
    allMemoryLabel  = new DLabel(tr("Total Capacity:"), topFrame);
    allMemory = new DLabel("256GB", topFrame);
    selectedPartLabel = new DLabel(tr("Selected Partition:"), topFrame);
    selectedPartition = new DLabel("sda3");
    line4Layout->addWidget(allMemoryLabel);
    line4Layout->addWidget(allMemory);
    allMemory->setMinimumWidth(70);
    line4Layout->addWidget(selectedPartLabel);
    line4Layout->addWidget(selectedPartition);
    line4Layout->setContentsMargins(0, 0, 360, 0);
//    line4Layout->addStretch();

    QHBoxLayout *line2Layout = new QHBoxLayout();
    deviceNameLabel = new DLabel(tr("Device:"), topFrame);
    deviceNameLabel->setMaximumWidth(40);
    deviceName = new DLabel("/dev/sda", topFrame);
    deviceName->setMinimumWidth(115);
    deviceName->setAlignment(Qt::AlignLeft);
    deviceFormateLabel = new DLabel(tr("Formate:"), topFrame);
    deviceFormateLabel->setMaximumWidth(50);
    deviceFormate = new DLabel("EXT3", topFrame);
    line2Layout->addWidget(deviceNameLabel);
    line2Layout->addWidget(deviceName);
    line2Layout->addWidget(deviceFormateLabel);
    line2Layout->addWidget(deviceFormate);
//    line2Layout->addStretch();
    line2Layout->setContentsMargins(0, 0, 370, 0);

    vLayout->addLayout(line1Layout);
    vLayout->addLayout(line4Layout);
    vLayout->addLayout(line2Layout);
    hLayout->addWidget(picLabel, 1);
    hLayout->addLayout(vLayout, 10);
}

void PartitionWidget::midFrameSetting()
{
    midFrame->setMinimumHeight(100);
    QVBoxLayout *mainLayout = new QVBoxLayout(midFrame);
    partChartWidget = new PartChartShowing(midFrame);
    mainLayout->addWidget(partChartWidget);

}

void PartitionWidget::botFrameSetting()
{
    QVBoxLayout *vLayout = new QVBoxLayout(botFrame);

    partWidget = new QWidget(botFrame);
    //分区详细页
    partInfoShowing();
    vLayout->addWidget(partWidget, 5);
    //按钮
    applyBtn = new DPushButton(tr("Apply"), botFrame);
    //取消
    cancleBtn = new DPushButton(tr("Cancle"), botFrame);
    //复原
    reveBtn = new DPushButton(tr("Revert"), botFrame);


    applyBtn->setMinimumWidth(120);
    cancleBtn->setMinimumWidth(120);
    reveBtn->setMinimumWidth(120);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(reveBtn, 1, Qt::AlignLeft);
    btnLayout->addWidget(cancleBtn);
    btnLayout->addWidget(applyBtn);

    vLayout->addLayout(btnLayout, 1);


}

void PartitionWidget::partInfoShowing()
{
    QVBoxLayout *vLayout = new QVBoxLayout(partWidget);
    QHBoxLayout *line1Layout = new QHBoxLayout();
    QFont font1;
    font1.setPointSize(11);
    partInfoLabel = new DLabel(tr("Partition Information"), partWidget);
    partInfoLabel->setFont(font1);
    partInfoLabel->setMaximumSize(100, 50);

    line1Layout->addWidget(partInfoLabel);
    line1Layout->addStretch();

    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 10, 0, 10);
    QHBoxLayout *line2Layout1 = new QHBoxLayout();
    partDoLabel = new DLabel(tr("Partition operation:"), partWidget);

    addButton = new DIconButton(DStyle::SP_IncreaseElement);
    remButton = new DIconButton(DStyle::SP_DecreaseElement);
    partInfoLabel->setMaximumWidth(200);
    line2Layout1->addWidget(partDoLabel, 1, Qt::AlignLeft);
    line2Layout1->addWidget(addButton);
    line2Layout1->addWidget(remButton);

    QHBoxLayout *line2Layout2 = new QHBoxLayout();
    line2Layout2->setContentsMargins(50, 0, 0, 0);
    partNameLabel = new DLabel(tr("Partition name:"), partWidget);
    partNameEdit = new DLineEdit(partWidget);
    partNameEdit->setMinimumWidth(250);
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->lineEdit()->setPlaceholderText("SSD256G");
    }
    partNameLabel->setMaximumHeight(20);
    line2Layout2->addWidget(partNameLabel);
    line2Layout2->addWidget(partNameEdit);
    line2Layout->addLayout(line2Layout1);
    line2Layout->addLayout(line2Layout2);

    QHBoxLayout *line3Layout = new QHBoxLayout();
    QHBoxLayout *line3Layout1 = new QHBoxLayout();
    partFormateLabel = new DLabel(tr("Partiton formate:"), partWidget);
    partFormateCombox = new DComboBox(partWidget);
    partFormateCombox->addItem("APFS");
    line3Layout1->addWidget(partFormateLabel, 1);
    line3Layout1->addWidget(partFormateCombox, 3);
    QHBoxLayout *line3Layout2 = new QHBoxLayout();
    line3Layout2->setContentsMargins(50, 0, 0, 0);
    partSizeLabel = new DLabel(tr("Partition size:"), partWidget);
    hSlider = new DSlider(Qt::Horizontal);
    partSizeEdit = new DLineEdit(partWidget);
    partSizeEdit->setObjectName("sizeEdit");
    if (partSizeEdit->text().isEmpty()) {
        partSizeEdit->lineEdit()->setPlaceholderText("125");
    }
    partComCobox = new DComboBox(partWidget);
    partComCobox->addItem("GB");
    partComCobox->addItem("MB");
    line3Layout2->addWidget(partSizeLabel, 1);
    line3Layout2->addWidget(hSlider, 2);
    line3Layout2->addWidget(partSizeEdit, 3);
    line3Layout2->addWidget(partComCobox, 2);

    line3Layout->addLayout(line3Layout1);
    line3Layout->addLayout(line3Layout2);
//    line3Layout->addLayout(line)
    vLayout->addLayout(line1Layout);
    vLayout->addLayout(line2Layout);
    vLayout->addLayout(line3Layout);
    vLayout->addStretch();
}





void PartitionWidget::partedInfo()
{



}

void PartitionWidget::getPartitionInfo(const PartitionInfo &data, const QString &disksize)
{
    QString s_pdisksize = QString::number(Utils::sector_to_unit(data.sector_end - data.sector_start, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
    QString s_used = QString::number(Utils::sector_to_unit(data.sectors_used, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
    QString s_unused = QString::number(Utils::sector_to_unit(data.sectors_unused, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
    qDebug() << data.device_path << disksize << data.path << s_pdisksize << Utils::FSTypeToString((FSType)data.fstype) << s_used << s_unused;
    devicePath = data.device_path ;
    deviceSize = disksize;
    partPath = data.path;
    partSize = s_pdisksize;
    partFstype = Utils::FSTypeToString((FSType)data.fstype);
    partUsed = s_used;
    partUnused = s_unused;
    initTopFrameData();
}

void PartitionWidget::initTopFrameData()
{
    allMemory->setText(deviceSize);
    int i = partPath.lastIndexOf("/");
    QString selectPartition = partPath.right(partPath.length() - i - 1);
    selectedPartition->setText(selectPartition);
    deviceName->setText(devicePath);
    deviceFormate->setText(partFstype);
    qDebug() << devicePath << selectPartition;
}

void PartitionWidget::initConnection()
{
    connect(hSlider, &DSlider::valueChanged, this, &PartitionWidget::slotSliderValueChanged);
    connect(partSizeEdit, &DLineEdit::textEdited, this, &PartitionWidget::slotSetSliderValue);
    connect(addButton, &DIconButton::clicked, this, &PartitionWidget::addPartitionSlot);
    connect(remButton, &DIconButton::clicked, this, &PartitionWidget::remPartitionSlot);
}

void PartitionWidget::paintEvent(QPaintEvent *event)
{
//       DApplicationHelper::instance()->setPalette(scroll, pa);
//       scroll->setAutoFillBackground(true);
    //    QWidget::paintEvent(event);
}

void PartitionWidget::slotSliderValueChanged(int value)
{
    QString strSize = QString("%1").arg(value);
    partSizeEdit->setText(strSize);
}

void PartitionWidget::slotSetSliderValue()
{
    QString value = partSizeEdit->text();
    hSlider->setValue(value.toInt());
}

void PartitionWidget::addPartitionSlot()
{
    partChartWidget->getData(1, partSize);
}

void PartitionWidget::remPartitionSlot()
{
    partChartWidget->getData(2, partSize);
}
