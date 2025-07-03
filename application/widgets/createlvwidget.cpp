// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "createlvwidget.h"
#include "common.h"
#include "customcontrol/passwordinputdialog.h"
#include "messagebox.h"

#include <DScrollArea>

#include <QDebug>
#include <QApplication>


CreateLVWidget::CreateLVWidget(QWidget *parent) : DDialog(parent)
{
    qDebug() << "CreateLVWidget initializing...";
    initUi();
    recPartitionInfo();
    initConnection();
    qDebug() << "CreateLVWidget initialized for VG:" << DMDbusHandler::instance()->getCurVGInfo().m_vgName;
}

void CreateLVWidget::initUi()
{
    qDebug() << "CreateLVWidget::initUi called.";
    setModal(true);
    setFixedSize(900, 600);
    m_mainFrame = new QWidget(this);
    qDebug() << "Main frame created and dialog properties set.";

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->setSpacing(5);

    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    setTitle(tr("Creating logical volumes on %1").arg(vgInfo.m_vgName));
    qDebug() << "Dialog title set to: Creating logical volumes on" << vgInfo.m_vgName;

    DLabel *tipLabel = new DLabel(QString(tr("Click %1 to create a logical volume. ")).arg("+"), m_mainFrame);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);

    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        qDebug() << "Applying LightType theme palette to tipLabel.";
        DPalette tipPalette;
        QColor tipColor("#000000");
        tipColor.setAlphaF(0.7);
        tipPalette.setColor(DPalette::WindowText, tipColor);
        tipLabel->setPalette(tipPalette);
    }  else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        qDebug() << "Applying DarkType theme palette to tipLabel.";
        DPalette tipPalette;
        QColor tipColor("#FFFFFF");
        tipColor.setAlphaF(0.7);
        tipPalette.setColor(DPalette::WindowText, tipColor);
        tipLabel->setPalette(tipPalette);
    }
    qDebug() << "Tip label initialized.";

    m_topFrame = new DFrame(m_mainFrame);
    m_topFrame->setLineWidth(0);
    //逻辑卷页最上端的布局等
    topFrameSetting();
    m_topFrame->setFrameRounded(true);
#if QT_VERSION_MAJOR > 5
    qDebug() << "QT_VERSION_MAJOR > 5, setting topFrame palette.";
    DPalette palette = m_topFrame->palette();
    palette.setBrush(DPalette::Base, palette.itemBackground());
    m_topFrame->setPalette(palette);
#else
    qDebug() << "QT_VERSION_MAJOR <= 5, setting topFrame palette.";
    DPalette palette = DApplicationHelper::instance()->palette(m_topFrame);
    palette.setBrush(DPalette::Base, palette.itemBackground());
    DApplicationHelper::instance()->setPalette(m_topFrame, palette);
#endif
    m_topFrame->setAutoFillBackground(true);
//    m_topFrame->setStyleSheet("background:red");
    m_topFrame->setFixedSize(864,110);
    qDebug() << "Top frame initialized.";

    m_midFrame = new DFrame(m_mainFrame);
    //逻辑卷页中间部分的图形绘制
    midFrameSetting();
    m_midFrame->setFrameStyle(DFrame::NoFrame);
//    m_midFrame->setStyleSheet("background:blue");
    m_midFrame->setFixedSize(864, 85);
    qDebug() << "Mid frame initialized.";

    m_botFrame = new DFrame(m_mainFrame);
//    m_botFrame->setStyleSheet("background:green");
    m_botFrame->setFixedSize(864, 253);
    //逻辑卷页最下端的布局等
    botFrameSetting();
    m_botFrame->setFrameStyle(DFrame::NoFrame);
    qDebug() << "Bottom frame initialized.";

    mainLayout->addWidget(tipLabel, 1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_topFrame, 1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_midFrame, 3);
    mainLayout->addWidget(m_botFrame, 10);
    mainLayout->setSpacing(0);
    qDebug() << "Main layout configured.";

    setIcon(QIcon::fromTheme(appName));
    addContent(m_mainFrame);
    qDebug() << "Application icon and main content set.";
}

void CreateLVWidget::topFrameSetting()
{
    qDebug() << "CreateLVWidget::topFrameSetting called.";
    //整体水平布局
    QHBoxLayout *hLayout = new QHBoxLayout(m_topFrame);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(30, 0, 0, 0);
    DLabel *picLabel = new DLabel(m_topFrame);
    picLabel->setPixmap(Common::getIcon("vg").pixmap(85, 85));
    picLabel->setMinimumSize(85, 85);
    //垂直布局-右侧三行标签
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(10, 20, 0, 20);
    DLabel *deviceInfoLabel = new DLabel(tr("VG Information"), m_topFrame);
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

    DLabel *allMemoryLabel = new DLabel(tr("Capacity:"), m_topFrame);
    DFontSizeManager::instance()->bind(allMemoryLabel, DFontSizeManager::T8, QFont::Normal);
    allMemoryLabel->setPalette(infoPalette);

    m_allMemory = new DLabel("256GiB", m_topFrame);
    m_allMemory->setMinimumWidth(90);
    m_allMemory->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_allMemory, DFontSizeManager::T8, QFont::Normal);
    m_allMemory->setPalette(infoPalette);

    DLabel *selectedPartLabel = new DLabel(tr("LV name:"), m_topFrame);
    DFontSizeManager::instance()->bind(selectedPartLabel, DFontSizeManager::T8, QFont::Normal);
    selectedPartLabel->setPalette(infoPalette);

    m_selectedPartition = new DLabel("sda3");
    DFontSizeManager::instance()->bind(m_selectedPartition, DFontSizeManager::T8, QFont::Normal);
    m_selectedPartition->setPalette(infoPalette);

    //第三行
    m_deviceNameLabel = new DLabel(tr("VG name:"), m_topFrame);
    DFontSizeManager::instance()->bind(m_deviceNameLabel, DFontSizeManager::T8, QFont::Normal);
    m_deviceNameLabel->setPalette(infoPalette);
    QFontMetrics fm(m_deviceNameLabel->font());
    m_deviceNameLabel->setFixedWidth(fm.boundingRect(tr("VG name:")).width() + 5);

    m_deviceName = new DLabel("/dev/sda", m_topFrame);
