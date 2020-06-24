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
#include "common.h"
#include <QDebug>

PartitionWidget::PartitionWidget(QWidget *parent)
    : DDialog(parent)
{
    //    setAttribute(Qt::WA_DeleteOnClose, true);
    initUi();
    recPartitionInfo();
    initConnection();
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
    //分区页最上端的布局等
    topFrameSetting();
    topFrame->setFrameRounded(true);
    DPalette pa = DApplicationHelper::instance()->palette(topFrame);
    pa.setBrush(DPalette::Base, pa.itemBackground());
    DApplicationHelper::instance()->setPalette(topFrame, pa);
    topFrame->setAutoFillBackground(true);
    midFrame = new DFrame(mainFrame);
    //分区页中间部分的图形绘制
    midFrameSetting();
    midFrame->setFrameStyle(DFrame::NoFrame);
    botFrame = new DFrame(mainFrame);
    //分区页最下端的布局等
    botFrameSetting();
    botFrame->setFrameStyle(DFrame::NoFrame);
    mainLayout->addWidget(tipLabel, 1);
    mainLayout->addWidget(topFrame, 1);
    mainLayout->addWidget(midFrame, 3);
    mainLayout->addWidget(botFrame, 10);
    setIcon(QIcon::fromTheme(app_name));
    this->addContent(mainFrame);

}

void PartitionWidget::topFrameSetting()
{
    //整体水平布局
    QHBoxLayout *hLayout = new QHBoxLayout(topFrame);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(30, 0, 0, 0);
    DLabel *picLabel = new DLabel(topFrame);
    picLabel->setPixmap(Common::getIcon("labeldisk").pixmap(85, 85));
    picLabel->setMinimumSize(85, 85);
    //垂直布局-右侧三行标签
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(10, 20, 0, 20);
    DLabel *deviceInfoLabel = new DLabel(tr("Disk Information"), topFrame);
    DFontSizeManager::instance()->bind(deviceInfoLabel, DFontSizeManager::T6);
    //第一行
    QHBoxLayout *line1Layout = new QHBoxLayout();
    line1Layout->addWidget(deviceInfoLabel);
    line1Layout->addStretch();
    //第二行
    QHBoxLayout *line2Layout = new QHBoxLayout();
    DLabel *allMemoryLabel = new DLabel(tr("Capacity:"), topFrame);
    allMemory = new DLabel("256GiB", topFrame);
    allMemory->setMinimumWidth(90);
    allMemory->setAlignment(Qt::AlignLeft);
    DLabel *selectedPartLabel = new DLabel(tr("Partition selected:"), topFrame);
    selectedPartition = new DLabel("sda3");
    line2Layout->addWidget(allMemoryLabel);
    line2Layout->addWidget(allMemory);
    line2Layout->addWidget(selectedPartLabel);
    line2Layout->addWidget(selectedPartition);
    line2Layout->addStretch();
    //第三行
    QHBoxLayout *line3Layout = new QHBoxLayout();
    DLabel *deviceNameLabel = new DLabel(tr("Disk:"), topFrame);
    deviceName = new DLabel("/dev/sda", topFrame);
    deviceName->setMinimumWidth(103);
    deviceName->setAlignment(Qt::AlignLeft);
    DLabel *deviceFormateLabel = new DLabel(tr("File system:"), topFrame);
    deviceFormate = new DLabel("EXT3", topFrame);
    line3Layout->addWidget(deviceNameLabel);
    line3Layout->addWidget(deviceName);
    line3Layout->addWidget(deviceFormateLabel);
    line3Layout->addWidget(deviceFormate);
    line3Layout->addStretch();
    vLayout->addLayout(line1Layout);
    vLayout->addLayout(line2Layout);
    vLayout->addLayout(line3Layout);

    hLayout->addWidget(picLabel, 1);
    hLayout->addLayout(vLayout, 10);
}

