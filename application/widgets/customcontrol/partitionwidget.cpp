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

PartitionWidget::PartitionWidget(QWidget *parent)
    : DDialog(parent)
{
    //    setAttribute(Qt::WA_DeleteOnClose, true);
    initUi();
    initConnection();
    getPartitionInfo();
}
PartitionWidget::~PartitionWidget()
{
}
void PartitionWidget::initUi()
{
    this->setModal(true);
    this->setFixedSize(900, 600);
    mainFrame = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setSpacing(5);

    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setTitle(tr("Partitioning %1").arg(info.path));
    DLabel *tipLabel = new DLabel(tr("Click ‘+’ to increase the number of partitions. Click on each partition to change its name and file system."), mainFrame);
    tipLabel->setWordWrap(true);
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
    DLabel *deviceInfoLabel = new DLabel(tr("Disk Information"), topFrame);
    DFontSizeManager::instance()->bind(deviceInfoLabel, DFontSizeManager::T6);
    QHBoxLayout *line1Layout = new QHBoxLayout();
    line1Layout->addWidget(deviceInfoLabel);
    line1Layout->addStretch();
    QHBoxLayout *line4Layout = new QHBoxLayout();
    DLabel *allMemoryLabel = new DLabel(tr("Capacity:"), topFrame);
    allMemory = new DLabel("256GiB", topFrame);
    allMemory->setMinimumWidth(90);
    allMemory->setAlignment(Qt::AlignLeft);
    DLabel *selectedPartLabel = new DLabel(tr("Partition selected:"), topFrame);
    selectedPartition = new DLabel("sda3");
    line4Layout->addWidget(allMemoryLabel);
    line4Layout->addWidget(allMemory);
    line4Layout->addWidget(selectedPartLabel);
    line4Layout->addWidget(selectedPartition);
    line4Layout->addStretch();
    QHBoxLayout *line2Layout = new QHBoxLayout();
    DLabel *deviceNameLabel = new DLabel(tr("Disk:"), topFrame);
    deviceName = new DLabel("/dev/sda", topFrame);
    deviceName->setMinimumWidth(103);
    deviceName->setAlignment(Qt::AlignLeft);
    DLabel *deviceFormateLabel = new DLabel(tr("File system:"), topFrame);
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
    applyBtn = new DPushButton(tr("Confirm"), botFrame);
    //取消
    cancleBtn = new DPushButton(tr("Cancel"), botFrame);
    //复原
    reveBtn = new DPushButton(tr("Revert"), botFrame);
    applyBtn->setEnabled(false);
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
//    qDebug() << "202" << selectedpartName;
    QVBoxLayout *vLayout = new QVBoxLayout(partWidget);
    //    QHBoxLayout *line1Layout = new QHBoxLayout();

    partInfoLabel = new DLabel(tr("Partition Information"), partWidget);
    DFontSizeManager::instance()->bind(partInfoLabel, DFontSizeManager::T5);

    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *line2Layout1 = new QHBoxLayout();
    partDoLabel = new DLabel(tr("Number of partitions:"), partWidget);
    DLabel *labelspace = new DLabel(partWidget);
    addButton = new DIconButton(DStyle::SP_IncreaseElement);
    remButton = new DIconButton(DStyle::SP_DecreaseElement);
    remButton->setToolTip(tr("Delete last partition"));
    line2Layout1->addWidget(partDoLabel, 2);
    //    line2Layout1->addStretch();
    line2Layout1->addWidget(labelspace, 5);
    line2Layout1->addWidget(addButton, 1);
    line2Layout1->addWidget(remButton, 1);
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);
    QHBoxLayout *line2Layout2 = new QHBoxLayout();
    partNameLabel = new DLabel(tr("Name:"), partWidget);
    partNameEdit = new DLineEdit(partWidget);
    line2Layout2->addWidget(partNameLabel);
    line2Layout2->addWidget(partNameEdit);
    line2Layout->addLayout(line2Layout1);
    line2Layout->addLayout(line2Layout2);
    line2Layout1->setContentsMargins(0, 0, 30, 0);
    QHBoxLayout *line3Layout = new QHBoxLayout();
    QHBoxLayout *line3Layout1 = new QHBoxLayout();
    DLabel *partFormateLabel = new DLabel(tr("File system:"), partWidget);
    partFormateLabel->setMinimumWidth(80);
    partFormateCombox = new DComboBox(partWidget);