//    m_deviceName->setMinimumWidth(103);
    m_deviceName->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_deviceName, DFontSizeManager::T8, QFont::Normal);
    m_deviceName->setPalette(infoPalette);

    DLabel *deviceFormateLabel = new DLabel(tr("LV file system:"), m_topFrame);
    DFontSizeManager::instance()->bind(deviceFormateLabel, DFontSizeManager::T8, QFont::Normal);
    deviceFormateLabel->setPalette(infoPalette);

    m_deviceFormate = new DLabel("EXT3", m_topFrame);
    DFontSizeManager::instance()->bind(m_deviceFormate, DFontSizeManager::T8, QFont::Normal);
    m_deviceFormate->setPalette(infoPalette);

    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->addWidget(allMemoryLabel);
    line2Layout->addWidget(m_allMemory);
    line2Layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *line3Layout = new QHBoxLayout();
    line3Layout->addWidget(selectedPartLabel);
    line3Layout->addWidget(m_selectedPartition);
    line3Layout->addStretch();
    line3Layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *line4Layout = new QHBoxLayout();
    line4Layout->addWidget(m_deviceNameLabel);
    line4Layout->addWidget(m_deviceName);
    line4Layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *line5Layout = new QHBoxLayout();
    line5Layout->addWidget(deviceFormateLabel);
    line5Layout->addWidget(m_deviceFormate);
    line5Layout->addStretch();
    line5Layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *line6Layout = new QVBoxLayout();
    line6Layout->addLayout(line2Layout);
    line6Layout->addLayout(line4Layout);
    line6Layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *line7Layout = new QVBoxLayout();
    line7Layout->addLayout(line3Layout);
    line7Layout->addLayout(line5Layout);
    line7Layout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *line8Layout = new QHBoxLayout();
    line8Layout->addLayout(line6Layout);
    line8Layout->addSpacing(50);
    line8Layout->addLayout(line7Layout);
    line8Layout->setContentsMargins(0, 0, 0, 0);

    vLayout->addLayout(line1Layout);
    vLayout->addLayout(line8Layout);
    vLayout->setContentsMargins(0, 10, 0, 15);

    hLayout->addWidget(picLabel);
    hLayout->addLayout(vLayout, 10);
    hLayout->setContentsMargins(20, 0, 0, 0);
    qDebug() << "Top frame UI elements and layouts configured.";
}

void CreateLVWidget::midFrameSetting()
{
    qDebug() << "CreateLVWidget::midFrameSetting called.";
    //调用绘制的图形
    m_midFrame->setMinimumHeight(85);
    QVBoxLayout *mainLayout = new QVBoxLayout(m_midFrame);
    m_partChartWidget = new PartChartShowing(m_midFrame);
    mainLayout->addWidget(m_partChartWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    qDebug() << "Middle frame UI elements and layouts configured.";
}

void CreateLVWidget::botFrameSetting()
{
    qDebug() << "CreateLVWidget::botFrameSetting called.";
    QVBoxLayout *vLayout = new QVBoxLayout(m_botFrame);
    m_partWidget = new QWidget(m_botFrame);
    //逻辑卷详细页
    partInfoShowing();
    vLayout->addWidget(m_partWidget, 5);
    //按钮
    m_applyBtn = new DPushButton(tr("Confirm", "button"), m_botFrame);
    m_applyBtn->setObjectName("confirm");
    m_applyBtn->setAccessibleName("confirm");
    //取消
    m_cancleBtn = new DPushButton(tr("Cancel", "button"), m_botFrame);
    m_cancleBtn->setAccessibleName("cancel");
    //复原
    m_reveBtn = new DPushButton(tr("Revert", "button"), m_botFrame);
    m_reveBtn->setObjectName("revert");
    m_reveBtn->setAccessibleName("revert");
    m_applyBtn->setEnabled(false);
    m_applyBtn->setMinimumWidth(170);
    m_cancleBtn->setMinimumWidth(170);
    m_reveBtn->setMinimumWidth(170);
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_reveBtn, 1, Qt::AlignLeft);
    btnLayout->addWidget(m_cancleBtn);
    btnLayout->addSpacing(10);
    btnLayout->addWidget(m_applyBtn);
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0, 0, 0, 10);

    vLayout->addLayout(btnLayout, 1);
    vLayout->setContentsMargins(0, 0, 0, 0);
    qDebug() << "Bottom frame UI elements and layouts configured.";
}