void PartitionWidget::midFrameSetting()
{
    //调用绘制的图形
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
    //整体垂直布局-三行
    QVBoxLayout *vLayout = new QVBoxLayout(partWidget);
    //第一行
    partInfoLabel = new DLabel(tr("Partition Information"), partWidget);
    DFontSizeManager::instance()->bind(partInfoLabel, DFontSizeManager::T5);
    //第二行
    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 0, 0, 0);
    partDoLabel = new DLabel(tr("Number of partitions:"), partWidget);
    DLabel *labelspace = new DLabel(partWidget);
    addButton = new DIconButton(DStyle::SP_IncreaseElement);
    remButton = new DIconButton(DStyle::SP_DecreaseElement);
    remButton->setToolTip(tr("Delete last partition"));
    DLabel *space = new DLabel(partWidget);
    //按钮初始状态
    if (sizeInfo.size() == 0)
        remButton->setEnabled(false);
    else
        remButton->setEnabled(true);
    partNameLabel = new DLabel(tr("Name:"), partWidget);
    partNameEdit = new DLineEdit(partWidget);
    line2Layout->addWidget(partDoLabel, 2);
    line2Layout->addWidget(labelspace, 5);
    line2Layout->addWidget(addButton, 1);
    line2Layout->addWidget(remButton, 1);
    line2Layout->addWidget(space, 1);
    line2Layout->addWidget(partNameLabel, 2);
    line2Layout->addWidget(partNameEdit, 7);
    //第三行
    QHBoxLayout *line3Layout = new QHBoxLayout();
    DLabel *partFormateLabel = new DLabel(tr("File system:"), partWidget);
    partFormateCombox = new DComboBox(partWidget);
    partFormateCombox->addItems(formateList);
    partFormateCombox->setCurrentIndex(2);
    DLabel *partSizeLabel = new DLabel(tr("Size:"), partWidget);
    hSlider = new DSlider(Qt::Horizontal);
    hSlider->setMaximum(100);
    hSlider->setValue(100);
    partSizeEdit = new DLineEdit(partWidget);
    partComCobox = new DComboBox(partWidget);
    partComCobox->addItem("GiB");
    partComCobox->addItem("MiB");
    partComCobox->setCurrentText("GiB");
    line3Layout->addWidget(partFormateLabel, 2);
    line3Layout->addWidget(partFormateCombox, 7);
    line3Layout->addWidget(space, 1);
    line3Layout->addWidget(partSizeLabel, 2);
    line3Layout->addWidget(hSlider, 2);
    line3Layout->addWidget(partSizeEdit, 3);
    line3Layout->addWidget(partComCobox, 2);

    vLayout->addWidget(partInfoLabel);
    vLayout->addLayout(line2Layout);
    vLayout->addLayout(line3Layout);
    vLayout->addStretch();
    //输入框正则表达
    setRegValidator();
}