//    qDebug() << DMDbusHandler::instance()->getallsupportfs();
    partFormateCombox->addItems(formateList);
    partFormateCombox->setFixedWidth(300);
    line3Layout1->addWidget(partFormateLabel);
    line3Layout1->addWidget(partFormateCombox);
    QHBoxLayout *line3Layout2 = new QHBoxLayout();
    line3Layout2->setContentsMargins(30, 0, 0, 0);
    DLabel *partSizeLabel = new DLabel(tr("Size:"), partWidget);
    hSlider = new DSlider(Qt::Horizontal);
    partSizeEdit = new DLineEdit(partWidget);
    //    hSlider->setMinimumWidth(80);
    partComCobox = new DComboBox(partWidget);
    partComCobox->addItem("GiB");
    partComCobox->addItem("MiB");
    //    partComCobox->setMaximumWidth(70);
    line3Layout2->addWidget(partSizeLabel, 1);
    line3Layout2->addWidget(hSlider, 2);
    line3Layout2->addWidget(partSizeEdit, 3);
    line3Layout2->addWidget(partComCobox, 2);
    line3Layout->addLayout(line3Layout1);
    //    line3Layout->addStretch();
    line3Layout->addLayout(line3Layout2);
    vLayout->addWidget(partInfoLabel);
    vLayout->addLayout(line2Layout);
    vLayout->addLayout(line3Layout);
    vLayout->addStretch();
}

void PartitionWidget::getPartitionInfo()
{
    QString s_disksize;
    PartitionInfo data;
    auto it = DMDbusHandler::instance()->probDeviceInfo().find(DMDbusHandler::instance()->getCurPartititonInfo().device_path);
    if (it != DMDbusHandler::instance()->probDeviceInfo().end()) {
        s_disksize = QString::number(Utils::sector_to_unit(it.value().length, it.value().sector_size, SIZE_UNIT::UNIT_GIB)) + "GiB";
        data = DMDbusHandler::instance()->getCurPartititonInfo();
    }
    QString s_pdisksize = Utils::format_size(data.sector_end - data.sector_start, data.sector_size);
//    QString s_used = QString::number(Utils::sector_to_unit(data.sectors_used, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GiB";
//    QString s_unused = QString::number(Utils::sector_to_unit(data.sectors_unused, data.sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GiB";
    QString devicePath = data.device_path;
    QString deviceSize = s_disksize;
    QString partPath = data.path;
    partSize = s_pdisksize;
//    QString partUsed = s_used;
//    QString partUnused = s_unused;
    QString partFstype = Utils::FSTypeToString(static_cast<FSType>(data.fstype));
    int i = partPath.lastIndexOf("/");
    QString selectPartition = partPath.right(partPath.length() - i - 1);
    partComCobox->setCurrentText("GiB");
    hSlider->setMaximum(100);
    hSlider->setValue(100);
    selectedpartName = selectPartition;
    selectedPartition->setText(selectPartition);
    deviceName->setText(devicePath);
    allMemory->setText(deviceSize);
    deviceFormate->setText(partFstype);
    if (s_pdisksize.contains("MiB")) {
        int j = partSize.lastIndexOf(" M");
        mTotal = partSize.left(j).toDouble();
        total = mTotal / 1024;
    } else {
        int j = partSize.lastIndexOf(" G");
        total = partSize.left(j).toDouble();
        mTotal = total * 1024;
    }
//    qDebug() << partSize << total << mTotal;
    partComCobox->setEnabled(true);
    setSelectValue();
}

