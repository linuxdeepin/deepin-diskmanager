/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitionwidget.cpp
 *
 * @brief 创建分区窗口
 *
 * @date 2020-09-08 15:34
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
    setModal(true);
    setFixedSize(900, 600);
    m_mainFrame = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->setSpacing(5);

    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setTitle(tr("Partitioning %1").arg(info.m_path));
    DLabel *tipLabel = new DLabel(tr("Click ‘+’ to increase the number of partitions. Click on each partition to change its name and file system."), m_mainFrame);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    m_topFrame = new DFrame(m_mainFrame);
    //分区页最上端的布局等
    topFrameSetting();
    m_topFrame->setFrameRounded(true);
    DPalette palette = DApplicationHelper::instance()->palette(m_topFrame);
    palette.setBrush(DPalette::Base, palette.itemBackground());
    DApplicationHelper::instance()->setPalette(m_topFrame, palette);
    m_topFrame->setAutoFillBackground(true);

    m_midFrame = new DFrame(m_mainFrame);
    //分区页中间部分的图形绘制
    midFrameSetting();
    m_midFrame->setFrameStyle(DFrame::NoFrame);
//    m_midFrame->setStyleSheet("background:red");
    m_botFrame = new DFrame(m_mainFrame);
//    m_botFrame->setStyleSheet("background:green");
    //分区页最下端的布局等
    botFrameSetting();
    m_botFrame->setFrameStyle(DFrame::NoFrame);
    mainLayout->addWidget(tipLabel, 1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_topFrame, 1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_midFrame, 3);
    mainLayout->addWidget(m_botFrame, 10);
    mainLayout->setSpacing(0);

    setIcon(QIcon::fromTheme(appName));
    addContent(m_mainFrame);

}