void PartitionWidget::recPartitionInfo()
{
    //获取数据
    QString s_disksize;
    PartitionInfo data;
    auto it = DMDbusHandler::instance()->probDeviceInfo().find(DMDbusHandler::instance()->getCurPartititonInfo().device_path);
    if (it != DMDbusHandler::instance()->probDeviceInfo().end()) {
        s_disksize = QString::number(Utils::sector_to_unit(it.value().length, it.value().sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GiB";
        data = DMDbusHandler::instance()->getCurPartititonInfo();
    }
    QString devicePath = data.device_path;
    QString deviceSize = s_disksize;
    QString partPath = data.path;
    QString partFstype = Utils::FSTypeToString(static_cast<FSType>(data.fstype));
    //所选空闲分区
    int i = partPath.lastIndexOf("/");
    QString selectPartition = partPath.right(partPath.length() - i - 1);
    selectedPartition->setText(selectPartition);
    deviceName->setText(devicePath);
    allMemory->setText(deviceSize);
    deviceFormate->setText(partFstype);
    total = Utils::sector_to_unit(data.sector_end - data.sector_start, data.sector_size, SIZE_UNIT::UNIT_GIB);
    mTotal = total * 1024;
    //初始值,显示保留两位小数,真正使用保留4位小数
    currentEditSize = QString::number(mTotal, 'f', 4);
    partComCobox->setEnabled(true);
    setSelectUnallocatesSpace();
}

void PartitionWidget::initConnection()
{
    connect(hSlider, &DSlider::valueChanged, this, &PartitionWidget::slotSliderValueChanged);
    connect(partSizeEdit, &DLineEdit::textEdited, this, &PartitionWidget::slotSetSliderValue);
    connect(partNameEdit, &DLineEdit::textEdited, this, &PartitionWidget::slotSetPartName);
    connect(addButton, &DIconButton::clicked, this, &PartitionWidget::addPartitionSlot);
    connect(remButton, &DIconButton::clicked, this, &PartitionWidget::remPartitionSlot);
    connect(applyBtn, &DPushButton::clicked, this, &PartitionWidget::applyBtnSlot);
    connect(reveBtn, &DPushButton::clicked, this, &PartitionWidget::revertBtnSlot);
    connect(cancleBtn, &DPushButton::clicked, this, &PartitionWidget::cancelBtnSlot);
    connect(partComCobox, &DComboBox::currentTextChanged, this, &PartitionWidget::comboxCurTextSlot);
    connect(partChartWidget, &PartChartShowing::sendMoveFlag, this, &PartitionWidget::showTip);
    connect(partChartWidget, &PartChartShowing::sendFlag, this, &PartitionWidget::showSelectPathInfo);
    connect(partChartWidget, &PartChartShowing::judgeLastPartition, this, &PartitionWidget::judgeLastPartitionSlot);
}

double PartitionWidget::sumValue()
{
    double sum = 0.00;
    for (int j = 0; j < sizeInfo.count(); j++) {
        sum = sum + sizeInfo.at(j);
        if (sum >= mTotal - 0.01)
            break;
    }
    return sum;
}

void PartitionWidget::setSelectUnallocatesSpace()
{
    partNameEdit->lineEdit()->setPlaceholderText(tr("Unallocated"));
    partSizeEdit->setText("");
    if (partComCobox->currentText() == "GiB") {
        partSizeEdit->setText(QString::number(total - (sumValue() / 1024),  'f', 2));
    } else {
        partSizeEdit->setText(QString::number(mTotal - sumValue(),  'f', 2));
    }
}

void PartitionWidget::setAddOrRemResult(const bool &isExceed)
{
    partNameEdit->setText("");
    partSizeEdit->setText("");
    if (sizeInfo.size() == 0)
        mflag = 1;
    else
        mflag = 0;
    partChartWidget->transFlag(mflag, m_value);
    hSlider->setValue(100);
    partNameEdit->lineEdit()->setPlaceholderText(tr("Name"));
    partSizeEdit->lineEdit()->setPlaceholderText(tr("Size"));
    setEnable(0, isExceed);
    slotSliderValueChanged(100);
}

void PartitionWidget::setRegValidator()
{
    QRegExp reg("^[0-9]+(.[0-9]{1,4})?$");
    QRegExpValidator *va = new QRegExpValidator(reg, this);
    partSizeEdit->lineEdit()->setValidator(va);
    QRegExp re("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegExpValidator *va1 = new QRegExpValidator(re, this);
    partNameEdit->lineEdit()->setValidator(va1);
}


bool PartitionWidget::max_amount_prim_reached()
{
    bool breachmax = false;
    int primary_count = 0;
    PartitionVec partvector = DMDbusHandler::instance()->getCurDevicePartitionArr();
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    for (int i = 0; i < partvector.size(); i++) {
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
    mflag = flag;
    number = num;
//    qDebug() << "338" << num;
    if (mflag == 1 || mflag == 3) {
        setSelectUnallocatesSpace();
        hSlider->setValue(100);
        number = -1;
    } else if (mflag == 2) {
        hSlider->setValue(static_cast<int>(sizeInfo.at(num) / mTotal * 100));
        partSizeEdit->setText("");
        partNameEdit->setText("");
        partNameEdit->lineEdit()->setPlaceholderText(partName.at(num));
        double clicked = sizeInfo.at(num);
        if (partComCobox->currentText() == "GiB")
            clicked = clicked / 1024;
        partSizeEdit->setText(QString::number(clicked, 'f', 2));
    }
    setEnable(mflag, isExc);
    showTip(flag, num, posX);
}

void PartitionWidget::showTip(const int &hover, const int &num, const int &posX)
{
//    qDebug() << "359" << num;
    int x = this->frameGeometry().x();
    int y = this->frameGeometry().y();
    if (hover == 2) {
        if (partName.at(num) != " ")
            QToolTip::showText(QPoint(x + posX + 5, y + 215), partName.at(num), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 1000);
    } else if (hover == 3 || hover == 1) {
        QToolTip::showText(QPoint(x + posX + 5, y + 215), tr("Unallocated"), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 1000);
    }
}


void PartitionWidget::setEnable(const int &flag, const bool &isExceed)
{
    DPalette pa2 = DApplicationHelper::instance()->palette(partNameLabel);
    if (isExceed == true) {//还有空闲空间剩余
        if (flag == 2) {//选中新建的分区
            setControlEnable(false);
        }  else {
            setControlEnable(true);
        }
        pa2.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    } else {//无空闲空间剩余
        setControlEnable(false);
        if (flag != 2) {
            pa2.setBrush(DPalette::Text, pa2.placeholderText());
            partNameEdit->setEnabled(false);
        } else {
            pa2.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
            partNameEdit->setEnabled(true);
        }
    }
    partNameLabel->setPalette(pa2);//各情况下,分区名称label的样式
    if (sizeInfo.size() == 0) {
        remButton->setEnabled(false);
        applyBtn->setEnabled(false);
    } else {
        remButton->setEnabled(true);
    }
}

void PartitionWidget::setControlEnable(const bool &isTrue)
{
    if (isTrue == true) {
        addButton->setEnabled(true);
        partSizeEdit->setEnabled(true);
        hSlider->setEnabled(true);
        partComCobox->setEnabled(true);
        partFormateCombox->setEnabled(true);

    } else {
        addButton->setEnabled(false);
        partSizeEdit->setEnabled(false);
        hSlider->setEnabled(false);
        partComCobox->setEnabled(false);
        partFormateCombox->setEnabled(false);
    }
    partNameEdit->setEnabled(true);
    setLabelColor(isTrue);
}

void PartitionWidget::setLabelColor(const bool &isOk)
{
    if (isOk == true) {
//        qDebug() << isOk;
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
        botFrame->setPalette(pa);
    } else {
        DPalette pa = DApplicationHelper::instance()->palette(botFrame);
        pa.setBrush(DPalette::Text, pa.placeholderText());
        botFrame->setPalette(pa);
    }
    DPalette pa1 = DApplicationHelper::instance()->palette(partInfoLabel);
    pa1.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    partInfoLabel->setPalette(pa1);
    DPalette pa2 = DApplicationHelper::instance()->palette(partDoLabel);
    pa2.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    partDoLabel->setPalette(pa2);
}

void PartitionWidget::comboxCurTextSlot()
{
//    qDebug() << partSizeEdit->text().toDouble();
    if (partComCobox->currentText() == "MiB") {
        double m = partSizeEdit->text().toDouble();
        if (sizeInfo.size() == 0 && hSlider->value() == 100)
            partSizeEdit->setText(QString::number(mTotal,  'f', 2));
        else
            partSizeEdit->setText(QString::number(m * 1024,  'f', 2));
    } else if (partComCobox->currentText() == "GiB") {
        double m = partSizeEdit->text().toDouble();
        if (sizeInfo.size() == 0 && hSlider->value() == 100)
            partSizeEdit->setText(QString::number(total,  'f', 2));
        else
            partSizeEdit->setText(QString::number(m / 1024,  'f', 2));
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
    if (block == 0) {
        //选中分区大小与整个空闲分区的占比
        if (mflag == 2) {
            if (partComCobox->currentText() == "MiB") {
                strSize = QString::number((static_cast<double>(value) / 100) * mTotal,  'f', 2);
            } else {
                strSize = QString::number((static_cast<double>(value) / 100) * total,  'f', 2);
            }
            partSizeEdit->setText(strSize);
        } else {//剩余空间为总大小,占比情况
            if (partComCobox->currentText() == "MiB") {
                strSize = QString::number((static_cast<double>(value) / 100) * (mTotal - sumValue()),  'f', 2);
                if (mTotal - sumValue() < 51.2) {
                    judgeLastPartitionSlot();
                }
            } else {
                strSize = QString::number(((static_cast<double>(value) / 100) * (mTotal - sumValue())) / 1024,  'f', 2);
                if (total - sumValue() / 1024 < 0.05) {
                    judgeLastPartitionSlot();
                }
            }
            partSizeEdit->setText(strSize);
        }
    }
    currentEditSize = QString::number((static_cast<double>(value) / 100) * (mTotal - sumValue()),  'f', 4);
//    qDebug() << currentEditSize;
    block = 0;
}

void PartitionWidget::slotSetSliderValue()
{
    double value = partSizeEdit->text().toDouble();
    if (partComCobox->currentText() == "MiB")
        value = value / 1024;
    block = 1;
    hSlider->setValue(static_cast<int>((value / total) * 100));
    currentEditSize = QString::number(value * 1024, 'f', 4);
//    qDebug() << value * 1024 << static_cast<int>((value / total) * 100);
}

void PartitionWidget::slotSetPartName()
{
    if (!partName.isEmpty() && number > -1) {
        partName.replace(number, partNameEdit->text());
    }
}

void PartitionWidget::addPartitionSlot()
{
    if (sizeInfo.size() >= 24 || max_amount_prim_reached() == true)
        return;
    double currentSize = 0.00;
    stPart part;
    applyBtn->setEnabled(true);
    //一次新建不超过24个分区
    if (partNameEdit->text().isEmpty()) {
        partNameEdit->setText(" ");
    }
    partName.append(partNameEdit->text());
    //输入框内的值超过剩余空闲空间,以剩余空间新建
    currentSize = currentEditSize.toDouble();
    if (currentSize >= (mTotal - sumValue())) {
        currentSize = mTotal - sumValue();
        part.blast = true;
    }
    if (currentSize <= 51.2)
        return;
    sizeInfo.append(currentSize);
    //绘制新建分区图形
    partChartWidget->transInfos(mTotal, sizeInfo);
    part.name = partNameEdit->text();
    part.fstype = partFormateCombox->currentText();
    Byte_Value sector_size = DMDbusHandler::instance()->getCurPartititonInfo().sector_size;
    part.count = static_cast<int>(currentSize * (MEBIBYTE / sector_size)) ;
//    if (partComCobox->currentText().compare("MiB") == 0) {
//        part.count = static_cast<int>(partSizeEdit->text().toDouble() * (MEBIBYTE / sector_size)) ;
//    } else {
//        part.count = static_cast<int>(partSizeEdit->text().toDouble() * (GIBIBYTE / sector_size));
//    }
    m_patrinfo.push_back(part);
    partChartWidget->update();
    //新增分区后样式变化
    if (sumValue() >= mTotal - 0.01)
        isExc = false;
    else
        isExc = true;
    setAddOrRemResult(isExc);
}

void PartitionWidget::remPartitionSlot()
{
    if (m_patrinfo.size() > 0)
        m_patrinfo.pop_back();
    addButton->setEnabled(true);
    if (sizeInfo.size() > 0) {
        sizeInfo.removeAt(sizeInfo.size() - 1);
        partName.removeAt(partName.size() - 1);
    }
    //绘制删除分区图形
    partChartWidget->transInfos(mTotal, sizeInfo);
    partChartWidget->update();
    //删除分区后样式变化
    isExc = true;
    setAddOrRemResult(isExc);
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
        if (m_patrinfo.at(i).blast)
            newpart.sector_end = curinfo.sector_end;
        else
            newpart.sector_end = newpart.sector_start + m_patrinfo.at(i).count;
        qDebug() << beforend << curinfo.sector_start << curinfo.sector_end;
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
        //newpart .inside_extended && newpart .type == TYPE_UNALLOCATED extend partition
        //newpart .type == TYPE_LOGICAL logical partition
        //newpart .sector_start <= (MEBIBYTE / newpart .sector_size) /* Beginning of disk device */
        if ((newpart .inside_extended && newpart .type == TYPE_UNALLOCATED)
                || newpart .type == TYPE_LOGICAL
                || newpart .sector_start <= (MEBIBYTE / newpart .sector_size)) {
            newpart.sector_start += MEBIBYTE / newpart.sector_size;
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
    //复原,即清空
    m_patrinfo.clear();
    sizeInfo.clear();
    partName.clear();
    partNameEdit->clear();
    partFormateCombox->setCurrentIndex(2);
    partChartWidget->transInfos(mTotal, sizeInfo);
    if (sizeInfo.size() == 0) {
        applyBtn->setEnabled(false);
    }
    partChartWidget->update();
    hSlider->setValue(100);
    slotSliderValueChanged(100);
    setSelectUnallocatesSpace();
    partChartWidget->transFlag(1, 0);
    setEnable(1, true);
}

void PartitionWidget::cancelBtnSlot()
{
    this->close();
}