void PartitionWidget::initConnection()
{
    connect(hSlider, &DSlider::valueChanged, this, &PartitionWidget::slotSliderValueChanged);
    connect(partSizeEdit, &DLineEdit::editingFinished, this, &PartitionWidget::slotSetSliderValue);
    connect(partNameEdit, &DLineEdit::textEdited, this, &PartitionWidget::slotSetPartName);
    connect(addButton, &DIconButton::clicked, this, &PartitionWidget::addPartitionSlot);
    connect(remButton, &DIconButton::clicked, this, &PartitionWidget::remPartitionSlot);
    connect(applyBtn, &DPushButton::clicked, this, &PartitionWidget::applyBtnSlot);
    connect(reveBtn, &DPushButton::clicked, this, &PartitionWidget::revertBtnSlot);
    connect(cancleBtn, &DPushButton::clicked, this, &PartitionWidget::cancelBtnSlot);
    connect(partComCobox, &DComboBox::currentTextChanged, this, &PartitionWidget::comboxCurTextSlot);
    connect(partChartWidget, &PartChartShowing::sendFlag, this, &PartitionWidget::showSelectPathInfo);
    connect(partChartWidget, &PartChartShowing::judgeLastPartition, this, &PartitionWidget::judgeLastPartitionSlot);
}

double PartitionWidget::leaveSpace()
{
    double sum = 0.00;
    for (int j = 0; j < sizeInfo.count(); j++) {
        sum = sum + sizeInfo.at(j);
        if (sum >= mTotal)
            break;
    }
    return sum;
}

void PartitionWidget::setSelectValue()
{
    partNameEdit->lineEdit()->setPlaceholderText(tr("Unallocated"));
    partSizeEdit->setText("");
    if (partComCobox->currentText() == "GiB") {
        partSizeEdit->setText(QString::number(total - (leaveSpace() / 1024), 'f', 3));
    } else {
        partSizeEdit->setText(QString::number(mTotal - leaveSpace(), 'f', 3));
    }
}



bool PartitionWidget::max_amount_prim_reached()
{
    bool breachmax = false;
    int primary_count = 0;
    PartitionVec partvector = DMDbusHandler::instance()->getCurDevicePartitionArr();
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    for (unsigned int i = 0; i < partvector.size(); i++) {
        if (partvector[i].type == TYPE_PRIMARY || partvector[i].type == TYPE_EXTENDED)
            primary_count++;
    }
    int maxprims = DMDbusHandler::instance()->getCurDeviceInfo().max_prims;
    if (!info.inside_extended && primary_count >= maxprims) {
        breachmax = true;
        //        qDebug() << QString("It is not possible to create more than %1 primary partition").arg(maxprims);
        //        qDebug() << QString("If you want more partitions you should first create an extended partition. Such a partition can contain other partitions."
        //                            "Because an extended partition is also a primary partition it might be necessary to remove a primary partition first.");
    }
    return breachmax;
}

void PartitionWidget::showSelectPathInfo(const int &flag, const int &num, const int &posX)
{
    int x = this->frameGeometry().x();
    int y = this->frameGeometry().y();
    mflag = flag;
    number = num;
    setEnable();
    if (mflag == 1) {
        hSlider->setValue((mTotal - leaveSpace()) / mTotal * 100);
        QToolTip::showText(QPoint(x + posX, y + 215), tr("Unallocated"), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 2000);
        setSelectValue();
    } else if (mflag == 2) {
        hSlider->setValue(sizeInfo.at(num) / mTotal * 100);
        partSizeEdit->setText("");
        partNameEdit->setText("");
        if (partName.at(num) != " ")
            QToolTip::showText(QPoint(x + posX + 5, y + 215), partName.at(num), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 2000);
        partNameEdit->lineEdit()->setPlaceholderText(partName.at(num));
        double clicked = sizeInfo.at(num);
        if (partComCobox->currentText() == "GiB")
            clicked = clicked / 1024;
        partSizeEdit->setText(QString::number(clicked));
    } else if (mflag == 3) {
        partNameEdit->setText("");
        QToolTip::showText(QPoint(x + posX + 5, y + 215), tr("Unallocated"), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 2000);
        setSelectValue();
        hSlider->setValue(100);
        number = -1;
    }
}

