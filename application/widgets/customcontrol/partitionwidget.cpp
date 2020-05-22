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
#include "widgets/widgetdeclare.h"
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
    tipLabel = new DLabel(tr("Click the '+' button to increase the number of partitions on the device,\n and click each partition to change the name formate"), mainFrame);
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
    picLabel->setPixmap(getIcon("labeldisk").pixmap(85, 85));
    picLabel->setMinimumSize(85, 85);

    QHBoxLayout *hLayout = new QHBoxLayout(topFrame);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(30, 0, 0, 0);

    QVBoxLayout *vLayout = new QVBoxLayout();
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
//    allMemoryLabel->setMinimumWidth(100);
    allMemory = new DLabel("256GB", topFrame);
    allMemory->setMinimumWidth(90);
    allMemory->setAlignment(Qt::AlignLeft);
    selectedPartLabel = new DLabel(tr("Selected Partition:"), topFrame);
    selectedPartition = new DLabel("sda3");
    line4Layout->addWidget(allMemoryLabel);
    line4Layout->addWidget(allMemory);
    line4Layout->addWidget(selectedPartLabel);
    line4Layout->addWidget(selectedPartition);
    line4Layout->addStretch();

    QHBoxLayout *line2Layout = new QHBoxLayout();
    deviceNameLabel = new DLabel(tr("Device:"), topFrame);
//    deviceNameLabel->setMinimumWidth(80);
    deviceName = new DLabel("/dev/sda", topFrame);
    deviceName->setMinimumWidth(140);
    deviceName->setAlignment(Qt::AlignLeft);
    deviceFormateLabel = new DLabel(tr("Formate:"), topFrame);

    deviceFormate = new DLabel("EXT3", topFrame);
    line2Layout->addWidget(deviceNameLabel);
    line2Layout->addWidget(deviceName);
    line2Layout->addWidget(deviceFormateLabel);
    line2Layout->addWidget(deviceFormate);
    line2Layout->addStretch();

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
//    QHBoxLayout *line1Layout = new QHBoxLayout();

    partInfoLabel = new DLabel(tr("Partition Information"), partWidget);
    DFontSizeManager::instance()->bind(partInfoLabel, DFontSizeManager::T6);
//    partInfoLabel->setStyleSheet("border :1px solid red");
//    partInfoLabel->setFixedWidth(100);



    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 10, 0, 10);
    QHBoxLayout *line2Layout1 = new QHBoxLayout();
    partDoLabel = new DLabel(tr("Partition operation:"), partWidget);

    addButton = new DIconButton(DStyle::SP_IncreaseElement);
    remButton = new DIconButton(DStyle::SP_DecreaseElement);
//    partDoLabel->setFixedWidth(140);
    line2Layout1->addWidget(partDoLabel);
//    line2Layout1->addSpacing(115);
    line2Layout1->addStretch();
    line2Layout1->addWidget(addButton);
    line2Layout1->addWidget(remButton);
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);
    QHBoxLayout *line2Layout2 = new QHBoxLayout();
//
    partNameLabel = new DLabel(tr("Partition name:"), partWidget);
    partNameEdit = new DLineEdit(partWidget);
//    partNameEdit->setMinimumWidth(260);
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->lineEdit()->setPlaceholderText("SSD256G");
    }