void CreateLVWidget::partInfoShowing()
{
    qDebug() << "CreateLVWidget::partInfoShowing called.";
    auto formateList = DMDbusHandler::instance()->getAllSupportFileSystem();
    formateList.removeOne("linux-swap");
    qDebug() << "Supported file systems retrieved. Removed linux-swap. Remaining count:" << formateList.count();
    //lvm暂时不支持加密
//    if (!DMDbusHandler::instance()->getIsSystemDisk(DMDbusHandler::instance()->getCurVGInfo().m_vgName)) {
//        formateList = DMDbusHandler::instance()->getEncryptionFormate(formateList);
//    }

    //整体垂直布局-三行
    QVBoxLayout *vLayout = new QVBoxLayout(m_partWidget);
    //第一行
    m_partInfoLabel = new DLabel(tr("LV Information"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partInfoLabel, DFontSizeManager::T6, QFont::Medium);
    QPalette partPalette;
    partPalette.setColor(QPalette::WindowText, QColor("#414D68"));
    m_partInfoLabel->setPalette(partPalette);
    qDebug() << "LV Information label initialized.";

    //第二行
    QPalette infoPalette;
    infoPalette.setColor(QPalette::WindowText, QColor("#001A2E"));

    QHBoxLayout *line2Layout = new QHBoxLayout();
    line2Layout->setContentsMargins(0, 0, 0, 0);
    m_partDoLabel = new DLabel(tr("Create LV:"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partDoLabel, DFontSizeManager::T8, QFont::Normal);
    m_partDoLabel->setPalette(infoPalette);

    DLabel *labelSpace = new DLabel(m_partWidget);
    m_addButton = new DIconButton(DStyle::SP_IncreaseElement);
    m_addButton->setObjectName("addButton");
    m_addButton->setAccessibleName("addButton");
    m_remButton = new DIconButton(DStyle::SP_DecreaseElement);
    m_remButton->setObjectName("removeButton");
    m_remButton->setAccessibleName("removeButton");
    m_remButton->setToolTip(tr("Delete last logical volume"));
    DLabel *space = new DLabel(m_partWidget);

    //按钮初始状态
    if (m_sizeInfo.size() == 0) {
        qDebug() << "No size info, disabling remove button.";
        m_remButton->setEnabled(false);
    } else {
        qDebug() << "Size info present, enabling remove button.";
        m_remButton->setEnabled(true);
    }

    m_partNameLabel = new DLabel(tr("LV name:"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partNameLabel, DFontSizeManager::T8, QFont::Normal);
    m_partNameLabel->setPalette(infoPalette);

    m_partNameEdit = new DLineEdit(m_partWidget);
    m_partNameEdit->lineEdit()->setMaxLength(256);
    m_partNameEdit->setObjectName("lvName");
    m_partNameEdit->setAccessibleName("lvName");
    m_partNameEdit->lineEdit()->setReadOnly(true);
    m_partNameEdit->setClearButtonEnabled(false);

    line2Layout->addWidget(m_partDoLabel, 1);
    line2Layout->addSpacing(7);
    line2Layout->addWidget(labelSpace, 5);
    line2Layout->addWidget(m_addButton, 1);
    line2Layout->addWidget(m_remButton, 1);
    line2Layout->addWidget(space, 1);
    line2Layout->addWidget(m_partNameLabel, 1);
    line2Layout->addWidget(m_partNameEdit, 7);
    qDebug() << "Second row layout elements configured.";
    //第三行
    QHBoxLayout *line3Layout = new QHBoxLayout();
    m_partFormateLabel = new DLabel(tr("LV file system:"), m_partWidget);
    DFontSizeManager::instance()->bind(m_partFormateLabel, DFontSizeManager::T8, QFont::Normal);
    m_partFormateLabel->setPalette(infoPalette);

    m_partFormateCombox = new DComboBox(m_partWidget);
    m_partFormateCombox->addItems(formateList);
    m_partFormateCombox->setCurrentText("ext4");
    m_partFormateCombox->setAccessibleName("File system");

    DLabel *partSizeLabel = new DLabel(tr("LV capacity:"), m_partWidget);
    DFontSizeManager::instance()->bind(partSizeLabel, DFontSizeManager::T8, QFont::Normal);
    partSizeLabel->setPalette(infoPalette);

    m_slider = new DSlider(Qt::Horizontal);
    m_slider->setMaximum(100);
    m_slider->setValue(100);
    m_slider->setAccessibleName("slider");
    m_partSizeEdit = new DLineEdit(m_partWidget);
    m_partSizeEdit->setObjectName("lvSize");
    m_partSizeEdit->setAccessibleName("lvSize");
    m_partComboBox = new DComboBox(m_partWidget);
    m_partComboBox->addItem("GiB");
    m_partComboBox->addItem("MiB");
    m_partComboBox->setCurrentText("GiB");
    m_partComboBox->setAccessibleName("unit");

    line3Layout->addWidget(m_partFormateLabel, 1);
    line3Layout->addWidget(m_partFormateCombox, 7);
    line3Layout->addWidget(space, 1);
    line3Layout->addWidget(partSizeLabel);
    line3Layout->addWidget(m_slider, 2);
    line3Layout->addWidget(m_partSizeEdit, 3);
    line3Layout->addWidget(m_partComboBox, 2);
    qDebug() << "Third row layout elements configured.";

    // 第四行
    DPalette palette;
    palette.setColor(DPalette::Text, QColor("#526a7f"));

    QFont font;
    font.setWeight(QFont::Normal);
    font.setFamily("Source Han Sans");
    font.setPixelSize(12);

    m_emptyLabel = new DLabel(m_partWidget);
#if QT_VERSION_MAJOR > 5
    qDebug() << "QT_VERSION_MAJOR > 5, setting emptyLabel width.";
    m_emptyLabel->setFixedWidth(m_partFormateLabel->fontMetrics().boundingRect(tr("LV file system:")).width());
#else
    qDebug() << "QT_VERSION_MAJOR <= 5, setting emptyLabel width.";
    m_emptyLabel->setFixedWidth(m_partFormateLabel->fontMetrics().width(tr("LV file system:")));
#endif
    m_encryptionInfo = new DLabel(m_partWidget);
    m_encryptionInfo->setFont(font);
    m_encryptionInfo->setPalette(palette);
    m_encryptionInfo->adjustSize();
    m_encryptionInfo->setWordWrap(true);
    m_encryptionInfo->setAlignment(Qt::AlignTop);
    m_encryptionInfo->setAccessibleName("encryptionInfo");

    m_scrollArea = new DScrollArea;
    m_scrollArea->setFrameShadow(QFrame::Plain);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
    m_scrollArea->setWidget(m_encryptionInfo);

    // 设置滚动区域背景颜色为透明
    QPalette areaPalette;
    areaPalette.setColor(QPalette::Base, QColor(255, 255, 255, 0));
    m_scrollArea->setPalette(areaPalette);

    QHBoxLayout *line4Layout = new QHBoxLayout();
    line4Layout->addWidget(m_emptyLabel, 1);
    line4Layout->addWidget(m_scrollArea, 7);
    line4Layout->addStretch();
    qDebug() << "Fourth row layout elements configured.";

    vLayout->addWidget(m_partInfoLabel);
    vLayout->addLayout(line2Layout);
    vLayout->addSpacing(4);
    vLayout->addLayout(line3Layout);
    vLayout->addLayout(line4Layout);
    vLayout->addStretch();
    vLayout->setContentsMargins(0, 0, 0, 0);
    //输入框正则表达
    setRegValidator();
    qDebug() << "Part info showing UI setup complete.";
}

void CreateLVWidget::recPartitionInfo()
{
    qDebug() << "Loading partition info for current VG";
    m_lstLVName.clear();
    if (DMDbusHandler::VOLUMEGROUP == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is VOLUMEGROUP.";
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();

        m_deviceName->setText(vgInfo.m_vgName);
        QString vgSize = vgInfo.m_vgSize;
        if (vgSize.contains("1024")) {
            qDebug() << "VG size contains 1024, reformatting.";
            vgSize = Utils::LVMFormatSize(vgInfo.m_peCount * vgInfo.m_PESize + vgInfo.m_PESize);
        }
        m_allMemory->setText(vgSize);
        m_selectedPartition->setText("unallocated");
        m_deviceFormate->setText("unallocated");
        qDebug() << "VG information set in UI.";

        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        for (int i = 0; i < vgInfo.m_lvlist.count(); i++) {
            LVInfo info = vgInfo.m_lvlist.at(i);
            if (info.m_lvName.isEmpty() && info.m_lvUuid.isEmpty()) {
                lvInfo = info;
                qDebug() << "Found unallocated LV info.";
            } else {
                m_lstLVName.append(info.m_lvName);
                qDebug() << "Added existing LV name to list:" << info.m_lvName;
            }
        }

        Sector lvByteSize = static_cast<Sector>(lvInfo.m_lvLECount * lvInfo.m_LESize);
        m_total = Utils::LVMSizeToUnit(lvByteSize, SIZE_UNIT::UNIT_GIB);
        m_totalSize = m_total * 1024;
        m_peSize = static_cast<int>(Utils::LVMSizeToUnit(vgInfo.m_PESize, SIZE_UNIT::UNIT_MIB));
        m_currentEditSize = QString::number(m_totalSize, 'f', 2);
        m_partComboBox->setEnabled(true);
        setSelectUnallocatesSpace();
        qDebug() << "LV size details calculated and UI updated.";
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is LOGICALVOLUME.";
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();

        m_deviceName->setText(lvInfo.m_vgName);
        QString vgSize = vgInfo.m_vgSize;
        if (vgSize.contains("1024")) {
            qDebug() << "VG size contains 1024, reformatting.";
            vgSize = Utils::LVMFormatSize(vgInfo.m_peCount * vgInfo.m_PESize + vgInfo.m_PESize);
        }
        m_allMemory->setText(vgSize);
        m_selectedPartition->setText("unallocated");
        QString partFstype = Utils::fileSystemTypeToString(static_cast<FSType>(lvInfo.m_lvFsType));
        m_deviceFormate->setText(partFstype);
        qDebug() << "LV information set in UI for existing LV.";

        Sector lvByteSize = static_cast<Sector>(lvInfo.m_lvLECount * lvInfo.m_LESize);
        m_total = Utils::LVMSizeToUnit(lvByteSize, SIZE_UNIT::UNIT_GIB);
        m_totalSize = m_total * 1024;
        m_peSize = static_cast<int>(Utils::LVMSizeToUnit(vgInfo.m_PESize, SIZE_UNIT::UNIT_MIB));
        m_currentEditSize = QString::number(m_totalSize, 'f', 2);
        m_partComboBox->setEnabled(true);
        setSelectUnallocatesSpace();
        qDebug() << "LV size details calculated and UI updated for existing LV.";

        for (int i = 0; i < vgInfo.m_lvlist.count(); i++) {
            LVInfo info = vgInfo.m_lvlist.at(i);
            if (lvInfo.m_lvName.isEmpty() && lvInfo.m_lvUuid.isEmpty()) {
                m_lstLVName.append(info.m_lvName);
                qDebug() << "Added existing LV name to list (empty name/uuid):" << info.m_lvName;
            }
        }
    }

    m_partNameEdit->setText(lvName());
    qDebug() << "LV name edit field set.";
}

void CreateLVWidget::initConnection()
{
    qDebug() << "CreateLVWidget::initConnection called. Connecting signals and slots.";
    connect(m_slider, &DSlider::valueChanged, this, &CreateLVWidget::onSliderValueChanged);
    connect(m_partSizeEdit, &DLineEdit::textChanged, this, &CreateLVWidget::onSetSliderValue);
    connect(m_addButton, &DIconButton::clicked, this, &CreateLVWidget::onAddPartition);
    connect(m_remButton, &DIconButton::clicked, this, &CreateLVWidget::onRemovePartition);
    connect(m_applyBtn, &DPushButton::clicked, this, &CreateLVWidget::onApplyButton);
    connect(m_reveBtn, &DPushButton::clicked, this, &CreateLVWidget::onRevertButton);
    connect(m_cancleBtn, &DPushButton::clicked, this, &CreateLVWidget::onCancelButton);
    connect(m_partComboBox, static_cast<void (DComboBox:: *)(const int)>(&DComboBox::currentIndexChanged),
            this, &CreateLVWidget::onComboxCurTextChange);
    connect(m_partFormateCombox, &DComboBox::currentTextChanged, this, &CreateLVWidget::onComboxFormatTextChange);
    connect(m_partChartWidget, &PartChartShowing::sendMoveFlag, this, &CreateLVWidget::onShowTip);
    connect(m_partChartWidget, &PartChartShowing::sendFlag, this, &CreateLVWidget::onShowSelectPathInfo);
    connect(m_partChartWidget, &PartChartShowing::judgeLastPartition, this, &CreateLVWidget::onJudgeLastPartition);
    qDebug() << "Signals and slots connected.";
}

QString CreateLVWidget::lvName()
{
    qDebug() << "CreateLVWidget::lvName called to generate LV name.";
    QString name = QString("lv%1").arg(m_lstLVName.count() + 1, 2, 10, QLatin1Char('0'));
    for (int i = 0; i <= m_lstLVName.count(); i++) {
        name = QString("lv%1").arg(i + 1, 2, 10, QLatin1Char('0'));
        if (-1 == m_lstLVName.indexOf(name)) {
            qDebug() << "Generated unique LV name:" << name;
            break;
        }
    }

    return name;
}

double CreateLVWidget::sumValue()
{
    qDebug() << "CreateLVWidget::sumValue called.";
    double sum = 0.00;

    for (int i = 0; i < m_sizeInfo.count(); i++) {
        sum = sum + m_sizeInfo.at(i);
        if (sum >= m_totalSize - 0.01) {
            qDebug() << "Sum value reached total size, breaking loop.";
            break;
        }
    }

    qDebug() << "Calculated sum value:" << sum;
    return sum;
}

void CreateLVWidget::setSelectUnallocatesSpace()
{
    qDebug() << "CreateLVWidget::setSelectUnallocatesSpace called.";
    m_partNameEdit->lineEdit()->setPlaceholderText(tr("Unallocated"));
    m_partSizeEdit->setText("");

    if (m_partComboBox->currentText() == "GiB") {
        qDebug() << "Setting unallocated space in GiB.";
        m_partSizeEdit->setText(QString::number(m_total - (sumValue() / 1024), 'f', 2));
    } else {
        qDebug() << "Setting unallocated space in MiB.";
        m_partSizeEdit->setText(QString::number(m_totalSize - sumValue(), 'f', 2));
    }
    qDebug() << "Unallocated space set.";
}

void CreateLVWidget::setAddOrRemResult(const bool &isExceed)
{
    qDebug() << "CreateLVWidget::setAddOrRemResult called. isExceed:" << isExceed;
    m_partNameEdit->setText(lvName());
    m_partSizeEdit->setText("");

    if (m_sizeInfo.size() == 0) {
        qDebug() << "No size info, setting flag to 1.";
        m_flag = 1;
    } else {
        qDebug() << "Size info present, setting flag to 0.";
        m_flag = 0;
    }

    m_partChartWidget->transFlag(m_flag, m_value);
    m_slider->setValue(100);
    m_partNameEdit->lineEdit()->setPlaceholderText(tr("Name"));
    m_partSizeEdit->lineEdit()->setPlaceholderText(tr("Size"));

    setEnable(0, isExceed);
    onSliderValueChanged(100);
    qDebug() << "Add/remove result handled.";
}

void CreateLVWidget::setRegValidator()
{
    qDebug() << "CreateLVWidget::setRegValidator called.";
#if QT_VERSION_MAJOR > 5
    qDebug() << "Setting QRegularExpressionValidator for QT_VERSION_MAJOR > 5.";
    QRegularExpression reg("^[0-9]+(\\.[0-9]{1,4})?$");
    QRegularExpressionValidator *va = new QRegularExpressionValidator(reg, this);
    m_partSizeEdit->lineEdit()->setValidator(va);

    QRegularExpression re("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegularExpressionValidator *va1 = new QRegularExpressionValidator(re, this);
    m_partNameEdit->lineEdit()->setValidator(va1);
#else
    qDebug() << "Setting QRegExpValidator for QT_VERSION_MAJOR <= 5.";
    QRegExp reg("^[0-9]+(\\.[0-9]{1,4})?$");
    QRegExpValidator *va = new QRegExpValidator(reg, this);
    m_partSizeEdit->lineEdit()->setValidator(va);

    QRegExp re("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegExpValidator *va1 = new QRegExpValidator(re, this);
    m_partNameEdit->lineEdit()->setValidator(va1);
#endif
    qDebug() << "Validators set.";
}

void CreateLVWidget::onShowSelectPathInfo(const int &flag, const int &num, const int &posX)
{
    qDebug() << "CreateLVWidget::onShowSelectPathInfo called. Flag:" << flag << ", Num:" << num;
    m_flag = flag;
    m_number = num;

    if (m_flag == 0) {
        qDebug() << "Flag is 0, returning.";
        return;
    }

    if (m_flag == 1 || m_flag == 3) {
        qDebug() << "Flag is 1 or 3, setting unallocated space.";
        setSelectUnallocatesSpace();
        m_slider->setValue(100);
        m_number = -1;
    } else if (m_flag == 2) {
        qDebug() << "Flag is 2.";
        if (num < 0) {
            qDebug() << "Num is less than 0, disabling part name edit.";
#if QT_VERSION_MAJOR > 5
            DPalette palette = m_partNameLabel->palette();
#else
            DPalette palette = DApplicationHelper::instance()->palette(m_partNameLabel);
#endif
            palette.setBrush(DPalette::Text, palette.placeholderText());
            m_partNameLabel->setPalette(palette);
            m_partNameEdit->setEnabled(false);
            return;
        }

        m_slider->setValue(static_cast<int>(m_sizeInfo.at(num) / m_totalSize * 100));
        m_partSizeEdit->setText("");
        m_partNameEdit->setText(lvName());
        m_partNameEdit->lineEdit()->setPlaceholderText(m_partName.at(num));

        double clicked = m_sizeInfo.at(num);

        if (m_partComboBox->currentText() == "GiB") {
            qDebug() << "Converting clicked size to GiB.";
            clicked = clicked / 1024;
        }
        m_partSizeEdit->setText(QString::number(clicked, 'f', 2));
    }

    setEnable(m_flag, m_isExceed);
    onShowTip(flag, num, posX);
    qDebug() << "Show select path info handled.";
}

void CreateLVWidget::onShowTip(const int &hover, const int &num, const int &posX)
{
    qDebug() << "CreateLVWidget::onShowTip called. Hover:" << hover << ", Num:" << num;
    int x = frameGeometry().x();
    int y = frameGeometry().y();

    if (hover == 2) {
        if (m_partName.at(num) != " ") {
            qDebug() << "Showing tip for selected partition:" << m_partName.at(num);
            QToolTip::showText(QPoint(x + posX + 5, y + 215), m_partName.at(num), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 1000);
        }
    } else if (hover == 3 || hover == 1) {
        qDebug() << "Showing tip for unallocated space.";
        QToolTip::showText(QPoint(x + posX + 5, y + 215), tr("Unallocated"), this, QRect(QPoint(x + posX, y + 215), QSize(80, 20)), 1000);
    }
    qDebug() << "Tooltip shown.";
}


void CreateLVWidget::setEnable(const int &flag, const bool &isExceed)
{
    qDebug() << "CreateLVWidget::setEnable called. Flag:" << flag << ", isExceed:" << isExceed;
#if QT_VERSION_MAJOR > 5
    DPalette palette = m_partNameLabel->palette();
#else
    DPalette palette = DApplicationHelper::instance()->palette(m_partNameLabel);
#endif

    if (isExceed) {//还有空闲空间剩余
        qDebug() << "There is free space remaining.";
        if (flag == 2) {//选中新建的逻辑卷
            qDebug() << "Selected new logical volume, disabling controls.";
            setControlEnable(false);
            m_partNameEdit->setEnabled(false);
        } else {
            qDebug() << "Enabling controls.";
            setControlEnable(true);
        }
//        palette.setColor(DPalette::Text, QColor(this->palette().buttonText().color()));
    } else {//无空闲空间剩余
        qDebug() << "No free space remaining, disabling controls.";
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
        qDebug() << "LV name length is acceptable.";
        m_partNameEdit->setAlert(false);
        m_partNameEdit->hideAlertMessage();
    } else {
        qDebug() << "LV name too long, disabling add button.";
        m_addButton->setEnabled(false);
    }

    m_partNameLabel->setPalette(palette);//各情况下,逻辑卷名称label的样式
    qDebug() << "Part name label palette set.";

    m_remButton->setEnabled(m_sizeInfo.size());
    m_applyBtn->setEnabled(m_sizeInfo.size());
    qDebug() << "Remove and apply button enabled state set.";
}

void CreateLVWidget::setControlEnable(const bool &isTrue)
{
    qDebug() << "CreateLVWidget::setControlEnable called. isTrue:" << isTrue;
    m_addButton->setEnabled(isTrue);
    m_partSizeEdit->setEnabled(isTrue);
    m_slider->setEnabled(isTrue);
    m_partComboBox->setEnabled(isTrue);
    m_partFormateCombox->setEnabled(isTrue);

    m_partNameEdit->setEnabled(true);
    setLabelColor(isTrue);
    qDebug() << "Controls enabled/disabled.";
}

void CreateLVWidget::setLabelColor(const bool &isOk)
{
    qDebug() << "CreateLVWidget::setLabelColor called. isOk:" << isOk;
    if (isOk) {
        qDebug() << "Setting label color to normal.";
#if QT_VERSION_MAJOR > 5
        DPalette framePalette = m_botFrame->palette();
#else
        DPalette framePalette = DApplicationHelper::instance()->palette(m_botFrame);
#endif
        framePalette.setColor(DPalette::Text, QColor(palette().buttonText().color()));
        m_botFrame->setPalette(framePalette);
    } else {
        qDebug() << "Setting label color to placeholder.";
#if QT_VERSION_MAJOR > 5
        DPalette palette = m_botFrame->palette();
#else
        DPalette palette = DApplicationHelper::instance()->palette(m_botFrame);
#endif
        palette.setBrush(DPalette::Text, palette.placeholderText());
        m_botFrame->setPalette(palette);
    }

#if QT_VERSION_MAJOR > 5
    DPalette palette1 = m_partInfoLabel->palette();
    DPalette palatte2 = m_partDoLabel->palette();
#else
    DPalette palette1 = DApplicationHelper::instance()->palette(m_partInfoLabel);
    DPalette palatte2 = DApplicationHelper::instance()->palette(m_partDoLabel);
#endif
    palette1.setColor(DPalette::Text, QColor(palette().buttonText().color()));
    m_partInfoLabel->setPalette(palette1);

    palatte2.setColor(DPalette::Text, QColor(palette().buttonText().color()));
    m_partDoLabel->setPalette(palatte2);
    qDebug() << "Label colors set.";
}

void CreateLVWidget::onComboxCurTextChange(int index)
{
    qDebug() << "CreateLVWidget::onComboxCurTextChange called. Index:" << index;
    if (!m_partSizeEdit->text().isEmpty()) {
        double m = m_currentEditSize.toDouble();

        if (index == 1) {
            qDebug() << "Index is 1 (MiB), setting size in MiB.";
            if (m_sizeInfo.size() == 0 && m_slider->value() == 100)
                m_partSizeEdit->setText(QString::number(m_totalSize, 'f', 2));
            else
                m_partSizeEdit->setText(QString::number(m, 'f', 2));
        } else if (index == 0) {
            qDebug() << "Index is 0 (GiB), setting size in GiB.";
            if (m_sizeInfo.size() == 0 && m_slider->value() == 100)
                m_partSizeEdit->setText(QString::number(m_total, 'f', 2));
            else
                m_partSizeEdit->setText(QString::number(m / 1024, 'f', 2));
        }
    }
    qDebug() << "Combobox current text change handled.";
}

void CreateLVWidget::onComboxFormatTextChange(const QString &text)
{
    qDebug() << "CreateLVWidget::onComboxFormatTextChange called. Text:" << text;
    if (m_partSizeEdit->isAlert()) {
        m_partSizeEdit->setAlert(false);
        m_partSizeEdit->hideAlertMessage();
        qDebug() << "Alert message hidden.";
    }

    m_scrollArea->setFixedWidth(m_partFormateCombox->width());
    if (text.contains("AES")) {
        qDebug() << "Format text contains AES.";
        QString text = tr("Use the aes-xts-plain64 standard algorithm to encrypt the disk. "
                          "You should decrypt it before mounting it again.");
        m_encryptionInfo->setFixedSize(m_partFormateCombox->width(),
                                       Common::getLabelAdjustHeight(m_partFormateCombox->width(), text, m_encryptionInfo->font()));
        m_encryptionInfo->setText(text);
    } else if (text.contains("SM4")) {
        qDebug() << "Format text contains SM4.";
        QString text = tr("Use the sm4-xts-plain state cryptographic algorithm to encrypt the disk. "
                          "You should decrypt it before mounting it again. "
                          "Operating Systems that do not support the state cryptographic "
                          "algorithm will not be able to decrypt the disk.");
        m_encryptionInfo->setFixedSize(m_partFormateCombox->width(),
                                       Common::getLabelAdjustHeight(m_partFormateCombox->width(), text, m_encryptionInfo->font()) - 1);
        m_encryptionInfo->setText(text);
    } else {
        qDebug() << "Format text is neither AES nor SM4, clearing encryption info.";
        m_encryptionInfo->setText("");
    }
    qDebug() << "Combobox format text change handled.";
}

void CreateLVWidget::onJudgeLastPartition()
{
    qDebug() << "CreateLVWidget::onJudgeLastPartition called. Disabling slider and size edit.";
    m_slider->setEnabled(false);
    m_partSizeEdit->setEnabled(false);
}

void CreateLVWidget::onSliderValueChanged(int value)
{
    qDebug() << "CreateLVWidget::onSliderValueChanged called. Value:" << value;
    m_value = value;
    QString size;

    if (m_block == 0) {
        qDebug() << "Block is 0.";
        //选中逻辑卷大小与整个空闲空间的占比
        if (m_flag == 2) {
            qDebug() << "Flag is 2 (selected logical volume). Calculating size based on totalSize.";
            if (m_partComboBox->currentText() == "MiB") {
                size = QString::number((static_cast<double>(value) / 100) * m_totalSize, 'f', 2);
            } else {
                size = QString::number((static_cast<double>(value) / 100) * m_total, 'f', 2);
            }

            m_partSizeEdit->setText(size);
        } else {//剩余空间为总大小,占比情况
            qDebug() << "Flag is not 2 (remaining space). Calculating size based on remaining totalSize.";
            if (m_partComboBox->currentText() == "MiB") {
                size = QString::number((static_cast<double>(value) / 100) * (m_totalSize - sumValue()), 'f', 2);
            } else {
                size = QString::number(((static_cast<double>(value) / 100) * (m_totalSize - sumValue())) / 1024, 'f', 2);
            }
            m_partSizeEdit->setText(size);
        }
    }

    m_currentEditSize = QString::number((static_cast<double>(value) / 100) * (m_totalSize - sumValue()),  'f', 2);
    m_block = 0;
    qDebug() << "Slider value change handled. Current edit size:" << m_currentEditSize;
}

void CreateLVWidget::onSetSliderValue()
{
    qDebug() << "CreateLVWidget::onSetSliderValue called.";
    if (m_partSizeEdit->isAlert()) {
        m_partSizeEdit->setAlert(false);
        m_partSizeEdit->hideAlertMessage();
        qDebug() << "Alert message hidden.";
    }

    if(m_partSizeEdit->text().trimmed().isEmpty()) {
        qDebug() << "Part size edit text is empty, disabling add button.";
        m_addButton->setDisabled(true);
        return;
    }

    double value = m_partSizeEdit->text().toDouble();
    if (m_partComboBox->currentText() == "MiB") {
        qDebug() << "Converting value from MiB to GiB.";
        value = value / 1024;
    }
    m_block = 1;
    m_slider->setValue(static_cast<int>((value / (m_total - (sumValue() / 1024))) * 100));
    m_currentEditSize = QString::number(value * 1024, 'f', 2);

    if (0.00 == value || value > (m_total - sumValue() / 1024)) {
        qDebug() << "Value is 0 or exceeds available space, disabling add button.";
        m_addButton->setDisabled(true);
    } else {
        qDebug() << "Value is acceptable, enabling add button.";
        m_addButton->setDisabled(false);
    }
    qDebug() << "Slider value set from text edit.";
}

void CreateLVWidget::onAddPartition()
{
    qDebug() << "CreateLVWidget::onAddPartition called.";
    double currentSize = 0.00;
    stLV part;
    m_partName.append(m_partNameEdit->text());
    qDebug() << "LV name appended:" << m_partNameEdit->text();
    //输入框内的值超过剩余空闲空间,以剩余空间新建
    currentSize = m_currentEditSize.toDouble();
    if (currentSize >= (m_totalSize - sumValue())) {
        qDebug() << "Current size exceeds available space, adjusting to remaining space.";
        currentSize = m_totalSize - sumValue();
        part.m_blast = true;
    }

    int peCount = static_cast<int>(currentSize / m_peSize);
    if ((peCount * m_peSize) < currentSize) {
        peCount += 1;
        qDebug() << "Adjusted PE count to:" << peCount;
    }
    currentSize = peCount * m_peSize;
    QString formate = m_partFormateCombox->currentText();
    if (formate.contains("AES") || formate.contains("SM4")) {
        qDebug() << "Encryption format detected:" << formate;
        if (currentSize <= 100) {
            qDebug() << "Encryption volume too small, showing alert.";
            m_partSizeEdit->setAlert(true);
            m_partSizeEdit->showAlertMessage(tr("To encrypt a volume, it should be larger than 100 MiB"));
            return;
        }

        QColor color = QColor(palette().buttonText().color());

        PasswordInputDialog passwordInputDialog(this);
        passwordInputDialog.setDeviceName(m_partNameEdit->text());
        passwordInputDialog.setObjectName("passwordInputDialog");
        passwordInputDialog.setAccessibleName("passwordInputDialog");
        if (passwordInputDialog.exec() != DDialog::Accepted) {
            qDebug() << "Password dialog not accepted, returning.";
            return;
        } else {
            qDebug() << "Password dialog accepted.";
            part.m_password = passwordInputDialog.getPassword();
            part.m_passwordHint = passwordInputDialog.getPasswordHint();
            if (formate.contains("AES")) {
                part.m_encryption = CRYPT_CIPHER::AES_XTS_PLAIN64;
                part.m_dmName = QString("%1_%2_aesE").arg(m_deviceName->text()).arg(m_partNameEdit->text());
                qDebug() << "Set encryption to AES.";
            } else if (formate.contains("SM4")) {
                part.m_encryption = CRYPT_CIPHER::SM4_XTS_PLAIN64;
                part.m_dmName = QString("%1_%2_sm4E").arg(m_deviceName->text()).arg(m_partNameEdit->text());
                qDebug() << "Set encryption to SM4.";
            }
            part.m_isEncryption = true;
            qDebug() << "Encryption details set.";

            MessageBox warningBox(this);
            warningBox.setObjectName("messageBox");
            warningBox.setAccessibleName("messageBox");
            // 为防止遗忘密码，请您自行备份密码，并妥善保存！  确 定
            warningBox.setWarings(tr("To avoid losing the password, please back up your password and keep it properly!"),
                                  tr("OK", "button"), DDialog::ButtonRecommend, "OK");
            warningBox.setGeometry(pos().x() + (width() - warningBox.width()) / 2, pos().y() + (height() - warningBox.height()) / 2,
                                   warningBox.width(), warningBox.height());
            warningBox.exec();
            qDebug() << "Password backup warning shown.";
        }

#if QT_VERSION_MAJOR > 5
        DPalette palette = m_botFrame->palette();
        palette.setColor(QPalette::ButtonText, color);
        setPalette(palette);
        qDebug() << "Palette set for QT_VERSION_MAJOR > 5.";
#else
        DPalette palette = DApplicationHelper::instance()->palette(m_botFrame);
        palette.setColor(QPalette::ButtonText, color);
        DApplicationHelper::instance()->setPalette(this, palette);
        qDebug() << "Palette set for QT_VERSION_MAJOR <= 5.";
#endif

        formate = formate.trimmed().split(" ").at(0);
        qDebug() << "Formatted encryption type:" << formate;
    }

    m_sizeInfo.append(currentSize);
    //绘制新建逻辑卷图形
    m_partChartWidget->transInfos(m_totalSize, m_sizeInfo);
    part.m_lvName = m_partNameEdit->text();
    part.m_fstype = formate;
    part.m_lvByteSize = static_cast<long long>(currentSize * 1024 * 1024);

    if (m_partComboBox->currentText() == "GiB") {
        qDebug() << "Setting LV size in GiB.";
        part.m_lvSize = QString::number(currentSize / 1024, 'f', 2) + m_partComboBox->currentText();
    } else {
        qDebug() << "Setting LV size in MiB.";
        part.m_lvSize = QString::number(currentSize) + m_partComboBox->currentText();
    }

    m_patrinfo.push_back(part);
    m_partChartWidget->update();
    qDebug() << "LV information added and chart updated.";

    //新增逻辑卷后样式变化
    if (sumValue() >= m_totalSize - 0.01)
        m_isExceed = false;
    else
        m_isExceed = true;
    qDebug() << "Updated isExceed flag:" << m_isExceed;

    m_lstLVName.append(m_partNameEdit->text());
    setAddOrRemResult(m_isExceed);

    m_applyBtn->setEnabled(true);
    qDebug() << "Apply button enabled.";

    if (!m_partNameEdit->text().isEmpty()) {
        QByteArray byteArray = m_partNameEdit->text().toUtf8();
        if (byteArray.size() >= 128) {
            qDebug() << "LV name byte size >= 128, disabling add button.";
            m_addButton->setDisabled(true);
        }
    }
    qDebug() << "Add partition process finished.";
}

void CreateLVWidget::onRemovePartition()
{
    qDebug() << "Removing last LV from creation list";

    if (m_patrinfo.size() > 0) {
        m_patrinfo.pop_back();
        qDebug() << "Last partition info removed.";
    }

    m_addButton->setEnabled(true);

    if (m_sizeInfo.size() > 0) {
        m_sizeInfo.removeAt(m_sizeInfo.size() - 1);
        m_partName.removeAt(m_partName.size() - 1);
        qDebug() << "Last size info and part name removed.";
    }

    //绘制删除逻辑卷图形
    m_partChartWidget->transInfos(m_totalSize, m_sizeInfo);
    m_partChartWidget->update();
    qDebug() << "Chart updated after removal.";
    //删除逻辑卷后样式变化
    m_isExceed = true;
    qDebug() << "Updated isExceed flag after removal:" << m_isExceed;

    m_lstLVName.removeLast();
    setAddOrRemResult(m_isExceed);
    qDebug() << "Remove partition process finished.";
}

void CreateLVWidget::onApplyButton()
{
    qDebug()  << "Applying LV creation for VG:" << DMDbusHandler::instance()->getCurVGInfo().m_vgName;

    QList<LVAction> lstLVInfo;
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    QString userName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    userName.remove(0, 6);
    qDebug() << "User name extracted:" << userName;

    for (int i = 0; i < m_patrinfo.size(); i++) {
        LVAction info;
        info.m_vgName = vgInfo.m_vgName;
        info.m_lvName = m_patrinfo.at(i).m_lvName;
        info.m_lvFs = Utils::stringToFileSystemType(m_patrinfo.at(i).m_fstype);
        info.m_lvSize = m_patrinfo.at(i).m_lvSize;
        info.m_lvByteSize = m_patrinfo.at(i).m_lvByteSize;
        info.m_user = userName;
        info.m_lvAct = LVMAction::LVM_ACT_LV_CREATE;

        if (m_patrinfo.at(i).m_isEncryption) {
            qDebug() << "Partition is encrypted.";
            info.m_luksFlag = LUKSFlag::IS_CRYPT_LUKS;
            info.m_crypt = m_patrinfo.at(i).m_encryption;
            QStringList tokenList;
            if (!m_patrinfo.at(i).m_passwordHint.isEmpty()) {
                tokenList.append(m_patrinfo.at(i).m_passwordHint);
                qDebug() << "Password hint added to token list.";
            }

            info.m_tokenList = tokenList;
            info.m_decryptStr = m_patrinfo.at(i).m_password;
            info.m_dmName = m_patrinfo.at(i).m_dmName;
        } else {
            qDebug() << "Partition is not encrypted.";
            info.m_luksFlag = LUKSFlag::NOT_CRYPT_LUKS;
            info.m_crypt = CRYPT_CIPHER::NOT_CRYPT;
            info.m_tokenList = QStringList();
            info.m_decryptStr = "";
            info.m_dmName = "";
        }

        lstLVInfo.append(info);
        qDebug() << "LVAction info appended for LV:" << info.m_lvName;
    }

    if (lstLVInfo.size() > 0) {
        qDebug() << "Creating" << lstLVInfo.size() << "logical volumes on VG:" << vgInfo.m_vgName;
        DMDbusHandler::instance()->createLV(vgInfo.m_vgName, lstLVInfo);
        close();
    }
}

void CreateLVWidget::onRevertButton()
{
    qDebug() << "Reverting all LV creation changes";

    //复原,即清空
    m_patrinfo.clear();
    m_sizeInfo.clear();
    m_partName.clear();
    m_lstLVName.clear();
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    for (int i = 0; i < vgInfo.m_lvlist.count(); i++) {
        LVInfo info = vgInfo.m_lvlist.at(i);
        if ("unallocated" != info.m_lvName) {
            m_lstLVName.append(info.m_lvName);
        }
    }
    m_partNameEdit->setText(lvName());
    m_partFormateCombox->setCurrentText("ext4");
    m_partComboBox->setCurrentIndex(0);
    m_partChartWidget->transInfos(m_totalSize, m_sizeInfo);
    m_isExceed = true;

    if (m_sizeInfo.size() == 0) {
        qDebug() << "No LV info to create, apply button disabled.";
        m_applyBtn->setEnabled(false);
    }

    m_partChartWidget->update();
    m_slider->setValue(100);
    onSliderValueChanged(100);
    setSelectUnallocatesSpace();
    m_partChartWidget->transFlag(1, 0);
    setEnable(1, true);
}

void CreateLVWidget::onCancelButton()
{
    qDebug() << "Canceling LV creation dialog";

    close();
}

bool CreateLVWidget::event(QEvent *event)
{
    // 字体大小改变
    if (QEvent::ApplicationFontChange == event->type()) {
        qDebug() << event->type() << QApplication::font().pointSizeF() / 0.75;
        QFontMetrics fm(m_deviceNameLabel->font());
        m_deviceNameLabel->setFixedWidth(fm.boundingRect(tr("VG name:")).width() + 5);
#if QT_VERSION_MAJOR > 5
        m_emptyLabel->setFixedWidth(m_partFormateLabel->fontMetrics().boundingRect(tr("LV file system:")).width());
#else
        m_emptyLabel->setFixedWidth(m_partFormateLabel->fontMetrics().width(tr("LV file system:")));
#endif
        m_encryptionInfo->setFixedWidth(m_partFormateCombox->width());
        m_scrollArea->setFixedWidth(m_partFormateCombox->width());
        DWidget::event(event);
    }

    return DDialog::event(event);
}

void CreateLVWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Escape) {
        event->ignore();
    } else {
        DDialog::keyPressEvent(event);
    }
}