void PartitionWidget::setEnable()
{
//    qDebug() << mflag;
    if (mflag == 2) {
        setUseEnable();
        addButton->setEnabled(false);
        remButton->setEnabled(true);
        partNameEdit->setEnabled(true);
        partSizeEdit->setEnabled(false);
        hSlider->setEnabled(false);
        partComCobox->setEnabled(false);
        partFormateCombox->setEnabled(false);
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setBrush(DPalette::Text, pa.placeholderText());
        botFrame->setPalette(pa);
    } else if (mflag == 3) {
        remButton->setEnabled(true);
        setEnable2();
    } else if (mflag == 1) {
        remButton->setEnabled(false);
        setEnable2();
    }
    setLabelColor();
//    qDebug() << leaveSpace() << mTotal;
    if (leaveSpace() >= mTotal && (mflag == -1 || mflag == 3)) {
//        qDebug() << "11111";
        setLabelColorGray();
    }

}
void PartitionWidget::setUseEnable()
{
    if (leaveSpace() >= mTotal) {
        remButton->setEnabled(true);
        addButton->setEnabled(false);
        partNameEdit->setEnabled(false);
        partSizeEdit->setEnabled(false);
        hSlider->setEnabled(false);
        partComCobox->setEnabled(false);
        partFormateCombox->setEnabled(false);
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setBrush(DPalette::Text, pa.placeholderText());
        botFrame->setPalette(pa);
    } else {
        if (sizeInfo.size() >= 1)
            remButton->setEnabled(true);
        else
            remButton->setEnabled(false);
        addButton->setEnabled(true);
        partNameEdit->setEnabled(true);
        partSizeEdit->setEnabled(true);
        hSlider->setEnabled(true);
        partFormateCombox->setEnabled(true);
        partComCobox->setEnabled(true);
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
        botFrame->setPalette(pa);
        setLabelColor();
    }
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);
}

void PartitionWidget::setEnable2()
{
    addButton->setEnabled(true);
    partNameEdit->setEnabled(true);
    partSizeEdit->setEnabled(true);
    hSlider->setEnabled(true);
    partComCobox->setEnabled(true);
    partFormateCombox->setEnabled(true);
    DPalette pa = DApplicationHelper::instance()->palette(botFrame);
    pa.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    botFrame->setPalette(pa);
}

void PartitionWidget::setLabelColor()
{
    DPalette pa = DApplicationHelper::instance()->palette(partNameLabel);
    pa.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    partNameLabel->setPalette(pa);
    DPalette pa1 = DApplicationHelper::instance()->palette(partInfoLabel);
    pa1.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    partInfoLabel->setPalette(pa1);
    DPalette pa2 = DApplicationHelper::instance()->palette(partDoLabel);
    pa2.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    partDoLabel->setPalette(pa2);
}

void PartitionWidget::setLabelColorGray()
{
    DPalette pa = DApplicationHelper::instance()->palette(partNameLabel);
    pa.setBrush(DPalette::Text, pa.placeholderText());
    partNameLabel->setPalette(pa);
}

