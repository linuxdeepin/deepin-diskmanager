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
    setAttribute(Qt::WA_DeleteOnClose, true);
    initUi();
    initConnection();
}
PartitionWidget::~PartitionWidget()
{

}
void PartitionWidget::initUi()
{
    this->setModal(true);
    this->setFixedSize(800, 600);
    mainFrame = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setSpacing(5);

    DLabel *titleLabel = new DLabel(tr("Will XXX system space be partitioned?"), mainFrame);
    QFont font;
    font.setBold(true);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T7);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignCenter);
    DLabel *tipLabel = new DLabel(tr("Click the '+' button to increase the number of partitions on the device,\n and click each partition to change the name formate"), mainFrame);
    tipLabel->setAlignment(Qt::AlignCenter);
    topFrame = new DFrame(mainFrame);
    topFrameSetting();
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

    DLabel *picLabel = new DLabel(topFrame);
    picLabel->setPixmap(getIcon("labeldisk").pixmap(85, 85));
    picLabel->setMinimumSize(85, 85);

    QHBoxLayout *hLayout = new QHBoxLayout(topFrame);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(30, 0, 0, 0);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(10, 20, 0, 20);


    DLabel *deviceInfoLabel = new DLabel(tr("Device Information"), topFrame);
    DFontSizeManager::instance()->bind(deviceInfoLabel, DFontSizeManager::T6);
    QHBoxLayout *line1Layout = new QHBoxLayout();

    line1Layout->addWidget(deviceInfoLabel);
    line1Layout->addStretch();
//
    QHBoxLayout *line4Layout = new QHBoxLayout();
    DLabel *allMemoryLabel  = new DLabel(tr("Total Capacity:"), topFrame);
//    allMemoryLabel->setMinimumWidth(100);
    allMemory = new DLabel("256GB", topFrame);
    allMemory->setMinimumWidth(90);
    allMemory->setAlignment(Qt::AlignLeft);
    DLabel *selectedPartLabel = new DLabel(tr("Selected Partition:"), topFrame);
    selectedPartition = new DLabel("sda3");
    line4Layout->addWidget(allMemoryLabel);
    line4Layout->addWidget(allMemory);
    line4Layout->addWidget(selectedPartLabel);
    line4Layout->addWidget(selectedPartition);
    line4Layout->addStretch();

    QHBoxLayout *line2Layout = new QHBoxLayout();
    DLabel *deviceNameLabel = new DLabel(tr("Device:"), topFrame);
//    deviceNameLabel->setMinimumWidth(80);
    deviceName = new DLabel("/dev/sda", topFrame);
    deviceName->setMinimumWidth(140);
    deviceName->setAlignment(Qt::AlignLeft);
    DLabel *deviceFormateLabel = new DLabel(tr("Formate:"), topFrame);

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
    auto formateList = DMDbusHandler::instance()->getallsupportfs();
    qDebug() << "202" << selectedpartName;
    QVBoxLayout *vLayout = new QVBoxLayout(partWidget);
