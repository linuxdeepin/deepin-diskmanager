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
QList<qreal> dataValue;
QList<QString> strName;
PartitionWidget::PartitionWidget(QWidget *parent) : DDialog(parent)
{

    dataValue << 150.00 << 70.00 << 36.00;
    strName << "150GB" << "70GB" << "36GB";
    this->setModal(true);
    this->setFixedSize(800, 600);
    mainFrame = new DFrame(this);

//    mainFrame->setAttribute(Qt::WA_TranslucentBackground, true);



    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setSpacing(5);

    topFrame = new DFrame(mainFrame);
    topFrameSetting();
    botFrame = new DFrame(mainFrame);
    botFrameSetting();
    botFrame->setFrameStyle(DFrame::NoFrame);
//    DPalette pa = DApplicationHelper::instance()->palette(topFrame);
//    pa.setBrush(DPalette::Background, QColor(200, 200, 200));
//    DApplicationHelper::instance()->setPalette(topFrame, pa);
//    topFrame->setAutoFillBackground(true);


    mainLayout->addWidget(topFrame, 2);
    mainLayout->addWidget(botFrame, 8);


    this->addContent(mainFrame);


}

void PartitionWidget::topFrameSetting()
{
    picLabel = new DLabel(topFrame);
    picLabel->setMaximumSize(60, 70);
    picLabel->setStyleSheet("border:1px solid gray");

    titleLabel = new DLabel(tr("Will XXX system space be partitioned?"), topFrame);
    QFont font;
    font.setBold(true);
    font.setPointSize(13);
    titleLabel->setFont(font);
    tipLabel = new DLabel(tr("Click the '+' button to increase the number of partitions on the device,and click each partition to change the name \n formate"), topFrame);
    QHBoxLayout *hLayout = new QHBoxLayout(topFrame);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(20, 10, 0, 10);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(titleLabel);
    vLayout->addWidget(tipLabel);

    hLayout->addWidget(picLabel, 1);
    hLayout->addLayout(vLayout, 10);
}

void PartitionWidget::botFrameSetting()
{
    pieWidget = new PieChartWidget(botFrame);
    pieWidget->setMinimumSize(200, 300);
    QVBoxLayout *topLayout = new QVBoxLayout(botFrame);
    QHBoxLayout *horLayput = new QHBoxLayout();
    horLayput->addWidget(pieWidget);

    //分区详细页
    infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);
    partInfoShowing();
    horLayput->addLayout(infoLayout);
    topLayout->addLayout(horLayput);

    //按钮
    applyBtn = new DPushButton(tr("Apply"), botFrame);
    connect(applyBtn, &DPushButton::clicked, this, [ = ] {
        if (dataValue.size() < 8)
        {
            listWidget = partWidget->findChildren<QWidget *>("test");
            qDebug() << "listWidget.size()" << listWidget.size() << listWidget;
            for (int i = 0; i < listWidget.size(); i++) {
                qreal size = listWidget.at(i)->findChild<DLineEdit *>("sizeEdit")->text().toInt();
                qDebug() << "size" << size;
                dataValue.append(size);
                QString str = listWidget.at(i)->findChild<DLineEdit *>("sizeEdit")->text() + partComCobox->currentText();
                strName.append(str);
                listWidget.at(i)->findChild<DLineEdit *>("sizeEdit")->setText("");
            }

            pieWidget->update();
        }

    });
    cancleBtn = new DPushButton(tr("Cancle"), botFrame);
    reveBtn = new DPushButton(tr("Revert"), botFrame);
    applyBtn->setMinimumWidth(120);
    cancleBtn->setMinimumWidth(120);
    reveBtn->setMinimumWidth(120);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(reveBtn, 1, Qt::AlignRight);
    btnLayout->addWidget(cancleBtn);
    btnLayout->addWidget(applyBtn);

    topLayout->addLayout(btnLayout);


}