void PartitionWidget::comboxCurTextSlot()
{
    if (partComCobox->currentText() == "MiB") {
        GM = 1;
//        partSizeEdit->setText(QString::number(mTotal - leaveSpace()));
        double m = partSizeEdit->text().toDouble();
        partSizeEdit->setText(QString::number(m * 1024, 'f', 3));
//        qDebug() << m;

    } else if (partComCobox->currentText() == "GiB") {
        GM = 2;
        double m = partSizeEdit->text().toDouble();
        partSizeEdit->setText(QString::number(m / 1024, 'f', 3));
//        partSizeEdit->setText(QString::number(total - (leaveSpace() / 1024)));
    }
}

void PartitionWidget::judgeLastPartitionSlot()
{
    hSlider->setEnabled(false);
    partSizeEdit->setEnabled(false);
}

void PartitionWidget::slotSliderValueChanged(int value)
{
    m_value = value;
    QString strSize;
    //    mflag = -1;
    if (block == 0) {
        if (mflag == 2 || mflag == 0) {
            if (partComCobox->currentText() == "MiB") {
                strSize = QString::number((static_cast<double>(value) / 100) * mTotal, 'f', 3);
            } else {
                strSize = QString::number((static_cast<double>(value) / 100) * total, 'f', 3);
            }
            partSizeEdit->setText(strSize);
        } else {

            if (partComCobox->currentText() == "MiB") {
                strSize = QString::number((static_cast<double>(value) / 100) * (mTotal - leaveSpace()), 'f', 3);
            } else {
                strSize = QString::number(((static_cast<double>(value) / 100) * (mTotal - leaveSpace())) / 1024, 'f', 3);
            }
            partSizeEdit->setText(strSize);
        }

    }
    block = 0;
}

void PartitionWidget::slotSetSliderValue()
{
    double value = partSizeEdit->text().toDouble();
    QString sizeTemp = partSizeEdit->text();
    if (partComCobox->currentText() == "MiB")
        value = value / 1024;
    partSizeEdit->setText(sizeTemp);
    block = 1;
    hSlider->setValue((value / total) * 100);
}

void PartitionWidget::slotSetPartName()
{
    if (!partName.isEmpty() && number > -1) {
        partName.replace(number, partNameEdit->text());
    }
}

void PartitionWidget::addPartitionSlot()
{
    stPart part;
    if (sizeInfo.size() >= 24)
        return;
    applyBtn->setEnabled(true);
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->setText(" ");
    }
    partName.append(partNameEdit->text());
    if (GM == 1 || partComCobox->currentText() == "MiB") {
        currentSize = partSizeEdit->text().toDouble();
    } else if (GM == 2 || partComCobox->currentText() == "GiB") {
        currentSize = partSizeEdit->text().toDouble() * 1024;
    }
    if (currentSize >= (mTotal - leaveSpace())) {
        currentSize = mTotal - leaveSpace();
        addButton->setEnabled(false);
    }
    if ((leaveSpace() < mTotal) && (currentSize > 0.00)) {
        sizeInfo.append(currentSize);
    }

    partChartWidget->getData(mTotal, sizeInfo);
    part.name = partNameEdit->text();
    part.fstype = partFormateCombox->currentText();
    Byte_Value sector_size = DMDbusHandler::instance()->getCurPartititonInfo().sector_size;
    if (partComCobox->currentText().compare("MiB") == 0) {
        part.count = currentSize * (MEBIBYTE / sector_size);
    } else {
        part.count = currentSize * (GIBIBYTE / sector_size);
    }
    m_patrinfo.push_back(part);
    comboxCurTextSlot();
    qDebug() << mTotal - leaveSpace() << mTotal  << leaveSpace() << m_value;
    partChartWidget->update();
    partNameEdit->setText("");
    partSizeEdit->setText("");
    partChartWidget->getflag(0, m_value);
    hSlider->setValue(100);
    partNameEdit->lineEdit()->setPlaceholderText(tr("Name"));
    partSizeEdit->lineEdit()->setPlaceholderText(tr("Size"));
    setEnable();
    setUseEnable();
    mflag = -1;
    slotSliderValueChanged(100);
    //    partChartWidget->getflag(0, m_value);
}