void PartitionWidget::topFrameSetting()
{
    //整体水平布局
    QHBoxLayout *hLayout = new QHBoxLayout(m_topFrame);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(30, 0, 0, 0);
    DLabel *picLabel = new DLabel(m_topFrame);
    picLabel->setPixmap(Common::getIcon("disk").pixmap(85, 85));
    picLabel->setMinimumSize(85, 85);
    //垂直布局-右侧三行标签
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(10, 20, 0, 20);
    DLabel *deviceInfoLabel = new DLabel(tr("Disk Information"), m_topFrame);
    DFontSizeManager::instance()->bind(deviceInfoLabel, DFontSizeManager::T6, QFont::Medium);
    QPalette devicePalette;
    devicePalette.setColor(QPalette::WindowText, QColor("#414D68"));
    deviceInfoLabel->setPalette(devicePalette);

    //第一行
    QHBoxLayout *line1Layout = new QHBoxLayout();
    line1Layout->addWidget(deviceInfoLabel);
    line1Layout->addStretch();
    //第二行
    QPalette infoPalette;
    infoPalette.setColor(QPalette::WindowText, QColor("#001A2E"));

    QHBoxLayout *line2Layout = new QHBoxLayout();
    DLabel *allMemoryLabel = new DLabel(tr("Capacity:"), m_topFrame);
    DFontSizeManager::instance()->bind(allMemoryLabel, DFontSizeManager::T8, QFont::Normal);
    allMemoryLabel->setPalette(infoPalette);

    m_allMemory = new DLabel("256GiB", m_topFrame);
    m_allMemory->setMinimumWidth(90);
    m_allMemory->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_allMemory, DFontSizeManager::T8, QFont::Normal);
    m_allMemory->setPalette(infoPalette);

    DLabel *selectedPartLabel = new DLabel(tr("Partition selected:"), m_topFrame);
    DFontSizeManager::instance()->bind(selectedPartLabel, DFontSizeManager::T8, QFont::Normal);
    selectedPartLabel->setPalette(infoPalette);

    m_selectedPartition = new DLabel("sda3");
    DFontSizeManager::instance()->bind(m_selectedPartition, DFontSizeManager::T8, QFont::Normal);
    m_selectedPartition->setPalette(infoPalette);

    line2Layout->addWidget(allMemoryLabel);
    line2Layout->addWidget(m_allMemory);
    line2Layout->addWidget(selectedPartLabel);
    line2Layout->addWidget(m_selectedPartition);
    line2Layout->addStretch();
    //第三行
    QHBoxLayout *line3Layout = new QHBoxLayout();
    DLabel *deviceNameLabel = new DLabel(tr("Disk:"), m_topFrame);
    DFontSizeManager::instance()->bind(deviceNameLabel, DFontSizeManager::T8, QFont::Normal);
    deviceNameLabel->setPalette(infoPalette);

    m_deviceName = new DLabel("/dev/sda", m_topFrame);
    m_deviceName->setMinimumWidth(103);
    m_deviceName->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_deviceName, DFontSizeManager::T8, QFont::Normal);
    m_deviceName->setPalette(infoPalette);

    DLabel *deviceFormateLabel = new DLabel(tr("File system:"), m_topFrame);
    DFontSizeManager::instance()->bind(deviceFormateLabel, DFontSizeManager::T8, QFont::Normal);
    deviceFormateLabel->setPalette(infoPalette);

    m_deviceFormate = new DLabel("EXT3", m_topFrame);
    DFontSizeManager::instance()->bind(m_deviceFormate, DFontSizeManager::T8, QFont::Normal);
    m_deviceFormate->setPalette(infoPalette);

    line3Layout->addWidget(deviceNameLabel);
    line3Layout->addWidget(m_deviceName);
    line3Layout->addWidget(deviceFormateLabel);
    line3Layout->addWidget(m_deviceFormate);
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
    m_midFrame->setMinimumHeight(85);
    QVBoxLayout *mainLayout = new QVBoxLayout(m_midFrame);
    m_partChartWidget = new PartChartShowing(m_midFrame);
    mainLayout->addWidget(m_partChartWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void PartitionWidget::botFrameSetting()
{
    QVBoxLayout *vLayout = new QVBoxLayout(m_botFrame);
    m_partWidget = new QWidget(m_botFrame);
    //分区详细页
    partInfoShowing();
    vLayout->addWidget(m_partWidget, 5);
    //按钮
    m_applyBtn = new DPushButton(tr("Confirm"), m_botFrame);
    //取消
    m_cancleBtn = new DPushButton(tr("Cancel"), m_botFrame);
    //复原
    m_reveBtn = new DPushButton(tr("Revert"), m_botFrame);
    m_applyBtn->setEnabled(false);
    m_applyBtn->setMinimumWidth(170);
    m_cancleBtn->setMinimumWidth(170);
    m_reveBtn->setMinimumWidth(170);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_reveBtn, 1, Qt::AlignLeft);
    btnLayout->addWidget(m_cancleBtn);
    btnLayout->addWidget(m_applyBtn);

    vLayout->addLayout(btnLayout, 1);
    vLayout->setContentsMargins(0, 0, 0, 0);
}