void PartitionWidget::partInfoShowing()
{
    deviceInfoLabel = new DLabel(tr("Device Information"), botFrame);
    QHBoxLayout *line1Layout = new QHBoxLayout();
    deviceNameLabel = new DLabel(tr("Device:"), botFrame);
    deviceName = new DLabel("SSD256G", botFrame);
    line1Layout->addWidget(deviceNameLabel);
    line1Layout->addWidget(deviceName);

    QHBoxLayout *line2Layout = new QHBoxLayout();
    deviceFormateLabel = new DLabel(tr("Formate:"), botFrame);
    deviceFormate = new DLabel("EXT3", botFrame);
    line2Layout->addWidget(deviceFormateLabel);
    line2Layout->addWidget(deviceFormate);

    QHBoxLayout *line3Layout = new QHBoxLayout();
    selectedPartLabel = new DLabel(tr("Selected Partition:"), botFrame);
    selectedPartition = new DLabel("SDA3");
    line3Layout->addWidget(selectedPartLabel);
    line3Layout->addWidget(selectedPartition);

    QHBoxLayout *line4Layout = new QHBoxLayout();
    allMemoryLabel  = new DLabel(tr("Total Capacity:"), botFrame);
    allMemory = new DLabel("125GB", botFrame);
    line4Layout->addWidget(allMemoryLabel);
    line4Layout->addWidget(allMemory);
//    allMemoryLabel->setStyleSheet("border:1px solid gray");
//    allMemory->setStyleSheet("border:1px solid gray");

    QHBoxLayout *line5Layout = new QHBoxLayout();
    partInfoLabel = new DLabel(tr("Partition Information"), botFrame);
    partInfoLabel->setMaximumSize(100, 50);
    addButton = new DIconButton(DStyle::SP_IncreaseElement);
    remButton = new DIconButton(DStyle::SP_DecreaseElement);
    line5Layout->addWidget(partInfoLabel);
    connect(addButton, &DIconButton::clicked, this, &PartitionWidget::addWidget);
    connect(remButton, &DPushButton::clicked, this, &PartitionWidget::remWidget);
    partInfoLabel->setMaximumWidth(130);
    line5Layout->addWidget(addButton);
    line5Layout->addWidget(remButton);
    line5Layout->setContentsMargins(0, 0, 180, 0);
//    partInfoLabel->setStyleSheet("border:1px solid gray");

    partWidget = new QWidget(botFrame);
    partWidget->setFixedSize(400, 250);

    infoLayout->addWidget(deviceInfoLabel);
    infoLayout->addLayout(line1Layout);
    infoLayout->addLayout(line2Layout);
    infoLayout->addLayout(line3Layout);
    infoLayout->addLayout(line4Layout);
    infoLayout->addLayout(line5Layout);
    infoLayout->addWidget(partWidget);

    scroll = new DScrollArea(partWidget);
    scroll->setMaximumHeight(180);
    scroll->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    scroll->setWidgetResizable(true);
    scrollWidget = new QWidget(partWidget);
    scrollLayout  = new QVBoxLayout(scrollWidget);

    addWidget();
    if (scrollLayout->count() == 1) {
        remButton->setEnabled(false);
    }


}

void PartitionWidget::addWidget()
{
    infoWidget = new QWidget(partWidget);
    infoWidget->setObjectName("test");
    infoWidget->setFixedSize(380, 140);
//    listWidget.append(infoWidget);
//    infoWidget->setStyleSheet("border:1px solid red");
    scrollLayout->addWidget(infoWidget);
    scrollWidget->setMinimumSize(380, scrollLayout->count() * 140);
    qDebug() << "12121212121" << scrollLayout->count() * infoWidget->height();
    scroll->setWidget(scrollWidget);
    layoutCount = scrollLayout->count();
    if (layoutCount > 1) {
        remButton->setEnabled(true);
    }
    partedInfo();
}

void PartitionWidget::remWidget()
{
    QWidget *p = scrollLayout->itemAt(layoutCount - 1)->widget();
    if (layoutCount == 1)
        return;
    scrollLayout->removeWidget(p);
    layoutCount = scrollLayout->count();
    scrollWidget->setMinimumSize(380, layoutCount * 140);
    qDebug() << "21212121212121" << layoutCount * 140;
}

void PartitionWidget::partedInfo()
{
    QVBoxLayout *vLayout = new QVBoxLayout(infoWidget);
    vLayout->setContentsMargins(5, 10, 0, 10);
    vLayout->setSpacing(10);
    QHBoxLayout *lineLayout1 = new QHBoxLayout();
    partNameLabel = new DLabel(tr("Partition name:"), infoWidget);
    partNameEdit = new DLineEdit(infoWidget);
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->lineEdit()->setPlaceholderText("SSD256G");
    }
    partNameLabel->setMaximumHeight(20);
    lineLayout1->addWidget(partNameLabel);
    lineLayout1->addWidget(partNameEdit);

    QHBoxLayout *lineLayout2 = new QHBoxLayout();
    partFormateLabel = new DLabel(tr("Partiton formate:"), infoWidget);
    partFormateCombox = new DComboBox(infoWidget);
    partFormateCombox->addItem("APFS");
    lineLayout2->addWidget(partFormateLabel, 1);
    lineLayout2->addWidget(partFormateCombox, 3);

    QHBoxLayout *lineLayout3 = new QHBoxLayout();
    partSizeLabel = new DLabel(tr("Partition size:"), infoWidget);
    partSizeEdit = new DLineEdit(infoWidget);
    partSizeEdit->setObjectName("sizeEdit");
    if (partSizeEdit->text().isEmpty()) {
        partSizeEdit->lineEdit()->setPlaceholderText("125");
    }
    partComCobox = new DComboBox(infoWidget);
    partComCobox->addItem("GB");
    partComCobox->addItem("MB");
    lineLayout3->addWidget(partSizeLabel, 1);
    lineLayout3->addWidget(partSizeEdit, 2);
    lineLayout3->addWidget(partComCobox, 1);

    if (layoutCount > 1) {
        DHorizontalLine *line = new DHorizontalLine(infoWidget);
        line->setLineWidth(2);
        vLayout->addWidget(line);
    }
    vLayout->addLayout(lineLayout1);
    vLayout->addLayout(lineLayout2);
    vLayout->addLayout(lineLayout3);
    layoutCount = scrollLayout->count();

    qDebug() << "xzxzxz" << layoutCount;

}

void PartitionWidget::paintEvent(QPaintEvent *event)
{

}