void PartitionWidget::remPartitionSlot()
{
    if (m_patrinfo.size() > 0)
        m_patrinfo.pop_back();
    addButton->setEnabled(true);
    if (sizeInfo.size() == 0) {
        return;
    }
    sizeInfo.removeAt(sizeInfo.size() - 1);
    partName.removeAt(partName.size() - 1);
    if (sizeInfo.size() == 0) {
        remButton->setEnabled(false);
        applyBtn->setEnabled(false);
    }
    partChartWidget->getData(mTotal, sizeInfo);
    comboxCurTextSlot();
    partChartWidget->update();
    partNameEdit->setText("");
    partSizeEdit->setText("");
    hSlider->setValue(100);
    partNameEdit->lineEdit()->setPlaceholderText(tr("Name"));
    partSizeEdit->lineEdit()->setPlaceholderText(tr("Size"));
    setEnable();
    setUseEnable();
    mflag = -1;
    slotSliderValueChanged(100);
}

void PartitionWidget::applyBtnSlot()
{
    bool bcancreate = true;
    PartitionVec partvector;
    DMDbusHandler *phandler = DMDbusHandler::instance();
    PartitionInfo curinfo = phandler->getCurPartititonInfo();
    DeviceInfo device = phandler->getCurDeviceInfo();
    Sector beforend = curinfo.sector_start;

    for (int i = 0; i < m_patrinfo.size(); i++) {
        PartitionInfo newpart;
        newpart.sector_start = beforend;
        newpart.sector_end = newpart.sector_start + m_patrinfo.at(i).count;
        beforend = newpart.sector_end + 1;
        newpart.fstype = Utils::StringToFSType(m_patrinfo.at(i).fstype);
        newpart.name = m_patrinfo.at(i).name;
        newpart.alignment = ALIGN_MEBIBYTE;
        newpart.sector_size = curinfo.sector_size;
        newpart.inside_extended = false;
        newpart.busy = false;
        newpart.fs_readonly = false;
        newpart.device_path = curinfo.device_path;
        if (device.disktype == "gpt") {
            newpart.type = TYPE_PRIMARY;
        } else {
            //非逻辑分区外没有指明创建的分区类型主分区/扩展分区，默认主分区
            if (curinfo.inside_extended) {
                newpart.type = TYPE_LOGICAL;
                newpart.inside_extended = true;
            } else {
                newpart.type = TYPE_PRIMARY;
            }
        }
        if (beforend > curinfo.sector_end && i < m_patrinfo.size() - 1) {
            bcancreate = false;
            //            qDebug() << "create too partition ,no enough space";
            break;
        } else {
            if (newpart.sector_end > curinfo.sector_end)
                newpart.sector_end = curinfo.sector_end;
            Sector diff = 0;
            diff = (newpart.sector_end + 1) % (MEBIBYTE / newpart.sector_size);
            if (diff)
                newpart.sector_end -= diff;
            partvector.push_back(newpart);
        }
    }
    if (bcancreate && partvector.size() > 0) {
        phandler->create(partvector);
        close();
    }
}

void PartitionWidget::revertBtnSlot()
{
    m_patrinfo.clear();
    sizeInfo.clear();
    partName.clear();
    partChartWidget->getData(mTotal, sizeInfo);
    comboxCurTextSlot();
    if (sizeInfo.size() == 0) {
        applyBtn->setEnabled(false);
    }
    hSlider->setValue(100);
    partChartWidget->update();
    partSizeEdit->setText("");
    slotSliderValueChanged(100);
    partNameEdit->lineEdit()->setPlaceholderText(tr("Unallocated"));
    partSizeEdit->lineEdit()->setPlaceholderText(QString::number(total));
    partChartWidget->getflag(1, 0);
    setUseEnable();
}

void PartitionWidget::cancelBtnSlot()
{
    this->close();
}