//    partNameLabel->setMaximumHeight(20);
//    partNameLabel->setFixedWidth(100);
    line2Layout2->addWidget(partNameLabel);
    line2Layout2->addWidget(partNameEdit);
    line2Layout->addLayout(line2Layout1);
    line2Layout->addLayout(line2Layout2);
    line2Layout1->setContentsMargins(0, 0, 30, 0);
    QHBoxLayout *line3Layout = new QHBoxLayout();
    QHBoxLayout *line3Layout1 = new QHBoxLayout();
    partFormateLabel = new DLabel(tr("Partiton formate:"), partWidget);

    partFormateCombox = new DComboBox(partWidget);
    partFormateCombox->addItem("APFS");
    partFormateCombox->setFixedWidth(220);
    line3Layout1->addWidget(partFormateLabel, 1);
    line3Layout1->addWidget(partFormateCombox, 3);
    QHBoxLayout *line3Layout2 = new QHBoxLayout();
    line3Layout2->setContentsMargins(25, 0, 0, 0);
    partSizeLabel = new DLabel(tr("Partition size:"), partWidget);
    hSlider = new DSlider(Qt::Horizontal);
    partSizeEdit = new DLineEdit(partWidget);
    partSizeEdit->setObjectName("sizeEdit");
    if (partSizeEdit->text().isEmpty()) {
        partSizeEdit->lineEdit()->setPlaceholderText("size");
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
    vLayout->addWidget(partInfoLabel);
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
    qDebug() << Utils::sector_to_unit(data.sector_end - data.sector_start, data.sector_size, SIZE_UNIT::UNIT_GIB) << data.device_path << disksize << data.path << s_pdisksize << Utils::FSTypeToString((FSType)data.fstype) << s_used << s_unused;
    devicePath = data.device_path ;
    deviceSize = disksize;
    partPath = data.path;
    partSize = s_pdisksize;
    partFstype = Utils::FSTypeToString((FSType)data.fstype);
    partUsed = s_used;
    partUnused = s_unused;
    partComCobox->setCurrentText("GB");
    int j = partSize.lastIndexOf("G");
    total = partSize.left(j).toDouble();
    if (total < 1) {
        total = total * 1024;
        partComCobox->setCurrentText("MB");
    }
    hSlider->setMaximum(total);
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
    connect(applyBtn, &DPushButton::clicked, this, &PartitionWidget::applyBtnSlot);
    connect(reveBtn, &DPushButton::clicked, this, &PartitionWidget::revertBtnSlot);
    connect(cancleBtn, &DPushButton::clicked, this, &PartitionWidget::cancelBtnSlot);
    connect(partChartWidget, &PartChartShowing::sendFlag, this, &PartitionWidget::showSelectPathInfo);
}

double PartitionWidget::leaveSpace()
{
    double sum = 0.00;
    for (int j = 0; j < sizeInfo.count(); j++) {
        sum = sum + sizeInfo.at(j);
        if (sum > total)
            break;
    }
    return sum;
}

void PartitionWidget::showSelectPathInfo(const int &flag, const int &num)
{
    qDebug() << flag;
    if (flag == 1) {
        partNameEdit->setText(tr("Free Space"));
        partSizeEdit->setText(QString::number(total));
    }
    if (flag == 2) {
        partNameEdit->setText(partName.at(num));
        partSizeEdit->setText(QString::number(sizeInfo.at(num)));
    }
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

    if (partNameEdit->text().isEmpty()) {
        partNameEdit->showAlertMessage(tr("Partition name is not empty"));
    } else {
        partName.append(partNameEdit->text());
    }
    if (partSizeEdit->text().isEmpty()) {
        partSizeEdit->showAlertMessage(tr("Partition Size is not empty"));
    }
    if (partComCobox->currentText() == "GB") {
        if (partSizeEdit->text().toDouble() > total)
            return;
    }
    if (total < 1 && partComCobox->currentText() == "MB") {
        total = total * 1024;

    }
    double size = partSizeEdit->text().toDouble();
    if (size > (total - leaveSpace()))
        size = total - leaveSpace();
    if ((leaveSpace() < total) && (size > 0.00) && !partNameEdit->text().isEmpty()) {
        sizeInfo.append(size);
    }
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);

    QColor color("#70BEFF");
    QColor color1("#4293FF");
    QColor color2("#6751E4");
    QColor color3("#FA7404");
    QColor color4("#FFD027");
    QColor color5("#2CCBBE");
    basecolor = QVector<QColor> {color, color1, color2, color3, color4, color5};
    partChartWidget->getData(partSize, sizeInfo, partName, basecolor);


    hSlider->setMaximum(total - leaveSpace());
    qDebug() << total - leaveSpace() << leaveSpace() << total << size;
    partChartWidget->update();
    partSizeEdit->setText("");
    partNameEdit->setText("");
    hSlider->setValue(0);
}

void PartitionWidget::remPartitionSlot()
{
    if (sizeInfo.size() == 0)
        return;
    sizeInfo.removeAt(sizeInfo.size() - 1);
    partName.removeAt(partName.size() - 1);
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    partChartWidget->getData(partSize, sizeInfo, partName, basecolor);
    hSlider->setMaximum(total - leaveSpace());
    partChartWidget->update();
    hSlider->setValue(0);

}

void PartitionWidget::applyBtnSlot()
{
    sizeInfo.clear();
    partName.clear();
    partChartWidget->getData(partSize, sizeInfo, partName, basecolor);
    qDebug() << sizeInfo;
    partChartWidget->update();
    this->close();
}

void PartitionWidget::revertBtnSlot()
{
    sizeInfo.clear();
    partName.clear();
    hSlider->setValue(0);
    partChartWidget->getData(partSize, sizeInfo, partName, basecolor);
    hSlider->setMaximum(total);
    hSlider->setValue(0);
    partChartWidget->update();
}

void PartitionWidget::cancelBtnSlot()
{
    this->close();
}