//    QHBoxLayout *line1Layout = new QHBoxLayout();

    DLabel *partInfoLabel = new DLabel(tr("Partition Information"), partWidget);
    DFontSizeManager::instance()->bind(partInfoLabel, DFontSizeManager::T6);

    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 10, 0, 10);
    QHBoxLayout *line2Layout1 = new QHBoxLayout();
    DLabel *partDoLabel = new DLabel(tr("Partition operation:"), partWidget);

    addButton = new DIconButton(DStyle::SP_IncreaseElement);
    remButton = new DIconButton(DStyle::SP_DecreaseElement);
    remButton->setToolTip(tr("Delete the latest partition"));
    line2Layout1->addWidget(partDoLabel);
    line2Layout1->addStretch();
    line2Layout1->addWidget(addButton);
    line2Layout1->addWidget(remButton);

    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);
    QHBoxLayout *line2Layout2 = new QHBoxLayout();

    DLabel *partNameLabel = new DLabel(tr("Partition name:"), partWidget);
    partNameEdit = new DLineEdit(partWidget);

    line2Layout2->addWidget(partNameLabel);
    line2Layout2->addWidget(partNameEdit);
    line2Layout->addLayout(line2Layout1);
    line2Layout->addLayout(line2Layout2);
    line2Layout1->setContentsMargins(0, 0, 30, 0);
    QHBoxLayout *line3Layout = new QHBoxLayout();
    QHBoxLayout *line3Layout1 = new QHBoxLayout();
    DLabel *partFormateLabel = new DLabel(tr("Partiton formate:"), partWidget);

    partFormateCombox = new DComboBox(partWidget);
    qDebug() << DMDbusHandler::instance()->getallsupportfs();
    partFormateCombox->addItems(formateList);
    partFormateCombox->setFixedWidth(220);
    line3Layout1->addWidget(partFormateLabel);
    line3Layout1->addWidget(partFormateCombox);
    QHBoxLayout *line3Layout2 = new QHBoxLayout();
    line3Layout2->setContentsMargins(25, 0, 0, 0);
    DLabel *partSizeLabel = new DLabel(tr("Partition size:"), partWidget);
    hSlider = new DSlider(Qt::Horizontal);
    partSizeEdit = new DLineEdit(partWidget);
    hSlider->setMaximumWidth(60);
    partComCobox = new DComboBox(partWidget);
    partComCobox->addItem("GB");
    partComCobox->addItem("MB");
    partComCobox->setMaximumWidth(70);
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