void PartitionWidget::partInfoShowing()
{
    auto formateList = DMDbusHandler::instance()->getAllSupportFileSystem();
    formateList.removeOne("linux-swap");
    //整体垂直布局-三行
    QVBoxLayout *vLayout = new QVBoxLayout(m_partWidget);
    //第一行
    m_partInfoLabel = new DLabel(tr("Partition Information"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partInfoLabel, DFontSizeManager::T6, QFont::Medium);
    QPalette partPalette;
    partPalette.setColor(QPalette::WindowText, QColor("#414D68"));
    m_partInfoLabel->setPalette(partPalette);

    //第二行
    QPalette infoPalette;
    infoPalette.setColor(QPalette::WindowText, QColor("#001A2E"));

    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 0, 0, 0);
    m_partDoLabel = new DLabel(tr("Number of partitions:"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partDoLabel, DFontSizeManager::T8, QFont::Normal);
    m_partDoLabel->setPalette(infoPalette);

    DLabel *labelSpace = new DLabel(m_partWidget);
    m_addButton = new DIconButton(DStyle::SP_IncreaseElement);
    m_remButton = new DIconButton(DStyle::SP_DecreaseElement);
    m_remButton->setToolTip(tr("Delete last partition"));
    DLabel *space = new DLabel(m_partWidget);

    //按钮初始状态
    if (m_sizeInfo.size() == 0)
        m_remButton->setEnabled(false);
    else
        m_remButton->setEnabled(true);

    m_partNameLabel = new DLabel(tr("Name:"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partNameLabel, DFontSizeManager::T8, QFont::Normal);
    m_partNameLabel->setPalette(infoPalette);

    m_partNameEdit = new DLineEdit(m_partWidget);
    m_partNameEdit->lineEdit()->setMaxLength(256);

    line2Layout->addWidget(m_partDoLabel, 1);
    line2Layout->addSpacing(7);
    line2Layout->addWidget(labelSpace, 5);
    line2Layout->addWidget(m_addButton, 1);
    line2Layout->addWidget(m_remButton, 1);
    line2Layout->addWidget(space, 1);
    line2Layout->addWidget(m_partNameLabel, 1);
    line2Layout->addWidget(m_partNameEdit, 7);
    //第三行
    QHBoxLayout *line3Layout = new QHBoxLayout();
    DLabel *partFormateLabel = new DLabel(tr("File system:"), m_partWidget);
    DFontSizeManager::instance()->bind(partFormateLabel, DFontSizeManager::T8, QFont::Normal);
    partFormateLabel->setPalette(infoPalette);

    m_partFormateCombox = new DComboBox(m_partWidget);
    m_partFormateCombox->addItems(formateList);
    m_partFormateCombox->setCurrentIndex(2);

    DLabel *partSizeLabel = new DLabel(tr("Size:"), m_partWidget);
    DFontSizeManager::instance()->bind(partSizeLabel, DFontSizeManager::T8, QFont::Normal);
    partSizeLabel->setPalette(infoPalette);

    m_slider = new DSlider(Qt::Horizontal);
    m_slider->setMaximum(100);
    m_slider->setValue(100);
    m_partSizeEdit = new DLineEdit(m_partWidget);
    m_partComboBox = new DComboBox(m_partWidget);
    m_partComboBox->addItem("GiB");
    m_partComboBox->addItem("MiB");
    m_partComboBox->setCurrentText("GiB");

    line3Layout->addWidget(partFormateLabel, 1);
    line3Layout->addWidget(m_partFormateCombox, 7);
    line3Layout->addWidget(space, 1);
    line3Layout->addWidget(partSizeLabel);
    line3Layout->addWidget(m_slider, 2);
    line3Layout->addWidget(m_partSizeEdit, 3);
    line3Layout->addWidget(m_partComboBox, 2);

    vLayout->addWidget(m_partInfoLabel);
    vLayout->addLayout(line2Layout);
    vLayout->addSpacing(4);
    vLayout->addLayout(line3Layout);
    vLayout->addStretch();
    vLayout->setContentsMargins(0, 0, 0, 0);
    //输入框正则表达
    setRegValidator();
}

void PartitionWidget::recPartitionInfo()
{
    //获取数据
    QString diskSize;
    PartitionInfo data;
    auto it = DMDbusHandler::instance()->probDeviceInfo().find(DMDbusHandler::instance()->getCurPartititonInfo().m_devicePath);

    if (it != DMDbusHandler::instance()->probDeviceInfo().end()) {
        diskSize = QString::number(Utils::sectorToUnit(it.value().length, it.value().sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GiB";
        data = DMDbusHandler::instance()->getCurPartititonInfo();
    }

    QString devicePath = data.m_devicePath;
    QString deviceSize = diskSize;
    QString partPath = data.m_path;
    QString partFstype = Utils::fileSystemTypeToString(static_cast<FSType>(data.m_fileSystemType));
    //所选空闲分区
    int i = partPath.lastIndexOf("/");
    QString selectPartition = partPath.right(partPath.length() - i - 1);
    m_selectedPartition->setText(selectPartition);
    m_deviceName->setText(devicePath);
    m_allMemory->setText(deviceSize);
    m_deviceFormate->setText(partFstype);
    m_total = Utils::sectorToUnit(data.m_sectorEnd - data.m_sectorStart, data.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_totalSize = m_total * 1024;
//    qDebug() << mTotal << total;
    //初始值,显示保留两位小数,真正使用保留4位小数
    m_currentEditSize = QString::number(m_totalSize, 'f', 4);
    m_partComboBox->setEnabled(true);
    setSelectUnallocatesSpace();
}

void PartitionWidget::initConnection()
{
    connect(m_slider, &DSlider::valueChanged, this, &PartitionWidget::onSliderValueChanged);
    connect(m_partSizeEdit, &DLineEdit::textEdited, this, &PartitionWidget::onSetSliderValue);
    connect(m_partNameEdit, &DLineEdit::textEdited, this, &PartitionWidget::onSetPartName);
    connect(m_partNameEdit, &DLineEdit::textEdited, this, &PartitionWidget::onTextChanged);
    connect(m_addButton, &DIconButton::clicked, this, &PartitionWidget::onAddPartition);
    connect(m_remButton, &DIconButton::clicked, this, &PartitionWidget::onRemovePartition);
    connect(m_applyBtn, &DPushButton::clicked, this, &PartitionWidget::onApplyButton);
    connect(m_reveBtn, &DPushButton::clicked, this, &PartitionWidget::onRevertButton);
    connect(m_cancleBtn, &DPushButton::clicked, this, &PartitionWidget::onCancelButton);
    connect(m_partComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxCurTextChange(int)));
    connect(m_partChartWidget, &PartChartShowing::sendMoveFlag, this, &PartitionWidget::onShowTip);
    connect(m_partChartWidget, &PartChartShowing::sendFlag, this, &PartitionWidget::onShowSelectPathInfo);
    connect(m_partChartWidget, &PartChartShowing::judgeLastPartition, this, &PartitionWidget::onJudgeLastPartition);
}

double PartitionWidget::sumValue()
{
    double sum = 0.00;

    for (int i = 0; i < m_sizeInfo.count(); i++) {
        sum = sum + m_sizeInfo.at(i);
        if (sum >= m_totalSize - 0.01)
            break;
    }

    return sum;
}

void PartitionWidget::setSelectUnallocatesSpace()
{
    m_partNameEdit->lineEdit()->setPlaceholderText(tr("Unallocated"));
    m_partSizeEdit->setText("");

    if (m_partComboBox->currentText() == "GiB") {
        m_partSizeEdit->setText(QString::number(m_total - (sumValue() / 1024),  'f', 2));
    } else {
        m_partSizeEdit->setText(QString::number(m_totalSize - sumValue(),  'f', 2));
    }
}

void PartitionWidget::setAddOrRemResult(const bool &isExceed)
{
    m_partNameEdit->setText("");
    m_partSizeEdit->setText("");

    if (m_sizeInfo.size() == 0)
        m_flag = 1;
    else
        m_flag = 0;

    m_partChartWidget->transFlag(m_flag, m_value);
    m_slider->setValue(100);
    m_partNameEdit->lineEdit()->setPlaceholderText(tr("Name"));
    m_partSizeEdit->lineEdit()->setPlaceholderText(tr("Size"));

    setEnable(0, isExceed);
    onSliderValueChanged(100);
}

void PartitionWidget::setRegValidator()
{
    QRegExp reg("^[0-9]+(.[0-9]{1,4})?$");
    QRegExpValidator *va = new QRegExpValidator(reg, this);
    m_partSizeEdit->lineEdit()->setValidator(va);

    QRegExp re("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegExpValidator *va1 = new QRegExpValidator(re, this);
    m_partNameEdit->lineEdit()->setValidator(va1);
}

bool PartitionWidget::maxAmountPrimReached()
{
    bool breachMax = false;
    int primary_count = 0;
    PartitionVec partVector = DMDbusHandler::instance()->getCurDevicePartitionArr();
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();

    for (int i = 0; i < partVector.size(); i++) {
        if (partVector[i].m_type == TYPE_PRIMARY || partVector[i].m_type == TYPE_EXTENDED)
            primary_count++;
    }

    int maxprims = DMDbusHandler::instance()->getCurDeviceInfo().max_prims;
    if (!info.m_insideExtended && primary_count >= maxprims) {
        breachMax = true;
        //        qDebug() << QString("It is not possible to create more than %1 primary partition").arg(maxprims);
        //        qDebug() << QString("If you want more partitions you should first create an extended partition. Such a partition can contain other partitions."
        //                            "Because an extended partition is also a primary partition it might be necessary to remove a primary partition first.");
    }

    return breachMax;
}

void PartitionWidget::onShowSelectPathInfo(const int &flag, const int &num, const int &posX)
{
    m_flag = flag;
    m_number = num;

    if (m_flag == 0) {
        return;
    }
//    qDebug() << "338" << num;
    if (m_flag == 1 || m_flag == 3) {
        setSelectUnallocatesSpace();
        m_slider->setValue(100);
        m_number = -1;
    } else if (m_flag == 2) {
        if (num < 0) {
            DPalette palette = DApplicationHelper::instance()->palette(m_partNameLabel);
            palette.setBrush(DPalette::Text, palette.placeholderText());
            m_partNameLabel->setPalette(palette);
            m_partNameEdit->setEnabled(false);
            return;
        }

        m_slider->setValue(static_cast<int>(m_sizeInfo.at(num) / m_totalSize * 100));
        m_partSizeEdit->setText("");
        m_partNameEdit->setText("");
        m_partNameEdit->lineEdit()->setPlaceholderText(m_partName.at(num));

        double clicked = m_sizeInfo.at(num);

        if (m_partComboBox->currentText() == "GiB")
            clicked = clicked / 1024;
        m_partSizeEdit->setText(QString::number(clicked, 'f', 2));
    }

    setEnable(m_flag, m_isExceed);
    onShowTip(flag, num, posX);
}

void PartitionWidget::onShowTip(const int &hover, const int &num, const int &posX)
{
//    qDebug() << "359" << num;
    int x = this->frameGeometry().x();
    int y = this->frameGeometry().y();

    if (hover == 2) {
        if (m_partName.at(num) != " ")
            QToolTip::showText(QPoint(x + posX + 5, y + 215), m_partName.at(num), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 1000);
    } else if (hover == 3 || hover == 1) {
        QToolTip::showText(QPoint(x + posX + 5, y + 215), tr("Unallocated"), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 1000);
    }
}


void PartitionWidget::setEnable(const int &flag, const bool &isExceed)
{
    DPalette palette = DApplicationHelper::instance()->palette(m_partNameLabel);

    if (isExceed) {//还有空闲空间剩余
        if (flag == 2) {//选中新建的分区
            setControlEnable(false);
            m_partNameEdit->setEnabled(false);
        } else {
            setControlEnable(true);
        }
//        palette.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    } else {//无空闲空间剩余
        setControlEnable(false);
//        if (flag != 2) {
//            palette.setBrush(DPalette::Text, palette.placeholderText());
//            m_partNameEdit->setEnabled(false);
//        } else {
//            palette.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
            m_partNameEdit->setEnabled(false);
//        }
    }

    if (m_partNameEdit->text().toUtf8().size() <= 16) {
        m_partNameEdit->setAlert(false);
        m_partNameEdit->hideAlertMessage();
    } else {
        m_addButton->setEnabled(false);
    }

    m_partNameLabel->setPalette(palette);//各情况下,分区名称label的样式

    if (m_sizeInfo.size() == 0) {
        m_remButton->setEnabled(false);
        m_applyBtn->setEnabled(false);
    } else {
        m_remButton->setEnabled(true);
        m_applyBtn->setEnabled(true);
    }
}

void PartitionWidget::setControlEnable(const bool &isTrue)
{
    if (isTrue) {
        m_addButton->setEnabled(true);
        m_partSizeEdit->setEnabled(true);
        m_slider->setEnabled(true);
        m_partComboBox->setEnabled(true);
        m_partFormateCombox->setEnabled(true);

    } else {
        m_addButton->setEnabled(false);
        m_partSizeEdit->setEnabled(false);
        m_slider->setEnabled(false);
        m_partComboBox->setEnabled(false);
        m_partFormateCombox->setEnabled(false);
    }

    m_partNameEdit->setEnabled(true);
    setLabelColor(isTrue);
}

void PartitionWidget::setLabelColor(const bool &isOk)
{
    if (isOk) {
//        qDebug() << isOk;
        DPalette palette = DApplicationHelper::instance()->palette(m_botFrame);
        palette.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
        m_botFrame->setPalette(palette);
    } else {
        DPalette palette = DApplicationHelper::instance()->palette(m_botFrame);
        palette.setBrush(DPalette::Text, palette.placeholderText());
        m_botFrame->setPalette(palette);
    }

    DPalette palette1 = DApplicationHelper::instance()->palette(m_partInfoLabel);
    palette1.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    m_partInfoLabel->setPalette(palette1);

    DPalette palatte2 = DApplicationHelper::instance()->palette(m_partDoLabel);
    palatte2.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    m_partDoLabel->setPalette(palatte2);
}

void PartitionWidget::onComboxCurTextChange(int index)
{
//    qDebug() << partSizeEdit->text().toDouble();
    if (!m_partSizeEdit->text().isEmpty()) {
        double m = m_currentEditSize.toDouble();

        if (index == 1) {
            if (m_sizeInfo.size() == 0 && m_slider->value() == 100)
                m_partSizeEdit->setText(QString::number(m_totalSize,  'f', 2));
            else
                m_partSizeEdit->setText(QString::number(m,  'f', 2));
        } else if (index == 0) {
//            double m = partSizeEdit->text().toDouble();
            if (m_sizeInfo.size() == 0 && m_slider->value() == 100)
                m_partSizeEdit->setText(QString::number(m_total,  'f', 2));
            else
                m_partSizeEdit->setText(QString::number(m / 1024,  'f', 2));
        }
//        qDebug() << m;
    }

}

void PartitionWidget::onJudgeLastPartition()
{
    m_slider->setEnabled(false);
    m_partSizeEdit->setEnabled(false);
}

void PartitionWidget::onSliderValueChanged(int value)
{
    m_value = value;
    QString size;

    if (m_block == 0) {
        //选中分区大小与整个空闲分区的占比
        if (m_flag == 2) {
            if (m_partComboBox->currentText() == "MiB") {
                size = QString::number((static_cast<double>(value) / 100) * m_totalSize,  'f', 2);
            } else {
                size = QString::number((static_cast<double>(value) / 100) * m_total,  'f', 2);
            }

            m_partSizeEdit->setText(size);
        } else {//剩余空间为总大小,占比情况
            if (m_partComboBox->currentText() == "MiB") {
                size = QString::number((static_cast<double>(value) / 100) * (m_totalSize - sumValue()),  'f', 2);

                if (m_totalSize - sumValue() < 52) {
                    onJudgeLastPartition();
                }
            } else {
                size = QString::number(((static_cast<double>(value) / 100) * (m_totalSize - sumValue())) / 1024,  'f', 2);

                if (m_total - sumValue() / 1024 < 0.05) {
                    onJudgeLastPartition();
                }
            }

            m_partSizeEdit->setText(size);
        }
    }
//    QString strSize1 = QString::number((static_cast<double>(value) / 100) * (m_totalSize - sumValue()),  'f', 2);
    m_currentEditSize = QString::number((static_cast<double>(value) / 100) * (m_totalSize - sumValue()),  'f', 4);
    qDebug() << m_currentEditSize << size << value << m_totalSize - sumValue();
    m_block = 0;
}

void PartitionWidget::onSetSliderValue()
{
    double value = m_partSizeEdit->text().toDouble();
    if (m_partComboBox->currentText() == "MiB")
        value = value / 1024;
    m_block = 1;
    m_slider->setValue(static_cast<int>((value / m_total) * 100));
    m_currentEditSize = QString::number(value * 1024, 'f', 4);
//    qDebug() << currentEditSize;
//    qDebug() << value * 1024 << static_cast<int>((value / total) * 100);
}

void PartitionWidget::onTextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        QByteArray byteArray = text.toUtf8();
        if (byteArray.size() > 16) {
            m_partNameEdit->setAlert(true);
            m_partNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);
            m_addButton->setEnabled(false);
        } else {
            m_partNameEdit->setAlert(false);
            m_partNameEdit->hideAlertMessage();
            m_addButton->setEnabled(true);
        }
    }
}

void PartitionWidget::onSetPartName()
{
    if (!m_partName.isEmpty() && m_number > -1) {
        m_partName.replace(m_number, m_partNameEdit->text());
    }
    qDebug() << m_partNameEdit->text().length();
}

void PartitionWidget::onAddPartition()
{
    DMDbusHandler *handler = DMDbusHandler::instance();
    DeviceInfo device = handler->getCurDeviceInfo();

    if (m_sizeInfo.size() >= 24 || maxAmountPrimReached() == true || (device.partition.size() + m_sizeInfo.size()) > device.max_prims) {
        DMessageManager::instance()->sendMessage(this, QIcon(":/icons/deepin/builtin/warning.svg"), tr("The number of new partitions exceeds the limit"));
        return;
    }

    double currentSize = 0.00;
    stPart part;
    m_applyBtn->setEnabled(true);
    //一次新建不超过24个分区
    if (m_partNameEdit->text().isEmpty()) {
        m_partNameEdit->setText(" ");
    }
    m_partName.append(m_partNameEdit->text());
    //输入框内的值超过剩余空闲空间,以剩余空间新建
    currentSize = m_currentEditSize.toDouble();
    if (currentSize >= (m_totalSize - sumValue())) {
        currentSize = m_totalSize - sumValue();
        part.m_blast = true;
    }

    if (currentSize <= 52) {
        DMessageManager::instance()->sendMessage(this, QIcon(":/icons/deepin/builtin/warning.svg"), tr("To create a partition, you need at least 52 MB"));
        return;
    }
    m_sizeInfo.append(currentSize);
    //绘制新建分区图形
    m_partChartWidget->transInfos(m_totalSize, m_sizeInfo);
    part.m_labelName = m_partNameEdit->text();
    part.m_fstype = m_partFormateCombox->currentText();
    Byte_Value sectorSize = DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize;
    part.m_count = static_cast<Sector>(currentSize * (MEBIBYTE / sectorSize));
//    if (partComCobox->currentText().compare("MiB") == 0) {
//        part.count = static_cast<int>(partSizeEdit->text().toDouble() * (MEBIBYTE / sector_size)) ;
//    } else {
//        part.count = static_cast<int>(partSizeEdit->text().toDouble() * (GIBIBYTE / sector_size));
//    }
    m_patrinfo.push_back(part);
    m_partChartWidget->update();

    //新增分区后样式变化
    if (sumValue() >= m_totalSize - 0.01)
        m_isExceed = false;
    else
        m_isExceed = true;

    setAddOrRemResult(m_isExceed);
}

void PartitionWidget::onRemovePartition()
{
    if (m_patrinfo.size() > 0)
        m_patrinfo.pop_back();

    m_addButton->setEnabled(true);

    if (m_sizeInfo.size() > 0) {
        m_sizeInfo.removeAt(m_sizeInfo.size() - 1);
        m_partName.removeAt(m_partName.size() - 1);
    }

    //绘制删除分区图形
    m_partChartWidget->transInfos(m_totalSize, m_sizeInfo);
    m_partChartWidget->update();
    //删除分区后样式变化
    m_isExceed = true;

    setAddOrRemResult(m_isExceed);
}

void PartitionWidget::onApplyButton()
{
    bool isCreate = true;
    PartitionVec partVector;
    DMDbusHandler *handler = DMDbusHandler::instance();
    PartitionInfo curInfo = handler->getCurPartititonInfo();
    DeviceInfo device = handler->getCurDeviceInfo();
    Sector beforend = curInfo.m_sectorStart;
    Sector deviceLength = handler->getCurDeviceInfoLength();

    for (int i = 0; i < m_patrinfo.size(); i++) {
        PartitionInfo newPart;
        newPart.m_sectorStart = beforend;
        if (m_patrinfo.at(i).m_blast)
            newPart.m_sectorEnd = curInfo.m_sectorEnd;
        else
            newPart.m_sectorEnd = newPart.m_sectorStart + m_patrinfo.at(i).m_count;
        qDebug() << beforend << curInfo.m_sectorStart << curInfo.m_sectorEnd;

        newPart.m_fileSystemType = Utils::stringToFileSystemType(m_patrinfo.at(i).m_fstype);
        newPart.m_fileSystemLabel = m_patrinfo.at(i).m_labelName;
        newPart.m_alignment = ALIGN_MEBIBYTE;
        newPart.m_sectorSize = curInfo.m_sectorSize;
        newPart.m_insideExtended = false;
        newPart.m_busy = false;
        newPart.m_fileSystemReadOnly = false;
        newPart.m_devicePath = curInfo.m_devicePath;
        if (device.disktype == "gpt") {
            newPart.m_type = TYPE_PRIMARY;
        } else {
            //非逻辑分区外没有指明创建的分区类型主分区/扩展分区，默认主分区
            if (curInfo.m_insideExtended) {
                newPart.m_type = TYPE_LOGICAL;
                newPart.m_insideExtended = true;
            } else {
                newPart.m_type = TYPE_PRIMARY;
            }
        }
        //newpart .inside_extended && newpart .type == TYPE_UNALLOCATED extend partition
        //newpart .type == TYPE_LOGICAL logical partition
        //newpart .sector_start <= (MEBIBYTE / newpart .sector_size) /* Beginning of disk device */
        if ((newPart.m_insideExtended && newPart.m_type == TYPE_UNALLOCATED)
                || newPart.m_type == TYPE_LOGICAL
                || newPart.m_sectorStart <= (MEBIBYTE / newPart.m_sectorSize)) {
            newPart.m_sectorStart += MEBIBYTE / newPart.m_sectorSize;
            newPart.m_sectorEnd += MEBIBYTE / newPart.m_sectorSize;
        }

        beforend = newPart.m_sectorEnd + 1;

        if (beforend > curInfo.m_sectorEnd && i < m_patrinfo.size() - 1) {
            isCreate = false;
            //            qDebug() << "create too partition ,no enough space";
            break;
        } else {
            if (newPart.m_sectorEnd > curInfo.m_sectorEnd)
                newPart.m_sectorEnd = curInfo.m_sectorEnd;

            if (deviceLength == curInfo.m_sectorEnd + 1) {
                if ((newPart.m_sectorEnd == curInfo.m_sectorEnd - 1) || (newPart.m_sectorEnd == curInfo.m_sectorEnd))
                    newPart.m_sectorEnd = curInfo.m_sectorEnd - 33;
            }

//            Sector diff = 0;
//            diff = (newpart.sector_end + 1) % (MEBIBYTE / newpart.sector_size);
//            if (diff)
//                newpart.sector_end -= diff;
            partVector.push_back(newPart);
        }
    }
    if (isCreate && partVector.size() > 0) {
        handler->create(partVector);
        close();
    }
}

void PartitionWidget::onRevertButton()
{
    //复原,即清空
    m_patrinfo.clear();
    m_sizeInfo.clear();
    m_partName.clear();
    m_partNameEdit->clear();
    m_partFormateCombox->setCurrentIndex(2);
    m_partComboBox->setCurrentIndex(0);
    m_partChartWidget->transInfos(m_totalSize, m_sizeInfo);

    if (m_sizeInfo.size() == 0) {
        m_applyBtn->setEnabled(false);
    }

    m_partChartWidget->update();
    m_slider->setValue(100);
    onSliderValueChanged(100);
    setSelectUnallocatesSpace();
    m_partChartWidget->transFlag(1, 0);
    setEnable(1, true);
}

void PartitionWidget::onCancelButton()
{
    close();
}