void PartitionWidget::getPartitionInfo(const PartitionInfo &data, const QString &disksize)
{
    QString s_pdisksize = QString::number(Utils::sector_to_unit(data.sector_end - data.sector_start, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
    QString s_used = QString::number(Utils::sector_to_unit(data.sectors_used, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
    QString s_unused = QString::number(Utils::sector_to_unit(data.sectors_unused, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
    qDebug() << Utils::format_size(data.sector_end - data.sector_start, data.sector_size) << Utils::sector_to_unit(data.sector_end - data.sector_start, data.sector_size, SIZE_UNIT::UNIT_GIB);

    QString devicePath = data.device_path ;
    QString deviceSize = disksize;
    QString partPath = data.path;
    partSize = s_pdisksize;
    QString partFstype = Utils::FSTypeToString((FSType)data.fstype);
    QString partUsed = s_used;
    QString partUnused = s_unused;
    partComCobox->setCurrentText("GB");
    int j = partSize.lastIndexOf("G");
    total = partSize.left(j).toDouble();
    partComCobox->setEnabled(true);
    total = total * 1024;
    partComCobox->setCurrentText("MB");
    if (total < 1) {
        partComCobox->setEnabled(false);
    }
    hSlider->setMaximum(100);
    hSlider->setValue(100);
    allMemory->setText(deviceSize);
    int i = partPath.lastIndexOf("/");
    QString selectPartition = partPath.right(partPath.length() - i - 1);
    selectedPartition->setText(selectPartition);
    selectedpartName = selectPartition;
    deviceName->setText(devicePath);
    deviceFormate->setText(partFstype);
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->lineEdit()->setPlaceholderText(selectedpartName);
    }
    if (partSizeEdit->text().isEmpty()) {
        partSizeEdit->lineEdit()->setPlaceholderText(QString::number(total));
    }
    qDebug() << devicePath << selectPartition << selectedpartName;

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
    connect(partComCobox, &DComboBox::currentTextChanged, this, &PartitionWidget::comboxCurTextSlot);
    connect(partChartWidget, &PartChartShowing::sendFlag, this, &PartitionWidget::showSelectPathInfo);
}

double PartitionWidget::leaveSpace()
{
    double sum = 0.00;
    for (int j = 0; j < sizeInfo.count(); j++) {
        sum = sum + sizeInfo.at(j);
        if (sum >= total)
            break;
    }
    return sum;
}

void PartitionWidget::setSelectValue()
{
    partNameEdit->lineEdit()->setPlaceholderText(tr("Free space"));
    partSizeEdit->setText("");
    if (partComCobox->currentText() == "GB") {
        partSizeEdit->setText(QString::number((total - leaveSpace()) / 1024));
    } else {
        partSizeEdit->setText(QString::number(total - leaveSpace()));
    }
}

bool PartitionWidget::max_amount_prim_reached()
{
    bool breachmax = false;
    int primary_count = 0;
    QVector<PartitionInfo> partvector = DMDbusHandler::instance()->getCurDevicePartitionArr();
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    for (unsigned int i = 0 ; i < partvector.size() ; i ++) {
        if (partvector[i].type == TYPE_PRIMARY || partvector[i].type == TYPE_EXTENDED)
            primary_count ++;
    }
    int maxprims = DMDbusHandler::instance()->getCurDeviceInfo().max_prims;
    if (! info.inside_extended && primary_count >= maxprims) {
        breachmax = true;
        qDebug() << QString("It is not possible to create more than %1 primary partition").arg(maxprims);
        qDebug() << QString("If you want more partitions you should first create an extended partition. Such a partition can contain other partitions."
                            "Because an extended partition is also a primary partition it might be necessary to remove a primary partition first.");

    }
    return breachmax;
}

void PartitionWidget::showSelectPathInfo(const int &flag, const int &num, const int &posX)
{
    int x = this->frameGeometry().x();
    int y = this->frameGeometry().y();
    mflag = flag;

    if (flag == 1) {
        hSlider->setValue((total - leaveSpace()) / total * 100);
        QToolTip::showText(QPoint(x + posX, y + 235), tr("Free Space"), this, QRect(QPoint(x + posX, y + 235), QSize(80, 20)), 2000);
        setSelectValue();

    } else if (flag == 2) {
        hSlider->setValue(sizeInfo.at(num) / total * 100);
        partSizeEdit->setText("");
        if (partName.at(num) != " ")
            QToolTip::showText(QPoint(x + posX + 5, y + 235), partName.at(num), this, QRect(QPoint(x + posX, y + 235), QSize(80, 20)), 2000);
        partNameEdit->lineEdit()->setPlaceholderText(partName.at(num));
        double clicked = sizeInfo.at(num);
        if (partComCobox->currentText() == "GB")
            clicked = clicked / 1024;
        partSizeEdit->setText(QString::number(clicked));

    } else if (flag == 3) {
        QToolTip::showText(QPoint(x + posX + 5, y + 235), tr("Free Space"), this, QRect(QPoint(x + posX, y + 235), QSize(80, 20)), 2000);
        setSelectValue();
        hSlider->setValue(100);
    }
    setEnable();
}

void PartitionWidget::setEnable()
{
    int j = partSize.lastIndexOf("G");
    double total1 = partSize.left(j).toDouble();
    if (mflag == 2) {
        addButton->setEnabled(false);
        remButton->setEnabled(false);
        if (leaveSpace() >= total) {
            remButton->setEnabled(true);
        }
        partNameEdit->setEnabled(false);
        partSizeEdit->setEnabled(false);
        hSlider->setEnabled(false);
        partComCobox->setEnabled(false);
        partFormateCombox->setEnabled(false);
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setBrush(DPalette::Text, pa.placeholderText());
        botFrame->setPalette(pa);
    } else if (mflag == 1 || mflag == 3) {
        qDebug() << mflag;
        addButton->setEnabled(true);
        if (leaveSpace() >= total) {
            remButton->setEnabled(true);
            addButton->setEnabled(false);
        }
        partNameEdit->setEnabled(true);
        partSizeEdit->setEnabled(true);
        hSlider->setEnabled(true);
        if (total1 < 1)
            partComCobox->setEnabled(false);
        else {
            partComCobox->setEnabled(true);
        }
        partFormateCombox->setEnabled(true);
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
        botFrame->setPalette(pa);
    }
//    else {

//    }
}

void PartitionWidget::comboxCurTextSlot()
{
    int j = partSize.lastIndexOf("G");
    double total1 = partSize.left(j).toDouble();

    if (total1 >= 1) {
        if (partComCobox->currentText() == "MB") {
            GM = 1;
            hSlider->setEnabled(true);
        } else if (partComCobox->currentText() == "GB") {
            GM = 2;
            if (total1 < 5) {
                hSlider->setEnabled(false);
            } else {
                hSlider->setEnabled(true);
            }

        }
    }
    if (total1 < 1) {
    }
    qDebug() << "total" << total;
}

void PartitionWidget::slotSliderValueChanged(int value)
{
    qDebug() << value;
    QString strSize;
    if (mflag != 3) {
        if (partComCobox->currentText() == "MB") {
            strSize = QString::number(((double)value / 100) * total, 'f', 2);
        } else {
            strSize = QString::number(((double)value / 100) * (total / 1024), 'f', 2);
        }
    } else {
        if (partComCobox->currentText() == "MB") {
            strSize = QString::number(((double)value / 100) * (total - leaveSpace()), 'f', 2);
        } else {
            strSize = QString::number(((double)value / 100) * ((total - leaveSpace()) / 1024), 'f', 2);
        }
    }

    partSizeEdit->setText(strSize);
}

void PartitionWidget::slotSetSliderValue()
{
    int j = partSize.lastIndexOf("G");
    double total1 = partSize.left(j).toDouble();
    double value = partSizeEdit->text().toDouble();
    if (partComCobox->currentText() == "MB")
        value = value / 1024;
    hSlider->setValue((value / total1) * 100);
}

void PartitionWidget::addPartitionSlot()
{
    stPart part;
    int j = partSize.lastIndexOf("G");
    double total1 = partSize.left(j).toDouble();
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->setText(" ");
    }
    partName.append(partNameEdit->text());
    if (partSizeEdit->text().isEmpty()) {
        partSizeEdit->showAlertMessage(tr("Partition Size is not empty"));
    }
    if (partComCobox->currentText() == "GB") {
        if (partSizeEdit->text().toDouble() > total)
            return;
    }
    currentSize = partSizeEdit->text().toDouble();
    if (GM == 2) {
        total = total1 * 1024;
        currentSize = partSizeEdit->text().toDouble() * 1024;
    }
    if (currentSize > (total - leaveSpace()))
        currentSize = total - leaveSpace();
    if ((leaveSpace() < total) && (currentSize > 0.00)) {
        sizeInfo.append(currentSize);
    }
    if (leaveSpace() >= total) {
        addButton->setEnabled(false);
    } else {
        addButton->setEnabled(true);
    }
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);

    partChartWidget->getData(total, sizeInfo);
    part.name = partNameEdit->text();
    part.fstype = partFormateCombox->currentText();
    Byte_Value sector_size = DMDbusHandler::instance() ->getCurPartititonInfo().sector_size;
    if (partComCobox->currentText().compare("MB") == 0) {
        part.count = currentSize * (MEBIBYTE / sector_size);
    } else {
        part.count = currentSize * (GIBIBYTE / sector_size);
    }
    comboxCurTextSlot();
    qDebug() << total - leaveSpace() << total << currentSize;
    partChartWidget->update();

    partNameEdit->setText("");
    hSlider->setValue(0);
    partSizeEdit->setText("");

}

void PartitionWidget::remPartitionSlot()
{
    m_patrinfo.pop_back();
    addButton->setEnabled(true);
    if (sizeInfo.size() == 0)
        return;
    sizeInfo.removeAt(sizeInfo.size() - 1);
    partName.removeAt(partName.size() - 1);
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    partChartWidget->getData(total, sizeInfo);
    comboxCurTextSlot();
    partChartWidget->update();
    partNameEdit->setText("");
    hSlider->setValue(0);
    partSizeEdit->setText("");
    partNameEdit->lineEdit()->setPlaceholderText(tr("Part Name"));
    partSizeEdit->lineEdit()->setPlaceholderText(tr("Part Size"));
}

void PartitionWidget::applyBtnSlot()
{
    QVector<PartitionInfo> partvector;
    Sector beforend = 0;
    for (int i = 0; i < m_patrinfo.size(); i++) {
        PartitionInfo newpart;
        //  newpart

    }
    close();

}

void PartitionWidget::revertBtnSlot()
{
    m_patrinfo.clear();
    sizeInfo.clear();
    partName.clear();
    partChartWidget->getData(total, sizeInfo);
    comboxCurTextSlot();
    hSlider->setValue(0);
    partChartWidget->update();
    partSizeEdit->setText("");
    partNameEdit->lineEdit()->setPlaceholderText(selectedpartName);
    partSizeEdit->lineEdit()->setPlaceholderText(QString::number(total));
}

void PartitionWidget::cancelBtnSlot()
{
    this->close();
}


