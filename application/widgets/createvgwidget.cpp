// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "createvgwidget.h"
#include "common.h"
#include "messagebox.h"

#include <DLabel>
#include <DFontSizeManager>
#include <DWindowCloseButton>
#include <DMessageManager>

#include <QHBoxLayout>
#include <QVBoxLayout>

CreateVGWidget::CreateVGWidget(int operationType, QWidget *parent)
    : DDBase(parent)
    , m_curOperationType(operationType)
{
    qDebug() << "CreateVGWidget initialized with operationType:" << operationType;
    initUi();
    initConnection();
    updateData();
    qDebug() << "CreateVGWidget initialization completed";
}

void CreateVGWidget::initUi()
{
    qDebug() << "CreateVGWidget::initUi called.";
    setFixedSize(800, 538);

    m_curSeclectData.clear();
    m_oldSeclectData.clear();
    m_curDiskItemWidget = nullptr;
    m_waterLoadingWidget = nullptr;
    m_isResizeInit = true;
    m_curDeviceNameList = DMDbusHandler::instance()->getDeviceNameList();

    DPalette paletteTitle;
    QColor colorTitle("#000000");
    colorTitle.setAlphaF(0.9);
    paletteTitle.setColor(DPalette::ToolTipText, colorTitle);

    DPalette paletteSubTitle;
    QColor colorSubTitle("#000000");
    colorSubTitle.setAlphaF(0.7);
    paletteSubTitle.setColor(DPalette::ToolTipText, colorSubTitle);

    DPalette palette1;
    palette1.setColor(DPalette::Text, QColor("#526A7F"));

    DPalette palette2;
    palette2.setColor(DPalette::TextTitle, QColor("#001A2E"));

    DLabel *titleLabel = new DLabel(tr("Create volume group"));
    QFont fontTitle;
    fontTitle.setWeight(QFont::Medium);
    fontTitle.setFamily("Source Han Sans");
    fontTitle.setPixelSize(14);
    titleLabel->setFont(fontTitle);
    titleLabel->setPalette(paletteTitle);
    titleLabel->setAlignment(Qt::AlignCenter);

    if (m_curOperationType == OperationType::RESIZE) {
        qDebug() << "CreateVGWidget::initUi - Resize operation type detected";
        titleLabel->setText(tr("Resize"));
    }

    QFont fontSubTitle;
    fontSubTitle.setWeight(QFont::Normal);
    fontSubTitle.setFamily("Source Han Sans");
    fontSubTitle.setPixelSize(14);

    QFont font1;
    font1.setWeight(QFont::Normal);
    font1.setFamily("Source Han Sans");
    font1.setPixelSize(12);

    QFont font2;
    font2.setWeight(QFont::Medium);
    font2.setFamily("Source Han Sans");
    font2.setPixelSize(13);

    QFont font3;
    font3.setWeight(QFont::Normal);
    font3.setFamily("Source Han Sans");
    font3.setPixelSize(12);

    // 第一个页面
    DLabel *subTitleLabel = new DLabel(tr("Select disks or partitions to create a volume group and set its capacity"), this);
    subTitleLabel->setFont(fontSubTitle);
    subTitleLabel->setPalette(paletteSubTitle);
    subTitleLabel->setAlignment(Qt::AlignCenter);

    m_seclectedLabel = new DLabel(tr("Capacity selected: %1").arg("0GiB"), this);
    m_seclectedLabel->setFont(font1);
    m_seclectedLabel->setPalette(palette1);
    m_seclectedLabel->setAlignment(Qt::AlignCenter);

    DLabel *describeLabel = new DLabel(tr("The selected disks will be converted to dynamic disks, "
                                          "and you will not be able to start installed operating systems from the disks."), this);
    describeLabel->setFont(font1);
    describeLabel->setPalette(palette1);
    describeLabel->setWordWrap(true);
    describeLabel->setAlignment(Qt::AlignCenter);

    QPalette paletteBackground;
    paletteBackground.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    m_diskFrame = new DFrame(this);
    m_diskFrame->setFixedSize(370, 306);
    m_diskFrame->setPalette(paletteBackground);
    m_diskFrame->setBackgroundRole(QPalette::Base);
    m_diskFrame->setFrameShadow(QFrame::Plain);
    m_diskFrame->setFrameShape(QFrame::NoFrame);
    m_diskFrame->setContentsMargins(0, 0, 0, 0);
    m_diskFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_diskScrollArea = new DScrollArea;
    setScrollAreaAttribute(m_diskScrollArea, true, true);

    QVBoxLayout *diskScrollAreaLayout = new QVBoxLayout;
    diskScrollAreaLayout->addWidget(m_diskScrollArea);
    diskScrollAreaLayout->setSpacing(0);
    diskScrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_diskFrame->setLayout(diskScrollAreaLayout);

    m_partitionFrame = new DFrame(this);
    m_partitionFrame->setFixedSize(370, 306);
    m_partitionFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_partitionFrame->setPalette(paletteBackground);
    m_partitionFrame->setFrameShadow(QFrame::Plain);
    m_partitionFrame->setFrameShape(QFrame::NoFrame);

    m_partitionScrollArea = new DScrollArea;
    setScrollAreaAttribute(m_partitionScrollArea, true, true);

    QVBoxLayout *partitionScrollAreaLayout = new QVBoxLayout;
    partitionScrollAreaLayout->addWidget(m_partitionScrollArea);
    partitionScrollAreaLayout->setSpacing(0);
    partitionScrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_partitionFrame->setLayout(partitionScrollAreaLayout);

    QLabel *iconNoPartitionLabel = new QLabel(this);
    QIcon iconNoPartition = Common::getIcon("nopartition");
    iconNoPartitionLabel->setPixmap(iconNoPartition.pixmap(QSize(90, 90)));
    iconNoPartitionLabel->setAlignment(Qt::AlignCenter);
    iconNoPartitionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel *noPartitionLabel = new QLabel(this);
    noPartitionLabel->setText(tr("No partitions available"));
    noPartitionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noPartitionLabel->setAlignment(Qt::AlignCenter);
    noPartitionLabel->setFont(font1);
    noPartitionLabel->setPalette(palette2);

    QVBoxLayout *noPartitionLayout = new QVBoxLayout;
    noPartitionLayout->addStretch();
    noPartitionLayout->addWidget(iconNoPartitionLabel, 0, Qt::AlignCenter);
    noPartitionLayout->addSpacing(16);
    noPartitionLayout->addWidget(noPartitionLabel, 0, Qt::AlignCenter);
    noPartitionLayout->addStretch();
    noPartitionLayout->setSpacing(0);
    noPartitionLayout->setContentsMargins(0, 0, 0, 0);

    DFrame *noPartitionFrame = new DFrame(this);
//    noPartitionFrame->setFixedSize(370, 305);
    noPartitionFrame->setBackgroundRole(DPalette::ItemBackground);
    noPartitionFrame->setLineWidth(0);
    noPartitionFrame->setLayout(noPartitionLayout);

    QVBoxLayout *noPartitionMainLayout = new QVBoxLayout;
    noPartitionMainLayout->addWidget(noPartitionFrame);
    noPartitionMainLayout->setSpacing(0);
    noPartitionMainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *noPartitionWidget = new QWidget(this);
    noPartitionWidget->setLayout(noPartitionMainLayout);

    m_partitionStackedWidget = new DStackedWidget(this);
    m_partitionStackedWidget->addWidget(m_partitionFrame);
    m_partitionStackedWidget->addWidget(noPartitionWidget);
    m_partitionStackedWidget->setCurrentIndex(0);

    DFrame *line = new DFrame(this);
    paletteBackground.setColor(QPalette::Base, QColor(0, 0, 0, 10));
    line->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setPalette(paletteBackground);
    line->setFixedSize(2, 306);

    QHBoxLayout *selectLayout = new QHBoxLayout(this);
    selectLayout->addWidget(m_diskFrame);
    selectLayout->addSpacing(10);
    selectLayout->addWidget(line);
    selectLayout->addSpacing(10);
    selectLayout->addWidget(m_partitionStackedWidget);
    selectLayout->setSpacing(0);
    selectLayout->setContentsMargins(10, 0, 10, 0);

    QVBoxLayout *firstCenterLayout = new QVBoxLayout;
    firstCenterLayout->addLayout(selectLayout);
    firstCenterLayout->addSpacing(10);
    firstCenterLayout->addWidget(m_seclectedLabel);
    firstCenterLayout->addSpacing(10);
    firstCenterLayout->addWidget(describeLabel);
    firstCenterLayout->addSpacing(8);
    firstCenterLayout->setSpacing(0);
    firstCenterLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *centerWidget = new QWidget(this);
    centerWidget->setLayout(firstCenterLayout);

    QLabel *iconNoDiskLabel = new QLabel(this);
    QIcon iconNoDisk = Common::getIcon("nodisk");
    iconNoDiskLabel->setPixmap(iconNoDisk.pixmap(QSize(136, 136)));
    iconNoDiskLabel->setAlignment(Qt::AlignCenter);
    iconNoDiskLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel *noDiskLabel = new QLabel(this);
    noDiskLabel->setText(tr("No disks or partitions available"));
    noDiskLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noDiskLabel->setAlignment(Qt::AlignCenter);
    noDiskLabel->setFont(font1);
    noDiskLabel->setPalette(palette2);

    QVBoxLayout *noDiskLayout = new QVBoxLayout;
    noDiskLayout->addStretch();
    noDiskLayout->addWidget(iconNoDiskLabel, 0, Qt::AlignCenter);
    noDiskLayout->addSpacing(10);
    noDiskLayout->addWidget(noDiskLabel, 0, Qt::AlignCenter);
    noDiskLayout->addStretch();
    noDiskLayout->setSpacing(0);
    noDiskLayout->setContentsMargins(0, 0, 0, 0);

    DFrame *noDiskFrame = new DFrame(this);
    noDiskFrame->setBackgroundRole(DPalette::ItemBackground);
    noDiskFrame->setLineWidth(0);
    noDiskFrame->setLayout(noDiskLayout);

    QVBoxLayout *noDiskMainLayout = new QVBoxLayout;
    noDiskMainLayout->addWidget(noDiskFrame);
    noDiskMainLayout->setSpacing(0);
    noDiskMainLayout->setContentsMargins(10, 0, 10, 20);

    QWidget *noDiskWidget = new QWidget(this);
    noDiskWidget->setLayout(noDiskMainLayout);

    m_firstCenterStackedWidget = new DStackedWidget(this);
    m_firstCenterStackedWidget->addWidget(centerWidget);
    m_firstCenterStackedWidget->addWidget(noDiskWidget);
    m_firstCenterStackedWidget->setCurrentIndex(0);

    m_firstCancelButton = new DPushButton(this);
    m_firstCancelButton->setText(tr("Cancel", "button"));
    m_firstCancelButton->setAccessibleName("firstCancel");
    m_nextButton = new DSuggestButton(this);
    m_nextButton->setText(tr("Next"));
    m_nextButton->setAccessibleName("next");
    m_nextButton->setDisabled(true);

    QHBoxLayout *firstButtonLayout = new QHBoxLayout;
    firstButtonLayout->addSpacing(87);
    firstButtonLayout->addWidget(m_firstCancelButton);
    firstButtonLayout->addSpacing(10);
    firstButtonLayout->addWidget(m_nextButton);
    firstButtonLayout->addSpacing(87);
    firstButtonLayout->setSpacing(0);
    firstButtonLayout->setContentsMargins(0, 0, 0, 10);

    QVBoxLayout *firstLayout = new QVBoxLayout;
    firstLayout->addWidget(subTitleLabel);
    firstLayout->addSpacing(10);
    firstLayout->addWidget(m_firstCenterStackedWidget);
    firstLayout->addLayout(firstButtonLayout);
    firstLayout->setSpacing(0);
    firstLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *firstWidget = new QWidget(this);
    firstWidget->setLayout(firstLayout);

    // 第二个页面
    DLabel *selectedTitleLabel = new DLabel(tr("Selected disks/partitions"), this);
    selectedTitleLabel->setFont(fontSubTitle);
    selectedTitleLabel->setPalette(paletteSubTitle);
    selectedTitleLabel->setAlignment(Qt::AlignBottom);

    m_addButton = new DPushButton(this);
    QIcon iconAdd = Common::getIcon("add");
    m_addButton->setIcon(iconAdd);
    m_addButton->setAccessibleName("addButton");
    m_addButton->setFixedSize(QSize(36, 36));
    m_addButton->setIconSize(QSize(16, 16));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(selectedTitleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_addButton);
    topLayout->setSpacing(0);
    topLayout->setContentsMargins(10, 0, 0, 0);

    m_selectedFrame = new DFrame(this);
    paletteBackground.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    m_selectedFrame->setFixedSize(760, 277);
    m_selectedFrame->setPalette(paletteBackground);
    m_selectedFrame->setFrameShadow(QFrame::Plain);
    m_selectedFrame->setFrameShape(QFrame::NoFrame);
    m_selectedFrame->setContentsMargins(0, 0, 0, 0);
    m_selectedFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_selectedScrollArea = new DScrollArea;
    setScrollAreaAttribute(m_selectedScrollArea, true, true);

    QVBoxLayout *selectedScrollAreaLayout = new QVBoxLayout;
    selectedScrollAreaLayout->addWidget(m_selectedScrollArea);
    selectedScrollAreaLayout->setSpacing(0);
    selectedScrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    m_selectedFrame->setLayout(selectedScrollAreaLayout);

    DLabel *selectSpaceLabel = new DLabel(tr("Set VG capacity"), this);
    selectSpaceLabel->setFont(font2);
    selectSpaceLabel->setPalette(palette2);
    //selectSpaceLabel->setAlignment(Qt::AlignCenter);

    DLabel *selectSpaceLabel2 = new DLabel(tr("Auto adjusted to integral multiples of 4 MiB"), this); //自动调整为4MiB的倍数
    selectSpaceLabel2->setFont(font3);
    selectSpaceLabel2->setPalette(palette1);
    selectSpaceLabel2->setAlignment(Qt::AlignCenter);

//    m_selectSpaceLabel = new DLabel("(1-100GiB)", this);
//    m_selectSpaceLabel->setFont(font1);
//    m_selectSpaceLabel->setPalette(palette1);
//    m_selectSpaceLabel->setAccessibleName("range");
//    m_selectSpaceLabel->setAlignment(Qt::AlignCenter);

    m_selectSpaceLineEdit = new DLineEdit(this);
    m_selectSpaceLineEdit->setFixedSize(182, 36);
    m_selectSpaceLineEdit->setAccessibleName("vgSize");
    m_selectSpaceLineEdit->lineEdit()->setPlaceholderText("1-100");
    m_selectSpaceLineEdit->setText("100");

#if QT_VERSION_MAJOR > 5
    QRegularExpression reg("^[0-9]+(.[0-9]{1,4})?$");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(reg, this);
#else
    QRegExp reg("^[0-9]+(.[0-9]{1,4})?$");
    QRegExpValidator *validator = new QRegExpValidator(reg, this);
#endif
    m_selectSpaceLineEdit->lineEdit()->setValidator(validator);

    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        m_selectSpaceLineEdit->setStyleSheet("background:rgba(0, 0, 0, 8)");
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        m_selectSpaceLineEdit->setStyleSheet("background:rgba(255, 255, 255, 8)");
    }

    m_selectSpaceComboBox = new DComboBox(this);
    m_selectSpaceComboBox->setFixedSize(100, 36);
    m_selectSpaceComboBox->addItem("MiB");
    m_selectSpaceComboBox->addItem("GiB");
    m_selectSpaceComboBox->addItem("TiB");
    m_selectSpaceComboBox->setCurrentText("GiB");
    m_selectSpaceComboBox->setAccessibleName("vgUnit");

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(selectSpaceLabel);
    vBoxLayout->addSpacing(5);
    vBoxLayout->addWidget(selectSpaceLabel2);

    QHBoxLayout *selectSpaceLayout = new QHBoxLayout;
    selectSpaceLayout->addLayout(vBoxLayout);
    selectSpaceLayout->addSpacing(5);
    //selectSpaceLayout->addWidget(m_selectSpaceLabel);
    selectSpaceLayout->addStretch();
    selectSpaceLayout->addWidget(m_selectSpaceLineEdit);
    selectSpaceLayout->addSpacing(10);
    selectSpaceLayout->addWidget(m_selectSpaceComboBox);
    selectSpaceLayout->setSpacing(0);
    selectSpaceLayout->setContentsMargins(10, 6, 10, 6);

    DFrame *checkSpaceFrame = new DFrame(this);
    checkSpaceFrame->setFixedSize(760, 48);
    checkSpaceFrame->setLineWidth(0);
    checkSpaceFrame->setBackgroundRole(DPalette::ItemBackground);
    checkSpaceFrame->setLayout(selectSpaceLayout);

    m_selectedSpaceLabel = new DLabel(tr("Capacity selected: %1"), this);
    m_selectedSpaceLabel->setFont(font1);
    m_selectedSpaceLabel->setPalette(palette1);
    m_selectedSpaceLabel->setAccessibleName("selectedSpace");
    m_selectedSpaceLabel->setAlignment(Qt::AlignCenter | Qt::AlignBottom);

    m_selectSpaceStackedWidget = new DStackedWidget(this);
    m_selectSpaceStackedWidget->addWidget(checkSpaceFrame);
    m_selectSpaceStackedWidget->addWidget(m_selectedSpaceLabel);
    m_selectSpaceStackedWidget->setCurrentIndex(0);

    QVBoxLayout *secondCenterLayout = new QVBoxLayout;
    secondCenterLayout->addLayout(topLayout);
    secondCenterLayout->addSpacing(10);
    secondCenterLayout->addWidget(m_selectedFrame);
    secondCenterLayout->addSpacing(15);
    secondCenterLayout->addWidget(m_selectSpaceStackedWidget);
    secondCenterLayout->setSpacing(0);
    secondCenterLayout->setContentsMargins(10, 2, 10, 0);

    QWidget *secondCenterWidget = new QWidget(this);
    secondCenterWidget->setLayout(secondCenterLayout);

    QLabel *iconNoInfoLabel = new QLabel(this);
    QIcon iconNoInfo = Common::getIcon("nodisk");
    iconNoInfoLabel->setPixmap(iconNoInfo.pixmap(QSize(136, 136)));
    iconNoInfoLabel->setAlignment(Qt::AlignCenter);
    iconNoInfoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel *noInfoLabel = new QLabel(this);
    noInfoLabel->setText(tr("Choose one disk or partition at least"));
    noInfoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    noInfoLabel->setAlignment(Qt::AlignCenter);
    noInfoLabel->setFont(font1);
    noInfoLabel->setPalette(palette2);

    QVBoxLayout *noInfoLayout = new QVBoxLayout;
    noInfoLayout->addStretch();
    noInfoLayout->addWidget(iconNoInfoLabel, 0, Qt::AlignCenter);
    noInfoLayout->addSpacing(10);
    noInfoLayout->addWidget(noInfoLabel, 0, Qt::AlignCenter);
    noInfoLayout->addStretch();
    noInfoLayout->setSpacing(0);
    noInfoLayout->setContentsMargins(0, 0, 0, 0);

    DFrame *noInfoFrame = new DFrame(this);
    noInfoFrame->setBackgroundRole(DPalette::ItemBackground);
    noInfoFrame->setLineWidth(0);
    noInfoFrame->setLayout(noInfoLayout);

    QVBoxLayout *noInfoMainLayout = new QVBoxLayout;
    noInfoMainLayout->addWidget(noInfoFrame);
    noInfoMainLayout->setSpacing(0);
    noInfoMainLayout->setContentsMargins(10, 6, 10, 10);

    QWidget *noInfoWidget = new QWidget(this);
    noInfoWidget->setLayout(noInfoMainLayout);

    m_selectedStackedWidget = new DStackedWidget(this);
    m_selectedStackedWidget->addWidget(secondCenterWidget);
    m_selectedStackedWidget->addWidget(noInfoWidget);
    m_selectedStackedWidget->setCurrentIndex(1);

    m_secondCancelButton = new DPushButton(this);
    m_secondCancelButton->setText(tr("Cancel", "button"));
    m_secondCancelButton->setAccessibleName("secondCancel");
    m_previousButton = new DPushButton(this);
    m_previousButton->setText(tr("Previous"));
    m_previousButton->setAccessibleName("previous");
    m_doneButton = new DSuggestButton(this);
    m_doneButton->setText(tr("Done", "button"));
    m_doneButton->setAccessibleName("done");

    QHBoxLayout *secondButtonLayout = new QHBoxLayout;
    secondButtonLayout->addSpacing(110);
    secondButtonLayout->addWidget(m_secondCancelButton);
    secondButtonLayout->addSpacing(10);
    secondButtonLayout->addWidget(m_previousButton);
    secondButtonLayout->addSpacing(10);
    secondButtonLayout->addWidget(m_doneButton);
    secondButtonLayout->addSpacing(110);
    secondButtonLayout->setSpacing(0);
    secondButtonLayout->setContentsMargins(0, 0, 0, 10);

    QVBoxLayout *secondLayout = new QVBoxLayout;
    secondLayout->addWidget(m_selectedStackedWidget);
    secondLayout->addSpacing(10);
    secondLayout->addLayout(secondButtonLayout);
    secondLayout->setSpacing(0);
    secondLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *secondWidget = new QWidget(this);
    secondWidget->setLayout(secondLayout);

    m_loadingWidget = new QWidget(this);

    m_stackedWidget = new DStackedWidget(this);
    m_stackedWidget->addWidget(firstWidget);
    m_stackedWidget->addWidget(secondWidget);
    m_stackedWidget->addWidget(m_loadingWidget);
    m_stackedWidget->setCurrentIndex(0);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(4);
    mainLayout->addWidget(m_stackedWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    qDebug() << "CreateVGWidget initialization completed";
}

void CreateVGWidget::initConnection()
{
    qDebug() << "CreateVGWidget::initConnection called.";
    connect(m_nextButton, &DSuggestButton::clicked, this, &CreateVGWidget::onNextButtonClicked);
    connect(m_addButton, &DPushButton::clicked, this, &CreateVGWidget::onPreviousButtonClicked);
    connect(m_previousButton, &DPushButton::clicked, this, &CreateVGWidget::onPreviousButtonClicked);
    connect(m_doneButton, &DPushButton::clicked, this, &CreateVGWidget::onDoneButtonClicked);
    connect(m_firstCancelButton, &DPushButton::clicked, this, &CreateVGWidget::onCancelButtonClicked);
    connect(m_secondCancelButton, &DPushButton::clicked, this, &CreateVGWidget::onCancelButtonClicked);
    connect(m_selectSpaceComboBox, static_cast<void (DComboBox::*)(const int)>(&DComboBox::currentIndexChanged),
            this, &CreateVGWidget::onCurrentIndexChanged);
    connect(m_selectSpaceLineEdit, &DLineEdit::textChanged, this, &CreateVGWidget::onTextChanged);
    connect(DMDbusHandler::instance(), &DMDbusHandler::vgCreateMessage, this, &CreateVGWidget::onVGCreateMessage);
    connect(DMDbusHandler::instance(), &DMDbusHandler::updateDeviceInfo, this, &CreateVGWidget::onUpdateUsb);
    qDebug() << "All connections initialized.";
}

void CreateVGWidget::setScrollAreaAttribute(DScrollArea *scrollArea, bool hScrollBarHidden, bool vScrollBarHidden)
{
    qDebug() << "CreateVGWidget::setScrollAreaAttribute called. hScrollBarHidden:" << hScrollBarHidden << ", vScrollBarHidden:" << vScrollBarHidden;
    scrollArea->setFrameShadow(QFrame::Plain);
    scrollArea->setFrameShape(QFrame::NoFrame);

    if (hScrollBarHidden) {
        qDebug() << "Hiding horizontal scroll bar.";
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
    }

    if (vScrollBarHidden) {
        qDebug() << "Hiding vertical scroll bar.";
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖向滚动条
    }
    qDebug() << "Scroll area attributes set.";
}

bool CreateVGWidget::judgeDataEquality(const PVInfoData &data1, const PVInfoData &data2)
{
    qDebug() << "CreateVGWidget::judgeDataEquality (PVInfoData) called.";
    return data1.m_partitionPath == data2.m_partitionPath
           && data1.m_sectorStart == data2.m_sectorStart
           && data1.m_sectorEnd == data2.m_sectorEnd
           && data1.m_diskPath == data2.m_diskPath;
}

bool CreateVGWidget::judgeDataEquality(const PartitionInfo &partInfo, const PVInfoData &pvInfo)
{
    qDebug() << "CreateVGWidget::judgeDataEquality (PartitionInfo) called.";
    return partInfo.m_path == pvInfo.m_partitionPath
           && partInfo.m_sectorStart == pvInfo.m_sectorStart
           && partInfo.m_sectorEnd == pvInfo.m_sectorEnd
           && partInfo.m_devicePath == pvInfo.m_diskPath;
}

void CreateVGWidget::showLoadingWidget(const QString &vgName)
{
    qDebug() << "CreateVGWidget::showLoadingWidget called. VG Name:" << vgName;
    m_stackedWidget->setCurrentIndex(2);
    DPalette palette1;
    palette1.setColor(DPalette::TextTitle, QColor("#001A2E"));

    DPalette palette2;
    palette2.setColor(DPalette::ButtonText, QColor("#414D68"));

    QFont font1;
    font1.setWeight(QFont::Normal);
    font1.setFamily("Source Han Sans");
    font1.setPixelSize(12);

    QFont font2;
    font2.setWeight(QFont::Medium);
    font2.setFamily("Source Han Sans");
    font2.setPixelSize(14);

    m_waterLoadingWidget = new WaterLoadingWidget(this);
    m_waterLoadingWidget->setFixedSize(128, 128);

    QLabel *waitLable = new QLabel(this);
    waitLable->setAlignment(Qt::AlignCenter);
    waitLable->setFont(font1);
    waitLable->setPalette(palette1);

    if (m_curOperationType == OperationType::RESIZE) {
        qDebug() << "Operation type is RESIZE, setting wait label text to 'Resizing space...'.";
        waitLable->setText(tr("Resizing space..."));
    } else {
        qDebug() << "Operation type is not RESIZE, setting wait label text to 'Creating...'.";
        waitLable->setText(tr("Creating..."));
    }

    QVBoxLayout *layout = new QVBoxLayout;
    for (int i = 0; i < m_curSeclectData.count(); i += 2) {
        QLabel *label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        label->setFont(font2);
        label->setPalette(palette2);

        if (i == (m_curSeclectData.count() - 1)) {
            qDebug() << "Displaying single selected partition path:" << m_curSeclectData.at(i).m_partitionPath;
            label->setText(QString("%1").arg(m_curSeclectData.at(i).m_partitionPath));
        } else {
            QString path1;
            QString path2;
            PVInfoData pvInfoData1 = m_curSeclectData.at(i);
            PVInfoData pvInfoData2 = m_curSeclectData.at(i + 1);

            if (pvInfoData1.m_disktype == "unrecognized") {
                qDebug() << "PVInfoData1 disktype is unrecognized, using diskPath.";
                path1 = pvInfoData1.m_diskPath;
            } else {
                qDebug() << "PVInfoData1 disktype is recognized, using partitionPath.";
                path1 = pvInfoData1.m_partitionPath;
            }

            if (pvInfoData2.m_disktype == "unrecognized") {
                qDebug() << "PVInfoData2 disktype is unrecognized, using diskPath.";
                path2 = pvInfoData2.m_diskPath;
            } else {
                qDebug() << "PVInfoData2 disktype is recognized, using partitionPath.";
                path2 = pvInfoData2.m_partitionPath;
            }

            if (i == (m_curSeclectData.count() - 2)) {
                qDebug() << "Displaying last pair of selected paths:" << path1 << "," << path2;
                label->setText(QString("%1, %2").arg(path1).arg(path2));
            } else {
                qDebug() << "Displaying selected paths:" << path1 << "," << path2;
                label->setText(QString("%1, %2, ").arg(path1).arg(path2));
            }
        }

        layout->addWidget(label, 0, Qt::AlignCenter);
    }

    layout->addStretch();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    DFrame *frame = new DFrame;
    DPalette paletteBase;
    paletteBase.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    frame->setPalette(paletteBase);
    frame->setFrameShadow(QFrame::Plain);
    frame->setFrameShape(QFrame::NoFrame);

    QLabel *selectLable = new QLabel(this);
    selectLable->setText(tr("Selected disks and partitions:"));
    selectLable->setAlignment(Qt::AlignCenter);
    selectLable->setFont(font2);
    selectLable->setPalette(palette2);

    bool isShowScrollArea = false;
    int count = m_curSeclectData.count() / 2 + m_curSeclectData.count() % 2;
    if (count > 5) {
        qDebug() << "Count of selected data is greater than 5, showing scroll area.";
        QWidget *widget = new QWidget;
        widget->setLayout(layout);
        widget->setFixedSize(600, count * 23 - 8);

        DScrollArea *scrollArea = new DScrollArea;
        setScrollAreaAttribute(scrollArea, true, false);
        scrollArea->setWidget(widget);

        QVBoxLayout *scrollAreaLayout = new QVBoxLayout;
        scrollAreaLayout->addWidget(selectLable, 0, Qt::AlignCenter);
        scrollAreaLayout->addSpacing(5);
        scrollAreaLayout->addWidget(scrollArea);
        scrollAreaLayout->setSpacing(0);
        scrollAreaLayout->setContentsMargins(0, 0, 0, 0);

        frame->setFixedSize(600, 124);
        frame->setLayout(scrollAreaLayout);

    } else {
        qDebug() << "Count of selected data is 5 or less, not showing scroll area.";
        layout->insertWidget(0, selectLable, 0, Qt::AlignCenter);

        frame->setFixedSize(600, (count + 1) * 23);
        frame->setLayout(layout);
    }

    if (count >= 5) {
        isShowScrollArea = true;
        qDebug() << "Setting isShowScrollArea to true as count >= 5.";
    }

    QLabel *vgSizeLabel = new QLabel(this);
    vgSizeLabel->setAlignment(Qt::AlignCenter);
    vgSizeLabel->setFont(font2);
    vgSizeLabel->setPalette(palette2);

    if (m_selectSpaceStackedWidget->currentIndex() == 1) {
        qDebug() << "Setting VG capacity label from min size.";
        vgSizeLabel->setText(tr("VG capacity: %1").arg(QString::number(m_minSize, 'f', 2)) + m_selectSpaceComboBox->currentText());
    } else {
        qDebug() << "Setting VG capacity label from line edit.";
        vgSizeLabel->setText(tr("VG capacity: %1").arg(m_selectSpaceLineEdit->text() + m_selectSpaceComboBox->currentText()));
    }

    QLabel *vgNameLabel = new QLabel(this);
    vgNameLabel->setAlignment(Qt::AlignCenter);
    vgNameLabel->setText(tr("VG name: %1").arg(vgName));
    vgNameLabel->setFont(font2);
    vgNameLabel->setPalette(palette2);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(26);
    mainLayout->addWidget(m_waterLoadingWidget, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(waitLable, 0, Qt::AlignCenter);
    if (isShowScrollArea) {
        qDebug() << "Adding spacing for scroll area.";
        mainLayout->addSpacing(30);
    } else {
        qDebug() << "Adding spacing without scroll area.";
        mainLayout->addSpacing(40);
    }
    mainLayout->addWidget(frame, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(vgSizeLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(vgNameLabel, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_loadingWidget->setLayout(mainLayout);

    DWindowCloseButton *button = findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    if (button != nullptr) {
        qDebug() << "Disabling and hiding window close button.";
        button->setDisabled(true);
        button->hide();
    }

    m_waterLoadingWidget->setStartTime(1000);
    qDebug() << "Loading widget shown.";
}

QString CreateVGWidget::getVGName()
{
    qDebug() << "CreateVGWidget::getVGName called.";
    LVMInfo lvmInfo = DMDbusHandler::instance()->probLVMInfo();
    QList<QString> lvmNameList = lvmInfo.m_vgInfo.keys();
    QString name = QString("vg%1").arg(lvmNameList.count() + 1, 2, 10, QLatin1Char('0'));
    for (int i = 0; i <= lvmNameList.count(); ++i) {
        name = QString("vg%1").arg(i + 1, 2, 10, QLatin1Char('0'));
        if (-1 == lvmNameList.indexOf(name)) {
            qDebug() << "Generated unique VG name:" << name;
            break;
        }
    }
    qDebug() << "Returning VG name:" << name;
    return name;
}

set<PVData> CreateVGWidget::getCurSelectPVData()
{
    qDebug() << "CreateVGWidget::getCurSelectPVData called.";
    set<PVData> pvDataSet;
    for (int i = 0; i < m_curSeclectData.count(); ++i) {
        PVInfoData infoData = m_curSeclectData.at(i);

        PVData pvData;
        pvData.m_diskPath = infoData.m_diskPath;
        pvData.m_startSector = infoData.m_sectorStart;
        pvData.m_endSector = infoData.m_sectorEnd;
        pvData.m_sectorSize = static_cast<int>(infoData.m_sectorSize);
        pvData.m_pvAct = LVMAction::LVM_ACT_PV;

        if (infoData.m_disktype == "unrecognized") {
            qDebug() << "PVInfoData disktype is unrecognized for path:" << infoData.m_diskPath;
            pvData.m_devicePath = infoData.m_diskPath;
            pvData.m_type = DevType::DEV_DISK;
        } else {
            if (infoData.m_partitionPath == "unallocated") {
                qDebug() << "Partition path is unallocated for disk:" << infoData.m_diskPath;
                pvData.m_devicePath = infoData.m_diskPath;
                pvData.m_type = DevType::DEV_UNALLOCATED_PARTITION;
            } else {
                qDebug() << "Partition path is recognized for partition:" << infoData.m_partitionPath;
                pvData.m_devicePath = infoData.m_partitionPath;
                pvData.m_type = DevType::DEV_PARTITION;
            }
        }

        pvDataSet.insert(pvData);
        qDebug() << "Inserted PVData for device:" << pvData.m_devicePath;
    }
    qDebug() << "Finished getting current selected PV data. Count:" << pvDataSet.size();
    return pvDataSet;
}

void CreateVGWidget::onNextButtonClicked()
{
    qDebug() << "Next button clicked, switching to step 2";
    m_stackedWidget->setCurrentIndex(1);
    updateSelectedData();
    qDebug() << "Step 2 UI updated with selected data";
}

void CreateVGWidget::onPreviousButtonClicked()
{
    qDebug() << "Previous button clicked, switching back to step 1";
    m_stackedWidget->setCurrentIndex(0);
    updateData();
    qDebug() << "Step 1 UI refreshed";
}

void CreateVGWidget::onDoneButtonClicked()
{
    qDebug() << "Done button clicked, starting VG operation";
    
    if (m_selectSpaceStackedWidget->currentIndex() == 0) {
        qDebug() << "Validating VG size input...";
        double curSize = QString::number(m_selectSpaceLineEdit->text().toDouble(), 'f', 2).toDouble();
        double minSize = QString::number(m_minSize, 'f', 2).toDouble();
        double maxSize = QString::number(m_maxSize, 'f', 2).toDouble();

        if (curSize < minSize) {
            qDebug() << "Current size" << curSize << "is less than minimum size" << minSize;
            m_selectSpaceLineEdit->setAlertMessageAlignment(Qt::AlignTop);
            m_selectSpaceLineEdit->showAlertMessage(tr("No less than the used capacity please"), m_mainFrame); // 不得低于已用空间
            m_selectSpaceLineEdit->setAlert(true);
            return;
        } else if (curSize > maxSize) {
            qDebug() << "Current size" << curSize << "is greater than maximum size" << maxSize;
            m_selectSpaceLineEdit->setAlertMessageAlignment(Qt::AlignTop);
            m_selectSpaceLineEdit->showAlertMessage(tr("No more than the maximum capacity please"), m_mainFrame); // 超出最大可用空间
            m_selectSpaceLineEdit->setAlert(true);
            return;
        }
        qDebug() << "VG size input validated successfully.";
    }

    if (m_curOperationType == OperationType::RESIZE) {
        qDebug() << "Operation type is RESIZE.";
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        set<PVData> pvDataSet = getCurSelectPVData();
        bool bigDataMove;
        QStringList realDelPvList;
        bool ret = adjudicationPVMove(vgInfo, pvDataSet, bigDataMove, realDelPvList);
        qDebug() << "Adjudication PV move result:" << ret << ", bigDataMove:" << bigDataMove;
        if (ret) {
            if (bigDataMove) {
                qDebug() << "Big data move detected, showing warning box for devices:" << realDelPvList;
                MessageBox warningBox(this);
                warningBox.setObjectName("messageBox");
                warningBox.setAccessibleName("removeWarningWidget");
                // XXX（磁盘名称）中存在大量数据，预计将耗费较长时间进行数据迁移，是否继续？  继续
                QString text1 = tr("A lot of data exists on %1, ").arg(realDelPvList.join(','));
                QString text2 = tr("which may take a long time to back it up.");
                QString text3 = tr("Do you want to continue?");
                warningBox.setWarings(text1 + "\n" + text2 + "\n" + text3, "", tr("Continue"), "continue",
                                      tr("Cancel"), "messageBoxCancelButton");
                if (warningBox.exec() == DDialog::Rejected) {
                    qDebug() << "User rejected big data move, returning.";
                    return;
                }
                qDebug() << "User accepted big data move.";
            }
        }
    }

    QList<PVData> pvDataList;
    for (int i = 0; i < m_curSeclectData.count(); ++i) {
        PVInfoData infoData = m_curSeclectData.at(i);
        PVData pvData;
        pvData.m_diskPath = infoData.m_diskPath;
        pvData.m_startSector = infoData.m_sectorStart;
        pvData.m_endSector = infoData.m_sectorEnd;
        pvData.m_sectorSize = static_cast<int>(infoData.m_sectorSize);
        pvData.m_pvAct = LVMAction::LVM_ACT_ADDPV;

        if (infoData.m_disktype == "unrecognized") {
            qDebug() << "PVInfoData disktype is unrecognized for path:" << infoData.m_diskPath << ", setting DEV_DISK.";
            pvData.m_devicePath = infoData.m_diskPath;
            pvData.m_type = DevType::DEV_DISK;
        } else {
            if (infoData.m_partitionPath == "unallocated") {
                qDebug() << "Partition path is unallocated for disk:" << infoData.m_diskPath << ", setting DEV_UNALLOCATED_PARTITION.";
                pvData.m_devicePath = infoData.m_diskPath;
                pvData.m_type = DevType::DEV_UNALLOCATED_PARTITION;
            } else {
                qDebug() << "Partition path is recognized for partition:" << infoData.m_partitionPath << ", setting DEV_PARTITION.";
                pvData.m_devicePath = infoData.m_partitionPath;
                pvData.m_type = DevType::DEV_PARTITION;
            }
        }

        if(infoData.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS){
            qDebug() << "LUKS encrypted partition detected for device:" << pvData.m_devicePath;
            LUKS_INFO luks = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(pvData.m_devicePath);
            if (luks.isDecrypt) {
                qDebug() << "LUKS device is decrypted, using mapper path:" << luks.m_mapper.m_dmPath;
                pvData.m_devicePath = luks.m_mapper.m_dmPath;
            }
        }

        pvDataList.append(pvData);
        qDebug() << "Added PVData to list for device:" << pvData.m_devicePath;
    }

    QString vgName = getVGName();
    if (m_curOperationType == OperationType::RESIZE) {
        qDebug() << "Operation type is RESIZE, calling resizeVG for VG:" << vgName;
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        vgName = vgInfo.m_vgName;

        DMDbusHandler::instance()->resizeVG(vgName, pvDataList, m_curSize);
    } else {
        qInfo() << "Creating new VG:" << vgName << "with" << pvDataList.size() << "PVs";
        DMDbusHandler::instance()->createVG(vgName, pvDataList, m_curSize);
        qDebug() << "CreateVG called for VG:" << vgName;
    }

    showLoadingWidget(vgName);
    qDebug() << "Done button clicked process finished.";
}

void CreateVGWidget::onCancelButtonClicked()
{
    qDebug() << "CreateVGWidget::onCancelButtonClicked called. Closing dialog.";
    close();
}

void CreateVGWidget::updateData()
{
    qDebug() << "CreateVGWidget::updateData called.";
    QList<DeviceInfo> lstDeviceInfo;
    lstDeviceInfo.clear();

    if (m_curOperationType == OperationType::RESIZE) {
        qDebug() << "Operation type is RESIZE, getting resizeAvailableDiskData.";
        lstDeviceInfo = resizeAvailableDiskData();
    } else {
        qDebug() << "Operation type is CREATE, getting createAvailableDiskData.";
        lstDeviceInfo = createAvailableDiskData();
    }
    qDebug() << "Available device info list count:" << lstDeviceInfo.count();

    if (lstDeviceInfo.count() == 0) {
        qDebug() << "No device info available, showing no disk widget and disabling next button.";
        m_firstCenterStackedWidget->setCurrentIndex(1);
        m_nextButton->setDisabled(true);
    } else {
        qDebug() << "Device info available, updating disk and partition UI.";
        QWidget *diskWidget = m_diskScrollArea->findChild<QWidget *>("diskScrollArea");
        Utils::deletePoint(diskWidget);

        m_curDiskItemWidget = nullptr;

        QWidget *partWidget = m_partitionScrollArea->findChild<QWidget *>("partScrollArea");
        Utils::deletePoint(partWidget);

        m_firstCenterStackedWidget->setCurrentIndex(0);
        QVBoxLayout *diskLayout = new QVBoxLayout;
        for (int i = 0; i < lstDeviceInfo.count(); ++i) {
            DeviceInfo info = lstDeviceInfo.at(i);
            QString diskSize = Utils::formatSize(info.m_length, info.m_sectorSize);

            PVInfoData diskInfoData;
            diskInfoData.m_level = DMDbusHandler::DISK;
            diskInfoData.m_disktype = info.m_disktype;
            diskInfoData.m_diskPath = info.m_path;
            diskInfoData.m_diskSize = diskSize;
            diskInfoData.m_partitionPath = "";
            diskInfoData.m_partitionSize = "";
            diskInfoData.m_sectorSize = info.m_sectorSize;
            diskInfoData.m_sectorStart = 0;
            diskInfoData.m_sectorEnd = 0;
            diskInfoData.m_selectStatus = Qt::CheckState::Unchecked;
            diskInfoData.m_lvmDevType = DevType::DEV_DISK;
            diskInfoData.m_luksFlag = info.m_luksFlag;

            QList<PVInfoData> lstPVInfoData;
            lstPVInfoData.clear();
            if (info.m_partition.size() == 1) {
                PartitionInfo partitionInfo = info.m_partition.at(0);
                QString partitionSize = Utils::formatSize(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                          partitionInfo.m_sectorSize);
                if (info.m_disktype != "unrecognized") {
                    diskInfoData.m_sectorStart = partitionInfo.m_sectorStart;
                    diskInfoData.m_sectorEnd = partitionInfo.m_sectorEnd;

                    PVInfoData partInfoData;
                    partInfoData.m_level = DMDbusHandler::PARTITION;
                    partInfoData.m_disktype = info.m_disktype;
                    partInfoData.m_diskPath = partitionInfo.m_devicePath;
                    partInfoData.m_diskSize = "";
                    partInfoData.m_partitionPath = partitionInfo.m_path;
                    partInfoData.m_partitionSize = partitionSize;
                    partInfoData.m_sectorSize = partitionInfo.m_sectorSize;
                    partInfoData.m_sectorStart = partitionInfo.m_sectorStart;
                    partInfoData.m_sectorEnd = partitionInfo.m_sectorEnd;
                    partInfoData.m_selectStatus = Qt::CheckState::Unchecked;
                    partInfoData.m_luksFlag = partitionInfo.m_luksFlag;
                    if (partitionInfo.m_path == "unallocated") {
                        partInfoData.m_lvmDevType = DevType::DEV_UNALLOCATED_PARTITION;
                    } else {
                        partInfoData.m_lvmDevType = DevType::DEV_PARTITION;
                    }

                    for (int i = 0; i < m_curSeclectData.count(); ++i) {
                        PVInfoData infoData = m_curSeclectData.at(i);
                        if (judgeDataEquality(partitionInfo, infoData)) {
                            diskInfoData.m_selectStatus = Qt::CheckState::Checked;
                            partInfoData.m_selectStatus = Qt::CheckState::Checked;
                            if (infoData.m_isReadOnly) {
                                diskInfoData.m_isReadOnly = infoData.m_isReadOnly;
                                partInfoData.m_isReadOnly = infoData.m_isReadOnly;
                            }
                            break;
                        }
                    }

                    lstPVInfoData.append(partInfoData);
                } else {
                    diskInfoData.m_sectorStart = 0;
                    diskInfoData.m_sectorEnd = info.m_length - 1;

                    for (int i = 0; i < m_curSeclectData.count(); ++i) {
                        PVInfoData infoData = m_curSeclectData.at(i);

                        if (judgeDataEquality(partitionInfo, infoData)) {
                            diskInfoData.m_selectStatus = Qt::CheckState::Checked;
                            if (infoData.m_isReadOnly) {
                                diskInfoData.m_isReadOnly = infoData.m_isReadOnly;
                            }
                            break;
                        }
                    }
                }
            } else {
                int checkCount = 0;
                int readOnlyCount = 0;
                for (int i = 0; i < info.m_partition.size(); ++i) {
                    PartitionInfo partitionInfo = info.m_partition.at(i);
                    QString partitionSize = Utils::formatSize(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                              partitionInfo.m_sectorSize);

                    PVInfoData partInfoData;
                    partInfoData.m_level = DMDbusHandler::PARTITION;
                    partInfoData.m_disktype = info.m_disktype;
                    partInfoData.m_diskPath = partitionInfo.m_devicePath;
                    partInfoData.m_diskSize = "";
                    partInfoData.m_partitionPath = partitionInfo.m_path;
                    partInfoData.m_partitionSize = partitionSize;
                    partInfoData.m_sectorSize = partitionInfo.m_sectorSize;
                    partInfoData.m_sectorStart = partitionInfo.m_sectorStart;
                    partInfoData.m_sectorEnd = partitionInfo.m_sectorEnd;
                    partInfoData.m_selectStatus = Qt::CheckState::Unchecked;
                    partInfoData.m_luksFlag = partitionInfo.m_luksFlag;
                    if (partitionInfo.m_path == "unallocated") {
                        partInfoData.m_lvmDevType = DevType::DEV_UNALLOCATED_PARTITION;
                    } else {
                        partInfoData.m_lvmDevType = DevType::DEV_PARTITION;
                    }

                    for (int j = 0; j < m_curSeclectData.count(); ++j) {
                        PVInfoData infoData = m_curSeclectData.at(j);
                        if (judgeDataEquality(partitionInfo, infoData)) {
                            partInfoData.m_selectStatus = Qt::CheckState::Checked;
                            checkCount++;
                            if (infoData.m_isReadOnly) {
                                partInfoData.m_isReadOnly = infoData.m_isReadOnly;
                                readOnlyCount++;
                            }
                            break;
                        }
                    }

                    lstPVInfoData.append(partInfoData);
                }

                if (checkCount == 0) {
                    diskInfoData.m_selectStatus = Qt::CheckState::Unchecked;
                } else if (checkCount == lstPVInfoData.count()) {
                    diskInfoData.m_selectStatus = Qt::CheckState::Checked;
                } else {
                    diskInfoData.m_selectStatus = Qt::CheckState::PartiallyChecked;
                }

                if (readOnlyCount == lstPVInfoData.count()) {
                    diskInfoData.m_isReadOnly = true;
                }
            }

            SelectPVItemWidget *selectPVItemWidget = new SelectPVItemWidget(diskInfoData);
            selectPVItemWidget->setData(lstPVInfoData);

            if (lstDeviceInfo.count() == 1) {
                selectPVItemWidget->setMode(true, true, true, true);
            } else {
                if (i == 0) {
                    selectPVItemWidget->setMode(true, false, true, false);
                } else if (i == lstDeviceInfo.count() - 1) {
                    selectPVItemWidget->setMode(false, true, false, true);
                } else {
                    selectPVItemWidget->setMode(false, false, false, false);
                }
            }

            if (i == 0) {
                selectPVItemWidget->setChecked(true);
                m_curDiskItemWidget = selectPVItemWidget;
                updatePartitionData(lstPVInfoData);
            }

            connect(selectPVItemWidget, &SelectPVItemWidget::selectItem, this, &CreateVGWidget::onDiskItemClicked);
            connect(selectPVItemWidget, &SelectPVItemWidget::checkBoxStateChange, this, &CreateVGWidget::onDiskCheckBoxStateChange);

            diskLayout->addWidget(selectPVItemWidget);
        }

        diskLayout->addStretch();
        diskLayout->setSpacing(1);
        diskLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget = new QWidget;
        widget->setObjectName("diskScrollArea");
        widget->setFixedSize(370, lstDeviceInfo.count() * 37);
        widget->setLayout(diskLayout);
        m_diskScrollArea->setWidget(widget);

        if (m_curSeclectData.count() == 0) {
            m_seclectedLabel->setText(tr("Capacity selected: %1").arg("0GiB"));
            m_nextButton->setDisabled(true);
        } else {
            double sumSize = 0;
            for (int i = 0; i < m_curSeclectData.count(); ++i) {
                PVInfoData infoData = m_curSeclectData.at(i);
                sumSize += Utils::sectorToUnit(infoData.m_sectorEnd - infoData.m_sectorStart + 1,
                                               infoData.m_sectorSize, SIZE_UNIT::UNIT_GIB);
            }

            m_seclectedLabel->setText(tr("Capacity selected: %1").arg(QString::number(sumSize, 'f', 2)) + "GiB");
            m_nextButton->setDisabled(false);
        }
    }
    qDebug() << "CreateVGWidget::updateData completed";
}

static bool isDeviceRemovable(const QString &devicePath)
{
    QFileInfo deviceInfo(devicePath);
    QString deviceName = deviceInfo.fileName(); // e.g., "sda"

    QString sysfsRemovablePath = QString("/sys/block/%1/removable").arg(deviceName);
    QFile removableFile(sysfsRemovablePath);
    bool removable = false;
    QByteArray content;

    if (!removableFile.exists()) {
        qWarning() << "Sysfs path does not exist:" << sysfsRemovablePath;
        goto quit;
    }

    if (!removableFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open sysfs removable file:" << sysfsRemovablePath;
        goto quit;
    }

    content = removableFile.readAll().trimmed(); // trimmed() 移除换行符和空格
    removable = (content == "1");

quit:
    qDebug() << "isDeviceRemovable:" << devicePath << removable;
    return removable;
}

QList<DeviceInfo> CreateVGWidget::createAvailableDiskData()
{
    qDebug() << "CreateVGWidget::createAvailableDiskData called.";
    QList<DeviceInfo> lstDeviceInfo;
    lstDeviceInfo.clear();

    DeviceInfoMap deviceInfoMap = DMDbusHandler::instance()->probDeviceInfo();
    QMap<QString, QString> isJoinAllVG = DMDbusHandler::instance()->getIsJoinAllVG();

    for (auto devInfo = deviceInfoMap.begin(); devInfo != deviceInfoMap.end(); devInfo++) {
        DeviceInfo info = devInfo.value();
        if (info.m_path.isEmpty() || info.m_path.contains("/dev/mapper")) {
            continue;
        }

        // 排除可移动设备
        if (isDeviceRemovable(info.m_path)) {
            continue;
        }

        // 排除已加入VG的磁盘以及分区表错误的磁盘
        if (isJoinAllVG.value(info.m_path) == "false") {//还需加上磁盘分区表是否错误的判断
            PartitionVec lstNewPartition;
            lstNewPartition.clear();
            bool isAvailable = true;

            if (info.m_partition.size() == 1) {
                PartitionInfo partitionInfo = info.m_partition.at(0);
                // 只有一个分区时，排除已加入VG的分区、被挂载的分区、分区大小小于100MiB的分区
                double partitionSize = Utils::sectorToUnit(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                           partitionInfo.m_sectorSize, UNIT_MIB);
                bool findPV = false;
                if(partitionInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS){
                    LUKSMap luks = DMDbusHandler::instance()->probLUKSInfo();
                    if(luks.mapperExists(partitionInfo.m_path)){
                        LUKS_MapperInfo mapper = luks.getMapper(partitionInfo.m_path);
                        if(mapper.m_vgflag != LVMFlag::LVM_FLAG_NOT_PV){
                            findPV = true;
                        }
                    }
                }

                if ((partitionInfo.m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV)
                        && !DMDbusHandler::instance()->partitionISMount(partitionInfo) && (partitionSize > 100)
                        && !findPV) {
                    lstNewPartition.append(partitionInfo);
                } else {
                    isAvailable = false;
                }
            } else {
                int partitionCount = 0;
                for (int i = 0; i < info.m_partition.size(); ++i) {
                    if (info.m_partition.at(i).m_path != "unallocated" && info.m_partition.at(i).m_type != PartitionType::TYPE_LOGICAL) {
                        partitionCount++;
                    }
                }

                PartitionInfo extendedInfo;
                for (int i = 0; i < info.m_partition.size(); ++i) {
                    PartitionInfo partitionInfo = info.m_partition.at(i);
                    bool isSysMountPoint = false;
                    for (int j = 0; j < partitionInfo.m_mountPoints.size(); ++j) {
                        if (partitionInfo.m_mountPoints[j] == "/boot/efi" || partitionInfo.m_mountPoints[j] == "/boot"
                                || partitionInfo.m_mountPoints[j] == "/" || partitionInfo.m_mountPoints[j] == "/recovery") {
                            isSysMountPoint = true;
                            break;
                        }
                    }

                    // 排除系统磁盘
                    if (isSysMountPoint || partitionInfo.m_flag == 4) {
                        isAvailable = false;
                        break;
                    }

                    // 排除已加入VG的分区、被挂载的分区、分区大小小于100MiB的分区
                    double partitionSize = Utils::sectorToUnit(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                               partitionInfo.m_sectorSize, UNIT_MIB);
                    QString fileSystemType = Utils::fileSystemTypeToString(static_cast<FSType>(partitionInfo.m_fileSystemType));
                    if (fileSystemType == "extended") {
                        extendedInfo = partitionInfo;
                    }

                    bool findPV = false;
                    if(partitionInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS){
                        LUKSMap luks = DMDbusHandler::instance()->probLUKSInfo();
                        if(luks.mapperExists(partitionInfo.m_path)){
                            LUKS_MapperInfo mapper = luks.getMapper(partitionInfo.m_path);
                            if(mapper.m_vgflag != LVMFlag::LVM_FLAG_NOT_PV){
                                findPV = true;
                            }
                        }
                    }

                    if ((partitionInfo.m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV) && !DMDbusHandler::instance()->partitionISMount(partitionInfo)
                            && (partitionSize > 100) && (fileSystemType != "extended")
                            && (partitionInfo.m_type != PartitionType::TYPE_LOGICAL)
                            && !findPV) {
                        // 排除不能创建新分区的磁盘空闲空间
                        if (partitionInfo.m_path == "unallocated") {
                            if (partitionCount >= info.m_maxPrims) {
                                continue;
                            }

                            if (info.m_disktype == "msdos") {
                                if (partitionInfo.m_sectorStart >= extendedInfo.m_sectorStart &&
                                        partitionInfo.m_sectorEnd <= extendedInfo.m_sectorEnd) {
                                    continue;
                                }
                            }
                        }

                        // 此处为各种条件排除后可创建VG的分区
                        lstNewPartition.append(partitionInfo);
                    }
                }
            }

            if (isAvailable) {
                info.m_partition = lstNewPartition;
                if (info.m_partition.size()) {  //解决bug 119081
                    lstDeviceInfo.append(info);
                }
            }
        }
    }

    qDebug() << "CreateVGWidget::createAvailableDiskData completed";
    return lstDeviceInfo;
}

QList<DeviceInfo> CreateVGWidget::resizeAvailableDiskData()
{
    qDebug() << "CreateVGWidget::resizeAvailableDiskData called.";
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    QMap<QString, PVInfo> mapPvInfo = vgInfo.m_pvInfo;
    QStringList pvList = mapPvInfo.keys();
    bool isSysDevice = DMDbusHandler::instance()->getIsSystemDisk(vgInfo.m_vgName);

    if (m_isResizeInit) {
        LVMInfo lvmInfo = DMDbusHandler::instance()->probLVMInfo();
        for (int i = 0; i < pvList.count(); ++i) {
            if (lvmInfo.pvExists(pvList.at(i))) {
                PVInfo pvInfo = lvmInfo.getPV(pvList.at(i));
                PVInfoData pvInfoData;
                pvInfoData.m_level = 1;
                pvInfoData.m_luksFlag = LUKSFlag::NOT_CRYPT_LUKS;
                pvInfoData.m_lvmDevType = pvInfo.m_lvmDevType;
                pvInfoData.m_selectStatus = Qt::CheckState::Checked;
                if (pvInfo.m_lvmDevType == DevType::DEV_DISK) {
                    pvInfoData.m_diskPath = pvInfo.m_pvPath;
                } else if (pvInfo.m_lvmDevType == DevType::DEV_PARTITION) {
                    pvInfoData.m_partitionPath = pvInfo.m_pvPath;
                } else if (pvInfo.m_lvmDevType == DevType::DEV_META_DEVICES) {
                    QString path = DMDbusHandler::instance()->probLUKSInfo().getDevPath(pvInfo.m_pvPath);
                    if (DMDbusHandler::instance()->probDeviceInfo().keys().indexOf(path) == -1) {
                        pvInfoData.m_partitionPath = path;
                    } else {
                        pvInfoData.m_diskPath = path;
                    }
                }

                m_oldSeclectData.append(pvInfoData);
            }
        }
    }

    QList<DeviceInfo> lstDeviceInfo;
    lstDeviceInfo.clear();
    DeviceInfoMap deviceInfoMap = DMDbusHandler::instance()->probDeviceInfo();
    for (auto devInfo = deviceInfoMap.begin(); devInfo != deviceInfoMap.end(); devInfo++) {
        DeviceInfo info = devInfo.value();
        if (info.m_path.isEmpty() || info.m_path.contains("/dev/mapper")) {
            continue;
        }

        if (pvList.indexOf(info.m_path) != -1) {
            for (int i = 0; i < m_oldSeclectData.count(); ++i) {
                PVInfoData pvInfoData = m_oldSeclectData.at(i);
                if (pvInfoData.m_diskPath == info.m_path) {
                    QString diskSize = Utils::formatSize(info.m_length, info.m_sectorSize);
                    pvInfoData.m_disktype = info.m_disktype;
                    if (info.m_disktype == "none") {
                        pvInfoData.m_disktype = "unrecognized";
                        info.m_disktype = "unrecognized";
                    }
                    pvInfoData.m_level = DMDbusHandler::DISK;
                    pvInfoData.m_diskPath = info.m_path;
                    pvInfoData.m_diskSize = diskSize;
                    pvInfoData.m_partitionPath = "";
                    pvInfoData.m_partitionSize = "";
                    pvInfoData.m_sectorSize = info.m_sectorSize;
                    pvInfoData.m_sectorStart = 0;
                    pvInfoData.m_sectorEnd = info.m_length - 1;
                    pvInfoData.m_luksFlag = info.m_luksFlag;
                    isSysDevice ? pvInfoData.m_isReadOnly = true : pvInfoData.m_isReadOnly = false;
                    if (pvInfoData.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
                        pvInfoData.m_isReadOnly = true;
                    }

                    m_oldSeclectData.replace(i, pvInfoData);
                    lstDeviceInfo.append(info);
                    break;
                }
            }
        } else {
            PartitionVec lstNewPartition;
            lstNewPartition.clear();
            bool isAvailable = true;

            if (info.m_partition.size() == 1) {
                PartitionInfo partitionInfo = info.m_partition.at(0);
                QString path = partitionInfo.m_path;
                if (partitionInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
                    LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(partitionInfo.m_path);
                    if (luksInfo.isDecrypt) {
                        path = luksInfo.m_mapper.m_dmPath;
                    }
                }
                //判断分区是否加入当前选择VG
                if (pvList.indexOf(path) != -1) {
                    for (int i = 0; i < m_oldSeclectData.count(); ++i) {
                        PVInfoData pvInfoData = m_oldSeclectData.at(i);
                        if (pvInfoData.m_partitionPath == partitionInfo.m_path) {
                            QString partitionSize = Utils::formatSize(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                                      partitionInfo.m_sectorSize);
                            pvInfoData.m_level = DMDbusHandler::PARTITION;
                            pvInfoData.m_disktype = info.m_disktype;
                            pvInfoData.m_diskPath = partitionInfo.m_devicePath;
                            pvInfoData.m_diskSize = "";
                            pvInfoData.m_partitionPath = partitionInfo.m_path;
                            pvInfoData.m_partitionSize = partitionSize;
                            pvInfoData.m_sectorSize = partitionInfo.m_sectorSize;
                            pvInfoData.m_sectorStart = partitionInfo.m_sectorStart;
                            pvInfoData.m_sectorEnd = partitionInfo.m_sectorEnd;
                            pvInfoData.m_luksFlag = partitionInfo.m_luksFlag;
                            isSysDevice ? pvInfoData.m_isReadOnly = true : pvInfoData.m_isReadOnly = false;
                            if (pvInfoData.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
                                pvInfoData.m_isReadOnly = true;
                            }

                            m_oldSeclectData.replace(i, pvInfoData);
                            lstNewPartition.append(partitionInfo);
                            break;
                        }
                    }
                } else {
                    // 只有一个分区时，排除已加入VG的分区、被挂载的分区、分区大小小于100MiB的分区
                    double partitionSize = Utils::sectorToUnit(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                               partitionInfo.m_sectorSize, UNIT_MIB);
                    bool findPV = false;
                    if(partitionInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS){
                        LUKSMap luks = DMDbusHandler::instance()->probLUKSInfo();
                        if(luks.mapperExists(partitionInfo.m_path)){
                            LUKS_MapperInfo mapper = luks.getMapper(partitionInfo.m_path);
                            if(mapper.m_vgflag != LVMFlag::LVM_FLAG_NOT_PV){
                                findPV = true;
                            }
                        }
                    }

                    if ((partitionInfo.m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV)
                            && !DMDbusHandler::instance()->partitionISMount(partitionInfo) && (partitionSize > 100)
                            && !findPV) {
                        lstNewPartition.append(partitionInfo);
                    } else {
                        isAvailable = false;
                    }
                }
            } else {
                int partitionCount = 0;
                for (int i = 0; i < info.m_partition.size(); ++i) {
                    if (info.m_partition.at(i).m_path != "unallocated" && info.m_partition.at(i).m_type != PartitionType::TYPE_LOGICAL) {
                        partitionCount++;
                    }
                }

                PartitionInfo extendedInfo;
                for (int i = 0; i < info.m_partition.size(); ++i) {
                    PartitionInfo partitionInfo = info.m_partition.at(i);
                    QString path = partitionInfo.m_path;
                    if (partitionInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
                        LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(partitionInfo.m_path);
                        if (luksInfo.isDecrypt) {
                            path = luksInfo.m_mapper.m_dmPath;
                        }
                    }

                    //判断分区是否加入当前选择VG
                    if (pvList.indexOf(path) != -1) {
                        for (int j = 0; j < m_oldSeclectData.count(); ++j) {
                            PVInfoData pvInfoData = m_oldSeclectData.at(j);
                            if (pvInfoData.m_partitionPath == partitionInfo.m_path) {
                                QString partitionSize = Utils::formatSize(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                                          partitionInfo.m_sectorSize);
                                pvInfoData.m_level = DMDbusHandler::PARTITION;
                                pvInfoData.m_disktype = info.m_disktype;
                                pvInfoData.m_diskPath = partitionInfo.m_devicePath;
                                pvInfoData.m_diskSize = "";
                                pvInfoData.m_partitionPath = partitionInfo.m_path;
                                pvInfoData.m_partitionSize = partitionSize;
                                pvInfoData.m_sectorSize = partitionInfo.m_sectorSize;
                                pvInfoData.m_sectorStart = partitionInfo.m_sectorStart;
                                pvInfoData.m_sectorEnd = partitionInfo.m_sectorEnd;
                                pvInfoData.m_luksFlag = partitionInfo.m_luksFlag;
                                isSysDevice ? pvInfoData.m_isReadOnly = true : pvInfoData.m_isReadOnly = false;
                                if (pvInfoData.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
                                    pvInfoData.m_isReadOnly = true;
                                }

                                m_oldSeclectData.replace(j, pvInfoData);
                                lstNewPartition.append(partitionInfo);
                                break;
                            }
                        }
                    } else {
                        bool isSysMountPoint = false;
                        for (int j = 0; j < partitionInfo.m_mountPoints.size(); ++j) {
                            if (partitionInfo.m_mountPoints[j] == "/boot/efi" || partitionInfo.m_mountPoints[j] == "/boot"
                                    || partitionInfo.m_mountPoints[j] == "/" || partitionInfo.m_mountPoints[j] == "/recovery") {
                                isSysMountPoint = true;
                                break;
                            }
                        }

                        // 排除系统磁盘
                        if (isSysMountPoint || partitionInfo.m_flag == 4) {
                            isAvailable = false;
//                            break;
                        }

                        // 排除已加入VG的分区、被挂载的分区、分区大小小于100MiB的分区
                        double partitionSize = Utils::sectorToUnit(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1,
                                                                   partitionInfo.m_sectorSize, UNIT_MIB);
                        QString fileSystemType = Utils::fileSystemTypeToString(static_cast<FSType>(partitionInfo.m_fileSystemType));
                        if (fileSystemType == "extended") {
                            extendedInfo = partitionInfo;
                        }

                        bool findPV = false;
                        if(partitionInfo.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS){
                            LUKSMap luks = DMDbusHandler::instance()->probLUKSInfo();
                            if(luks.mapperExists(partitionInfo.m_path)){
                                LUKS_MapperInfo mapper = luks.getMapper(partitionInfo.m_path);
                                if(mapper.m_vgflag != LVMFlag::LVM_FLAG_NOT_PV){
                                    findPV = true;
                                }
                            }
                        }

                        if ((partitionInfo.m_vgFlag == LVMFlag::LVM_FLAG_NOT_PV)
                                && !DMDbusHandler::instance()->partitionISMount(partitionInfo)
                                && (partitionSize > 100) && (fileSystemType != "extended")
                                && (partitionInfo.m_type != PartitionType::TYPE_LOGICAL) && !findPV) {
                            // 排除不能创建新分区的磁盘空闲空间
                            if (partitionInfo.m_path == "unallocated") {
                                if (partitionCount >= info.m_maxPrims) {
                                    continue;
                                }

                                if (info.m_disktype == "msdos") {
                                    if (partitionInfo.m_sectorStart >= extendedInfo.m_sectorStart &&
                                            partitionInfo.m_sectorEnd <= extendedInfo.m_sectorEnd) {
                                        continue;
                                    }
                                }
                            }

                            // 此处为各种条件排除后可创建VG的分区
                            lstNewPartition.append(partitionInfo);
                        }
                    }
                }

                // 如果没有可用分区，磁盘则不显示
                if (lstNewPartition.size() == 0) {
                    isAvailable = false;
                }
            }

            if (isAvailable) {
                info.m_partition = lstNewPartition;
                if (info.m_partition.size()) {  //解决bug 119081
                    lstDeviceInfo.append(info);
                }
            } else {
                PartitionVec lstNewPartition;
                for (int i = 0; i < info.m_partition.size(); ++i) {
                    for (int j = 0; j < m_oldSeclectData.count(); ++j) {
                        if (judgeDataEquality(info.m_partition.at(i), m_oldSeclectData.at(j))) {
                            lstNewPartition.append(info.m_partition.at(i));
                        }
                    }
                }

                if (lstNewPartition.count() > 0) {
                    info.m_partition = lstNewPartition;
                    lstDeviceInfo.append(info);
                }
            }
        }
    }

    if (m_isResizeInit) {
        qDebug() << "Resize init.";
        m_curSeclectData = m_oldSeclectData;
    }

    m_isResizeInit = false;
    qDebug() << "CreateVGWidget::createAvailableDiskData completed";
    return lstDeviceInfo;
}

void CreateVGWidget::onDiskItemClicked()
{
    qDebug() << "CreateVGWidget::onDiskItemClicked called.";
    // 当前选中行是否为已选中行
    SelectPVItemWidget *selectPVItemWidget = qobject_cast<SelectPVItemWidget *>(sender());
    if (m_curDiskItemWidget == selectPVItemWidget) {
        qDebug() << "Current disk item is already selected, returning.";
        return;
    }

    // 选中其他行时
    if (nullptr != m_curDiskItemWidget) {
        qDebug() << "Current disk item is not null, unchecking it.";
        m_curDiskItemWidget->setChecked(false);

        QWidget *partWidget = m_partitionScrollArea->findChild<QWidget *>("partScrollArea");
        Utils::deletePoint(partWidget);
    }

    selectPVItemWidget->setChecked(true);
    QList<PVInfoData> lstData = selectPVItemWidget->getData();
    m_curDiskItemWidget = selectPVItemWidget;

    updatePartitionData(lstData);
    qDebug() << "CreateVGWidget::onDiskItemClicked completed.";
}

void CreateVGWidget::updatePartitionData(const QList<PVInfoData> &lstData)
{
    qDebug() << "CreateVGWidget::updatePartitionData called. Data count:" << lstData.count();
    if (lstData.count() == 0) {
        qDebug() << "No partition data, showing no partition widget.";
        m_partitionStackedWidget->setCurrentIndex(1);
    } else {
        qDebug() << "Partition data available, updating partition UI.";
        m_partitionStackedWidget->setCurrentIndex(0);

        QVBoxLayout *partitionLayout = new QVBoxLayout;
        for (int i = 0; i < lstData.count(); ++i) {
            PVInfoData pvInfoData = lstData.at(i);
            qDebug() << "Processing partition:" << pvInfoData.m_partitionPath;

            SelectPVItemWidget *selectPVItemWidget = new SelectPVItemWidget(pvInfoData);
            if (lstData.count() == 1) {
                qDebug() << "Single partition in list, setting mode accordingly.";
                selectPVItemWidget->setMode(true, true, true, true);
            } else {
                if (i == 0) {
                    qDebug() << "First partition in multi-partition list, setting mode accordingly.";
                    selectPVItemWidget->setMode(true, false, true, false);
                } else if (i == lstData.count() - 1) {
                    qDebug() << "Last partition in multi-partition list, setting mode accordingly.";
                    selectPVItemWidget->setMode(false, true, false, true);
                } else {
                    qDebug() << "Middle partition in multi-partition list, setting mode accordingly.";
                    selectPVItemWidget->setMode(false, false, false, false);
                }
            }

            switch (pvInfoData.m_selectStatus) {
            case Qt::CheckState::Unchecked: {
                qDebug() << "Partition status: Unchecked.";
                selectPVItemWidget->setCheckBoxState(Qt::CheckState::Unchecked);
                break;
            }
            case Qt::CheckState::Checked: {
                qDebug() << "Partition status: Checked.";
                selectPVItemWidget->setCheckBoxState(Qt::CheckState::Checked);
                break;
            }
            default:
                qDebug() << "Partition status: Default (no change).";
                break;
            }

            connect(selectPVItemWidget, &SelectPVItemWidget::selectItem, this, &CreateVGWidget::onDiskItemClicked);
            connect(selectPVItemWidget, &SelectPVItemWidget::checkBoxStateChange, this, &CreateVGWidget::onPartitionCheckBoxStateChange);
            qDebug() << "Connections made for partition SelectPVItemWidget.";

            partitionLayout->addWidget(selectPVItemWidget);
        }

        partitionLayout->addStretch();
        partitionLayout->setSpacing(1);
        partitionLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget = new QWidget;
        widget->setObjectName("partScrollArea");
        widget->setFixedSize(370, lstData.count() * 37);
        widget->setLayout(partitionLayout);
        m_partitionScrollArea->setWidget(widget);
        qDebug() << "Partition scroll area widget set.";
    }
    qDebug() << "Update partition data process finished.";
}

void CreateVGWidget::onDiskCheckBoxStateChange(int state)
{
    qDebug() << "CreateVGWidget::onDiskCheckBoxStateChange called. State:" << state;
    // 若为半选状态置为选中状态
    if (state == Qt::CheckState::PartiallyChecked) {
        qDebug() << "Disk checkbox state is PartiallyChecked, setting to Checked.";
        SelectPVItemWidget *selectPVItemWidget = qobject_cast<SelectPVItemWidget *>(sender());
        selectPVItemWidget->setCheckBoxState(Qt::CheckState::Checked, true);
    } else {
        if (state == Qt::CheckState::Unchecked) {
            qDebug() << "Disk checkbox state is Unchecked (disk unselected).";
            // 磁盘取消选中
            QList<SelectPVItemWidget *> lstPartitionItem = m_partitionScrollArea->findChildren<SelectPVItemWidget *>();
            if (lstPartitionItem.count() == 0) {
                qDebug() << "No partition items, calling onDiskItemClicked.";
                onDiskItemClicked();
            } else {
                SelectPVItemWidget *selectPVItemWidget = qobject_cast<SelectPVItemWidget *>(sender());
                if (m_curDiskItemWidget == selectPVItemWidget) {
                    qDebug() << "Current disk item widget matches sender.";
                    int readOnlyCount = 0;
                    for (int i = 0; i < lstPartitionItem.count(); ++i) {
                        SelectPVItemWidget *selectPVItemWidget = lstPartitionItem.at(i);
                        if (selectPVItemWidget->getCurInfo().m_isReadOnly) {
                            readOnlyCount++;
                            qDebug() << "Partition is read-only, skipping uncheck.";
                            continue;
                        }

                        selectPVItemWidget->setCheckBoxState(Qt::CheckState::Unchecked);
                        qDebug() << "Unchecked partition:" << selectPVItemWidget->getCurInfo().m_partitionPath;
                    }

                    if (readOnlyCount > 0 && readOnlyCount != lstPartitionItem.count()) {
                        m_curDiskItemWidget->setCheckBoxState(Qt::CheckState::PartiallyChecked);
                        qDebug() << "Setting current disk item widget to PartiallyChecked due to read-only partitions.";
                    } else if (readOnlyCount == lstPartitionItem.count()) {
                        m_curDiskItemWidget->setCheckBoxState(Qt::CheckState::Checked);
                        qDebug() << "Setting current disk item widget to Checked (all read-only).";
                    }

                } else {
                    qDebug() << "Current disk item widget does not match sender, calling onDiskItemClicked.";
                    onDiskItemClicked();
                }
            }

            // 当前取消磁盘是否没有分区表
            if (m_curDiskItemWidget->getCurInfo().m_disktype == "unrecognized") {
                qDebug() << "Current unselected disk is unrecognized type.";
                PVInfoData pvData = m_curDiskItemWidget->getCurInfo();
                for (int i = 0; i < m_curSeclectData.count(); ++i) {
                    PVInfoData infoData = m_curSeclectData.at(i);
                    if (judgeDataEquality(pvData, infoData)) {
                        if (infoData.m_isReadOnly) {
                            qDebug() << "Read-only unrecognized disk, skipping removal.";
                            continue;
                        }

                        m_curSeclectData.removeAt(i);
                        qDebug() << "Removed unrecognized disk from selected data:" << pvData.m_diskPath;
                        break;
                    }
                }
            } else {
                qDebug() << "Current unselected disk is recognized type.";
                QList<PVInfoData> lstData = m_curDiskItemWidget->getData();
                for (int i = 0; i < lstData.count(); ++i) {
                    PVInfoData pvData = lstData.at(i);

                    for (int j = 0; j < m_curSeclectData.count(); ++j) {
                        PVInfoData infoData = m_curSeclectData.at(j);
                        if (judgeDataEquality(pvData, infoData)) {
                            if (infoData.m_isReadOnly) {
                                qDebug() << "Read-only partition, skipping removal.";
                                continue;
                            }
                            m_curSeclectData.removeAt(j);
                            qDebug() << "Removed partition from selected data:" << pvData.m_partitionPath;
                            break;
                        }
                    }
                }
            }

            if (m_curSeclectData.count() == 0) {
                qDebug() << "No current selected data after unchecking, setting capacity to 0GiB and disabling next button.";
                m_seclectedLabel->setText(tr("Capacity selected: %1").arg("0GiB"));
                m_nextButton->setDisabled(true);
            } else {
                double sumSize = 0;
                for (int i = 0; i < m_curSeclectData.count(); ++i) {
                    PVInfoData infoData = m_curSeclectData.at(i);
                    sumSize += Utils::sectorToUnit(infoData.m_sectorEnd - infoData.m_sectorStart + 1,
                                                   infoData.m_sectorSize, SIZE_UNIT::UNIT_GIB);
                }
                qDebug() << "Calculated sum size after unchecking:" << sumSize << "GiB.";

                m_seclectedLabel->setText(tr("Capacity selected: %1").arg(QString::number(sumSize, 'f', 2)) + "GiB");
            }
        } else if (state == Qt::CheckState::Checked) {
            qDebug() << "Disk checkbox state is Checked (disk selected).";
            // 磁盘选中
            QList<SelectPVItemWidget *> lstPartitionItem = m_partitionScrollArea->findChildren<SelectPVItemWidget *>();
            if (lstPartitionItem.count() == 0) {
                qDebug() << "No partition items, calling onDiskItemClicked.";
                onDiskItemClicked();
            } else {
                SelectPVItemWidget *selectPVItemWidget = qobject_cast<SelectPVItemWidget *>(sender());
                if (m_curDiskItemWidget == selectPVItemWidget) {
                    qDebug() << "Current disk item widget matches sender, checking all partitions.";
                    for (int i = 0; i < lstPartitionItem.count(); ++i) {
                        SelectPVItemWidget *selectPVItemWidget = lstPartitionItem.at(i);
                        selectPVItemWidget->setCheckBoxState(Qt::CheckState::Checked);
                        qDebug() << "Checked partition:" << selectPVItemWidget->getCurInfo().m_partitionPath;
                    }
                } else {
                    qDebug() << "Current disk item widget does not match sender, calling onDiskItemClicked.";
                    onDiskItemClicked();
                }
            }

            // 当前选中磁盘是否没有分区表
            if (m_curDiskItemWidget->getCurInfo().m_disktype == "unrecognized") {
                qDebug() << "Current selected disk is unrecognized type, adding to selected data.";
                m_curSeclectData << m_curDiskItemWidget->getCurInfo();
            } else {
                qDebug() << "Current selected disk is recognized type, adding its partitions to selected data.";
                QList<PVInfoData> lstData = m_curDiskItemWidget->getData();
                for (int i = 0; i < lstData.count(); ++i) {
                    PVInfoData pvData = lstData.at(i);

                    bool isSameData = false;
                    for (int j = 0; j < m_curSeclectData.count(); ++j) {
                        PVInfoData infoData = m_curSeclectData.at(j);
                        if (judgeDataEquality(pvData, infoData)) {
                            isSameData = true;
                            qDebug() << "Partition already in selected data, skipping:" << pvData.m_partitionPath;
                            break;
                        }
                    }

                    if (!isSameData) {
                        m_curSeclectData << pvData;
                        qDebug() << "Added partition to selected data:" << pvData.m_partitionPath;
                    }
                }
            }

            double sumSize = 0;
            for (int i = 0; i < m_curSeclectData.count(); ++i) {
                PVInfoData infoData = m_curSeclectData.at(i);
                sumSize += Utils::sectorToUnit(infoData.m_sectorEnd - infoData.m_sectorStart + 1,
                                               infoData.m_sectorSize, SIZE_UNIT::UNIT_GIB);
            }
            qDebug() << "Calculated sum size after checking:" << sumSize << "GiB.";

            m_seclectedLabel->setText(tr("Capacity selected: %1").arg(QString::number(sumSize, 'f', 2)) + "GiB");
            m_nextButton->setDisabled(false);
            qDebug() << "Capacity selected label updated and next button enabled.";

            if (m_curDiskItemWidget->getCurInfo().m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
                qDebug() << "LUKS encrypted disk selected, showing warning.";
                MessageBox warningBox(this);
                warningBox.setFixedWidth(450);
                warningBox.setObjectName("messageBox");
                warningBox.setAccessibleName("cryptDeviceWidget");
                // 加入逻辑卷组会进行磁盘/分区格式化，并删除加密磁盘/分区的密码  确定
                warningBox.setWarings(tr("Adding the disk/partition to a logical volume group \n"
                                         "will format it and remove its password."), "", tr("OK"), "ok");
                warningBox.exec();
                qDebug() << "LUKS warning shown.";
            }
        }
    }
    qDebug() << "Disk checkbox state change handled.";
}

void CreateVGWidget::onPartitionCheckBoxStateChange(int state)
{
    qDebug() << "CreateVGWidget::onPartitionCheckBoxStateChange called. State:" << state;
    SelectPVItemWidget *selectPVItemWidget = qobject_cast<SelectPVItemWidget *>(sender());
    PVInfoData pvData = selectPVItemWidget->getCurInfo();
    QList<PVInfoData> lstData = m_curDiskItemWidget->getData();
    bool isPartiallyCheck = false;

    if (state == Qt::CheckState::Unchecked) {
        qDebug() << "Partition checkbox state is Unchecked (partition unselected).";
        // 分区取消选中
        for (int i = 0; i < lstData.count(); ++i) {
            PVInfoData data = lstData.at(i);
            if (pvData.m_sectorStart == data.m_sectorStart && pvData.m_sectorEnd == data.m_sectorEnd) {
                data.m_selectStatus = state;
                lstData.replace(i, data);
                qDebug() << "Updated partition status to Unchecked for:" << data.m_partitionPath;
            }

            if (data.m_selectStatus == Qt::CheckState::Checked) {
                isPartiallyCheck = true;
                qDebug() << "Found checked partition, setting isPartiallyCheck to true.";
            }
        }

        if (isPartiallyCheck) {
            m_curDiskItemWidget->setCheckBoxState(Qt::CheckState::PartiallyChecked);
            qDebug() << "Setting disk item widget to PartiallyChecked.";
        } else {
            m_curDiskItemWidget->setCheckBoxState(Qt::CheckState::Unchecked);
            qDebug() << "Setting disk item widget to Unchecked.";
        }

        double sumSize = 0;
        int deleteNumber = -1;
        for (int i = 0; i < m_curSeclectData.count(); ++i) {
            PVInfoData infoData = m_curSeclectData.at(i);
            if (judgeDataEquality(pvData, infoData)) {
                deleteNumber = i;
                qDebug() << "Found partition to delete from selected data at index:" << i;
            } else {
                sumSize += Utils::sectorToUnit(infoData.m_sectorEnd - infoData.m_sectorStart + 1,
                                               infoData.m_sectorSize, SIZE_UNIT::UNIT_GIB);
            }
        }

        if (deleteNumber >= 0) {
            m_curSeclectData.removeAt(deleteNumber);
            qDebug() << "Removed partition from selected data:" << pvData.m_partitionPath;
        }

        if (m_curSeclectData.count() == 0) {
            m_seclectedLabel->setText(tr("Capacity selected: %1").arg("0GiB"));
            m_nextButton->setDisabled(true);
            qDebug() << "No selected data, setting capacity to 0GiB and disabling next button.";
        } else {
            m_seclectedLabel->setText(tr("Capacity selected: %1").arg(QString::number(sumSize, 'f', 2)) + "GiB");
            qDebug() << "Updated selected capacity label:" << sumSize << "GiB.";
        }
    } else {
        qDebug() << "Partition checkbox state is Checked (partition selected).";
        // 分区选中
        for (int i = 0; i < lstData.count(); ++i) {
            PVInfoData data = lstData.at(i);
            if (pvData.m_sectorStart == data.m_sectorStart && pvData.m_sectorEnd == data.m_sectorEnd) {
                data.m_selectStatus = state;
                lstData.replace(i, data);
                qDebug() << "Updated partition status to Checked for:" << data.m_partitionPath;
            }

            if (data.m_selectStatus == Qt::CheckState::Unchecked) {
                isPartiallyCheck = true;
                qDebug() << "Found unchecked partition, setting isPartiallyCheck to true.";
            }
        }

        if (isPartiallyCheck) {
            m_curDiskItemWidget->setCheckBoxState(Qt::CheckState::PartiallyChecked);
            qDebug() << "Setting disk item widget to PartiallyChecked.";
        } else {
            m_curDiskItemWidget->setCheckBoxState(Qt::CheckState::Checked);
            qDebug() << "Setting disk item widget to Checked.";
        }

        m_curSeclectData.append(pvData);
        qDebug() << "Added partition to current selected data:" << pvData.m_partitionPath;
        double sumSize = 0;
        for (int i = 0; i < m_curSeclectData.count(); ++i) {
            PVInfoData infoData = m_curSeclectData.at(i);
            sumSize += Utils::sectorToUnit(infoData.m_sectorEnd - infoData.m_sectorStart + 1,
                                           infoData.m_sectorSize, SIZE_UNIT::UNIT_GIB);
        }
        qDebug() << "Calculated sum size after checking partition:" << sumSize << "GiB.";

        m_seclectedLabel->setText(tr("Capacity selected: %1").arg(QString::number(sumSize, 'f', 2)) + "GiB");
        m_nextButton->setDisabled(false);
        qDebug() << "Updated selected capacity label and enabled next button.";

        if (pvData.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
            qDebug() << "LUKS encrypted partition selected, showing warning.";
            MessageBox warningBox(this);
//            warningBox.setFixedWidth(400);
            warningBox.setObjectName("messageBox");
            warningBox.setAccessibleName("cryptDeviceWidget");
            // 加入/创建逻辑卷组会进行磁盘/分区格式化，并删除加密磁盘/分区的密码  确定
            warningBox.setWarings(tr("Adding the disk/partition to a logical volume group \n"
                                     "will format it and remove its password."), "", tr("OK"), "ok");
            warningBox.exec();
            qDebug() << "LUKS warning shown.";
        }
    }

    m_curDiskItemWidget->setData(lstData);
    qDebug() << "Partition checkbox state change handled.";
}

SIZE_UNIT CreateVGWidget::getCurSizeUnit()
{
    qDebug() << "CreateVGWidget::getCurSizeUnit called.";
    SIZE_UNIT type = SIZE_UNIT::UNIT_GIB;
    switch (m_selectSpaceComboBox->currentIndex()) {
    case 0: {
        type = SIZE_UNIT::UNIT_MIB;
        qDebug() << "Selected unit: MiB.";
        break;
    }
    case 1: {
        type = SIZE_UNIT::UNIT_GIB;
        qDebug() << "Selected unit: GiB.";
        break;
    }
    case 2: {
        type = SIZE_UNIT::UNIT_TIB;
        qDebug() << "Selected unit: TiB.";
        break;
    }
    default:
        qDebug() << "Selected unit: Default (GiB).";
        break;
    }

    qDebug() << "Returning size unit:" << static_cast<int>(type);
    return type;
}

void CreateVGWidget::updateSelectedData()
{
    qDebug() << "CreateVGWidget::updateSelectedData called.";
    if (m_curSeclectData.count() == 0) {
        qDebug() << "No current selected data, showing no info widget and disabling done button.";
        m_selectedStackedWidget->setCurrentIndex(1);
        m_doneButton->setDisabled(true);
    } else {
        qDebug() << "Current selected data available, updating selected data UI.";
        m_selectedStackedWidget->setCurrentIndex(0);
        m_doneButton->setDisabled(false);

        QWidget *selectedWidget = m_selectedScrollArea->findChild<QWidget *>("selectedScrollArea");
        Utils::deletePoint(selectedWidget);
        qDebug() << "Old selected scroll area widget deleted.";

        QVBoxLayout *selectedLayout = new QVBoxLayout;
        bool isUnallocated = false;
        m_minSize = 0;
        m_maxSize = 0;
        m_sumSize = 0;
        SIZE_UNIT type = getCurSizeUnit();
        qDebug() << "Current size unit:" << static_cast<int>(type);

        for (int i = 0; i < m_curSeclectData.count(); ++i) {
            PVInfoData pvInfoData = m_curSeclectData.at(i);
            qDebug() << "Processing selected PVInfoData:" << pvInfoData.m_partitionPath;

            if (pvInfoData.m_partitionPath == "unallocated" || pvInfoData.m_disktype == "unrecognized") {
                isUnallocated = true;
                qDebug() << "Found unallocated or unrecognized partition.";
            } else {
                m_minSize += Utils::sectorToUnit(pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1,
                                                 pvInfoData.m_sectorSize, type);
            }

            m_maxSize += Utils::sectorToUnit(pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1,
                                             pvInfoData.m_sectorSize, type);

            m_sumSize += (pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1) * pvInfoData.m_sectorSize;
            qDebug() << "Added to sumSize (bytes):" << (pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1) * pvInfoData.m_sectorSize;

            SelectedItemWidget *selectedItemWidget = new SelectedItemWidget(pvInfoData);
            if (m_curSeclectData.count() == 1) {
                qDebug() << "Single selected item, setting mode accordingly.";
                selectedItemWidget->setMode(true, true, true, true);
            } else {
                if (i == 0) {
                    qDebug() << "First selected item, setting mode accordingly.";
                    selectedItemWidget->setMode(true, false, true, false);
                } else if (i == m_curSeclectData.count() - 1) {
                    qDebug() << "Last selected item, setting mode accordingly.";
                    selectedItemWidget->setMode(false, true, false, true);
                } else {
                    qDebug() << "Middle selected item, setting mode accordingly.";
                    selectedItemWidget->setMode(false, false, false, false);
                }
            }

            connect(selectedItemWidget, &SelectedItemWidget::deleteItem, this, &CreateVGWidget::onDeleteItemClicked);
            qDebug() << "Connection made for SelectedItemWidget delete item.";

            selectedLayout->addWidget(selectedItemWidget);
        }

        selectedLayout->addStretch();
        selectedLayout->setSpacing(1);
        selectedLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget = new QWidget;
        widget->setObjectName("selectedScrollArea");
        widget->setFixedSize(760, m_curSeclectData.count() * 37);
        widget->setLayout(selectedLayout);
        m_selectedScrollArea->setWidget(widget);
        qDebug() << "Selected scroll area widget set.";

        if (m_curOperationType == OperationType::RESIZE) {
            qDebug() << "Operation type is RESIZE, updating resize specific UI.";
            VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
            set<PVData> pvDataSet = getCurSelectPVData();
            Byte_Value maxSize = getMaxSize(vgInfo, pvDataSet);
            Byte_Value minSize = getMinSize(vgInfo, pvDataSet);

            m_maxSize = Utils::LVMSizeToUnit(maxSize, type);
            m_minSize = Utils::LVMSizeToUnit(minSize, type);
            m_sumSize = maxSize;

            if (maxSize == minSize || !isUnallocated) {
                qDebug() << "Max size equals min size or no unallocated space. Setting select space stacked widget to index 1.";
                m_selectSpaceStackedWidget->setCurrentIndex(1);
                m_curSize = m_sumSize;
                m_selectedSpaceLabel->setText(tr("Capacity selected: %1")
                                              .arg(QString::number(m_maxSize, 'f', 2)) + m_selectSpaceComboBox->currentText());
            } else {
                qDebug() << "Max size not equal to min size and unallocated space exists. Setting select space stacked widget to index 0.";
                m_selectSpaceStackedWidget->setCurrentIndex(0);
                m_selectSpaceLineEdit->lineEdit()->setPlaceholderText(QString("%1-%2").arg(QString::number(m_minSize, 'f', 2))
                                                                      .arg(QString::number(m_maxSize, 'f', 2)));
//                m_selectSpaceLabel->setText(QString("(%1-%2)").arg(QString::number(m_minSize, 'f', 2))
//                                            .arg(QString::number(m_maxSize, 'f', 2)) + m_selectSpaceComboBox->currentText());
                m_selectSpaceLineEdit->setText(QString::number(m_maxSize, 'f', 2));
                qDebug() << "Select space line edit placeholder and text updated.";
            }

            if (maxSize < minSize) {
                qDebug() << "Max size is less than min size, showing warning for not enough space.";
                bool bigDataMove;
                QStringList realDelPvList;
                adjudicationPVMove(vgInfo, pvDataSet, bigDataMove, realDelPvList);
                MessageBox warningBox(this);
                warningBox.setObjectName("messageBox");
                warningBox.setAccessibleName("NotEnoughSpaceWidget");
                // 剩余空间不足，移除XXX（磁盘名称）将会造成数据丢失，请先清理空间  确定
                warningBox.setWarings(tr("Not enough space to back up data on %1, please clear disk space")
                                      .arg(realDelPvList.join(',')), "", tr("OK"), "ok");

                m_doneButton->setDisabled(true);
                warningBox.exec();
                qDebug() << "Not enough space warning shown, done button disabled.";
                return;
            } else {
                // 判断当前选择的PV是否与当前调整VG的PV完全相同，若相同则“完成”按钮禁用
                if (m_curSeclectData.size() == m_oldSeclectData.size()) {
                    bool isIdentical = true;
                    for (int i = 0; i < m_curSeclectData.count(); ++i) {
                        PVInfoData curPVInfoData = m_curSeclectData.at(i);
                        bool isSame = false;
                        for (int j = 0; j < m_oldSeclectData.count(); ++j) {
                            PVInfoData oldPVInfoData = m_oldSeclectData.at(j);
                            if (judgeDataEquality(oldPVInfoData, curPVInfoData)) {
                                isSame = true;
                                qDebug() << "Current PV is identical to old PV:" << curPVInfoData.m_partitionPath;
                                break;
                            }
                        }
                        if (!isSame) {
                            isIdentical = false;
                            qDebug() << "Found non-identical PV, setting isIdentical to false.";
                            break;
                        }
                    }
                    if (isIdentical) {
                        m_doneButton->setDisabled(true);
                        qDebug() << "All PVs are identical, done button disabled.";
                        return;
                    }
                }

                m_doneButton->setDisabled(false);
                qDebug() << "Done button enabled.";
            }
        } else {
            qDebug() << "Operation type is CREATE, updating create specific UI.";
            if (isUnallocated) {
                qDebug() << "Unallocated space found. Setting select space stacked widget to index 0.";
                m_selectSpaceStackedWidget->setCurrentIndex(0);
                m_selectSpaceLineEdit->lineEdit()->setPlaceholderText(QString("%1-%2").arg(QString::number(m_minSize, 'f', 2))
                                                                      .arg(QString::number(m_maxSize, 'f', 2)));
//                m_selectSpaceLabel->setText(QString("(%1-%2)").arg(QString::number(m_minSize, 'f', 2))
//                                            .arg(QString::number(m_maxSize, 'f', 2)) + m_selectSpaceComboBox->currentText());
                m_selectSpaceLineEdit->setText(QString::number(m_maxSize, 'f', 2));
                qDebug() << "Select space line edit placeholder and text updated for unallocated space.";
            } else {
                qDebug() << "No unallocated space found. Setting select space stacked widget to index 1.";
                m_selectSpaceStackedWidget->setCurrentIndex(1);
                m_curSize = m_sumSize;
                m_selectedSpaceLabel->setText(tr("Capacity selected: %1")
                                              .arg(QString::number(m_minSize, 'f', 2)) + m_selectSpaceComboBox->currentText());
                qDebug() << "Selected space label updated for no unallocated space.";
            }
        }
    }
    qDebug() << "Update selected data process finished.";
}

void CreateVGWidget::onDeleteItemClicked(PVInfoData pvInfoData)
{
    qDebug() << "CreateVGWidget::onDeleteItemClicked called. Deleting item for:" << pvInfoData.m_partitionPath;
    for (int i = 0; i < m_curSeclectData.count(); ++i) {
        PVInfoData infoData = m_curSeclectData.at(i);
        if (judgeDataEquality(pvInfoData, infoData)) {
            m_curSeclectData.removeAt(i);
            qDebug() << "Removed item from current selected data:" << infoData.m_partitionPath;
            break;
        }
    }

    updateSelectedData();
    qDebug() << "Delete item process finished, updating selected data UI.";
}

void CreateVGWidget::onCurrentIndexChanged(int index)
{
    qDebug() << "CreateVGWidget::onCurrentIndexChanged called. Index:" << index;
    SIZE_UNIT type = getCurSizeUnit();
    m_minSize = 0;
    m_maxSize = 0;
    m_sumSize = 0;
    qDebug() << "Reset minSize, maxSize, sumSize to 0. Current size unit:" << static_cast<int>(type);

    for (int i = 0; i < m_curSeclectData.count(); ++i) {
        PVInfoData pvInfoData = m_curSeclectData.at(i);
        qDebug() << "Processing selected PVInfoData for index change:" << pvInfoData.m_partitionPath;

        if (pvInfoData.m_partitionPath != "unallocated" && pvInfoData.m_disktype != "unrecognized") {
            m_minSize += Utils::sectorToUnit(pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1,
                                             pvInfoData.m_sectorSize, type);
        }

        m_maxSize += Utils::sectorToUnit(pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1,
                                         pvInfoData.m_sectorSize, type);

        m_sumSize += (pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1) * pvInfoData.m_sectorSize;
        qDebug() << "Added to sumSize (bytes):" << (pvInfoData.m_sectorEnd - pvInfoData.m_sectorStart + 1) * pvInfoData.m_sectorSize;
    }


    m_selectSpaceLineEdit->lineEdit()->setPlaceholderText(QString("%1-%2").arg(QString::number(m_minSize, 'f', 2))
                                                          .arg(QString::number(m_maxSize, 'f', 2)));
//    m_selectSpaceLabel->setText(QString("(%1-%2)").arg(QString::number(m_minSize, 'f', 2))
//                                .arg(QString::number(m_maxSize, 'f', 2)) + m_selectSpaceComboBox->currentText());
    qDebug() << "Select space line edit placeholder updated.";

    m_curSize = m_curSize > m_sumSize ? m_sumSize : m_curSize;

    if (!m_selectSpaceLineEdit->text().isEmpty()) {
        qDebug() << "Select space line edit is not empty, updating text based on index.";
        switch (index) {
        case 0: {
            m_selectSpaceLineEdit->setText(QString::number(m_curSize / static_cast<double>(MEBIBYTE), 'f', 2));
            qDebug() << "Setting text to MiB.";
            break;
        }
        case 1: {
            m_selectSpaceLineEdit->setText(QString::number(m_curSize / static_cast<double>(GIBIBYTE), 'f', 2));
            qDebug() << "Setting text to GiB.";
            break;
        }
        case 2: {
            m_selectSpaceLineEdit->setText(QString::number(m_curSize / static_cast<double>(TEBIBYTE), 'f', 2));
            qDebug() << "Setting text to TiB.";
            break;
        }
        default:
            qDebug() << "Default case for index.";
            break;
        }
    }
    qDebug() << "Current index change handled.";
}

void CreateVGWidget::onTextChanged(const QString &text)
{
    qDebug() << "CreateVGWidget::onTextChanged called. Text:" << text;
    double curSize = QString::number(text.toDouble(), 'f', 2).toDouble();
    double maxSize = QString::number(m_maxSize, 'f', 2).toDouble();

    m_doneButton->setDisabled(text.isEmpty() || curSize == 0.00);
    m_selectSpaceLineEdit->setAlert(false);
    qDebug() << "Done button disabled state set, alert cleared.";

    if (!m_selectSpaceLineEdit->text().isEmpty()) {
        qDebug() << "Select space line edit is not empty, updating current size.";
        double curSize = QString::number(m_selectSpaceLineEdit->text().toDouble(), 'f', 2).toDouble();
        switch (m_selectSpaceComboBox->currentIndex()) {
        case 0: {
            m_curSize = static_cast<long long>(curSize * MEBIBYTE);
            break;
        }
        case 1: {
            m_curSize = static_cast<long long>(curSize * GIBIBYTE);
            break;
        }
        case 2: {
            m_curSize = static_cast<long long>(curSize * TEBIBYTE);
            break;
        }
        default:
            qDebug() << "Default case for current index.";
            break;
        }

        if (curSize >= maxSize) {
            m_curSize = m_sumSize;
            qDebug() << "Current size is greater than or equal to max size, setting current size to sum size.";
        }

        m_curSize = m_curSize > m_sumSize ? m_sumSize : m_curSize;
    }
    qDebug() << "Text changed handled.";
}

void CreateVGWidget::onVGCreateMessage(const QString &vgMessage)
{
    qDebug() << "Received VG create message:" << vgMessage;
    if (m_waterLoadingWidget != nullptr) {
        m_waterLoadingWidget->stopTimer();
        qDebug() << "Water loading widget timer stopped.";
    }

    QStringList infoList = vgMessage.split(":");
    if (infoList.count() <= 1) {
        qDebug() << "Info list count <= 1, closing dialog.";
        close();
        return;
    }

    if (infoList.at(0) == "0") {
        qWarning() << "VG operation failed with error code:" << infoList.at(1);
        QString text = "";
        switch (infoList.at(1).toInt()) {
        case LVMError::LVM_ERR_VG_ALREADY_EXISTS: {
            text = tr("Existing volume group, creation failed. Please retry after reboot."); // 逻辑卷组已存在，创建失败，请重启电脑再试
            qDebug() << "LVM_ERR_VG_ALREADY_EXISTS:" << text;
            break;
        }
        case LVMError::LVM_ERR_PV_CREATE_FAILED: {
            text = tr("Failed to create a physical volume. Please refresh Disk Utility and try again."); // 物理卷创建失败，请刷新磁盘管理器再试
            qDebug() << "LVM_ERR_PV_CREATE_FAILED:" << text;
            break;
        }
        case LVMError::LVM_ERR_IO_ERROR: {
            text = tr("Device input/output error. Please try again after reboot."); // 设备输入输出错误，请重启电脑再试
            qDebug() << "LVM_ERR_IO_ERROR:" << text;
            break;
        }
        default:
            qDebug() << "Unknown LVM error code.";
            break;
        }

        if (!text.isEmpty()) {
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(),
                                                     QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), text);
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(),
                                                           QMargins(0, 0, 0, 20));
            qDebug() << "Warning message sent.";
        }
    }

    qDebug() << "Closing CreateVGWidget";
    close();
}

void CreateVGWidget::onUpdateUsb()
{
    qDebug() << "USB device status changed, checking for updates...";

    QStringList deviceNameList = DMDbusHandler::instance()->getDeviceNameList();
    if (m_curDeviceNameList == deviceNameList) {
        qDebug() << "Device name list is unchanged, returning.";
        return;
    }

    // 是否有磁盘拔出
    QStringList deleteDevNameList;
    for (int i = 0; i < m_curDeviceNameList.size(); ++i) {
        QString devPath = m_curDeviceNameList.at(i);
        if (deviceNameList.indexOf(devPath) == -1) {
            deleteDevNameList.append(devPath);
            qDebug() << "Detected removed device:" << devPath;
        }
    }

    // 当前选择的磁盘信息列表里面是否包含拔出的磁盘
    QList<int> deleteIndexList;
    for (int i = 0; i < deleteDevNameList.size(); ++i) {
        QString devPath = deleteDevNameList.at(i);
        for (int j = 0; j < m_curSeclectData.size(); ++j) {
            PVInfoData pvInfoData = m_curSeclectData.at(j);
            if (pvInfoData.m_diskPath == devPath) {
                deleteIndexList.append(j);
                qDebug() << "Found removed device in selected data at index:" << j;
            }
        }
    }

    // 将拔出磁盘信息从当前选择的列表里剔除
    for (int i = 0; i < deleteIndexList.size(); ++i) {
        m_curSeclectData.removeAt(deleteIndexList.at(i));
        qDebug() << "Removed device from selected data at index:" << deleteIndexList.at(i);
    }

    if (m_stackedWidget->currentIndex() == 0) {
        qDebug() << "Current index is 0, updating disk data.";
        DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("://icons/deepin/builtin/ok.svg"),
                                                 tr("Refreshing the page to reload disks"));
        DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));

        qDebug() << "Refreshing disk selection UI due to USB change";
        updateData();
    } else if (m_stackedWidget->currentIndex() == 1) {
        qDebug() << "Current index is 1, updating selected data.";
        if (!deleteIndexList.isEmpty()) {
            qDebug() << "USB device removed, updating selected data UI.";
            DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("://icons/deepin/builtin/ok.svg"),
                                                     tr("Refreshing the page to reload disks"));
            DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));

            qDebug() << "Refreshing selected data UI due to USB change";
            updateSelectedData();
        }
    }

    m_curDeviceNameList = deviceNameList;
    qDebug() << "USB update handler finished.";
}

Byte_Value CreateVGWidget::getPVSize(const VGInfo &vg, const PVData &pv, bool flag)
{
    qDebug() << "CreateVGWidget::getPVSize called. PV path:" << pv.m_devicePath << ", flag:" << flag;
    LVMInfo lvmInfo =  DMDbusHandler::instance()->probLVMInfo();

    //判断
    if (lvmInfo.pvExists(pv)) {
        PVInfo pvInfo = lvmInfo.getPV(pv);
        if (pvInfo.joinVG()) { //如果pv已经加入vg 且加入的是别的vg 则返回-1
            qDebug() << "PV exists and joined another VG.";
            return lvmInfo.pvOfVg(vg, pv) ? pvInfo.m_pvByteTotalSize : -1;
        } else {
            qDebug() << "PV exists and not joined any VG, returning total byte size.";
            return pvInfo.m_pvByteTotalSize;
        }
    }

    auto it = DMDbusHandler::instance()->probDeviceInfo().find(pv.m_diskPath);
    if (it == DMDbusHandler::instance()->probDeviceInfo().end()) {
        qDebug() << "Device info not found for disk path:" << pv.m_diskPath;
        return -1;
    }

    DeviceInfo dev = it.value();

    long long startSec = pv.m_startSector;
    long long endSec = pv.m_endSector;

    if (startSec == 0 && pv.m_type == DEV_UNALLOCATED_PARTITION) {
        startSec = UEFI_SECTOR;
        qDebug() << "Unallocated partition at start sector 0, adjusting start sector to UEFI_SECTOR.";
    }

    if (pv.m_type == DEV_UNALLOCATED_PARTITION && pv.m_endSector == (dev.m_length - 1) && dev.m_disktype.contains("gpt")) {
        endSec -= GPTBACKUP;
        qDebug() << "Unallocated GPT partition at end of disk, adjusting end sector.";
    }

    if (pv.m_type == DEV_DISK && flag) {
        startSec -= UEFI_SECTOR;
        endSec -= GPTBACKUP;
        qDebug() << "Full disk and flag is true, adjusting start and end sectors.";
    }
    long long allByte = dev.m_sectorSize * (endSec - startSec + 1); //获取总的byte
    long long i = allByte % vg.m_PESize; //获取剩下的size大小

    if(i == 0){
        qDebug() << "Remaining size is 0, returning all bytes.";
        return  allByte;
    }else if(i >= MEBIBYTE){
        qDebug() << "Remaining size >= MiB, returning all bytes - remaining.";
        return allByte - i;
    }else  {
        qDebug() << "Remaining size < MiB, returning all bytes - remaining - 4 MiB.";
        return allByte - i - 4 * MEBIBYTE;
    }
   // return i == 0 ? allByte : ((i >= MEBIBYTE) ? allByte - i : allByte - i - 4 * MEBIBYTE); //去除末尾 保证是vg pe的倍数
}

Byte_Value CreateVGWidget::getDevSize(const VGInfo &vg, const PVData &pv, bool flag, long long size)
{
    qDebug() << "CreateVGWidget::getDevSize called. PV path:" << pv.m_devicePath << ", flag:" << flag << ", size:" << size;
    LVMInfo lvmInfo =  DMDbusHandler::instance()->probLVMInfo();

    //判断
    if (lvmInfo.pvExists(pv)) {
        PVInfo pvInfo = lvmInfo.getPV(pv);
        if (pvInfo.joinVG()) { //如果pv已经加入vg 且加入的是别的vg 则返回-1
            qDebug() << "PV exists and joined another VG, returning -1 if not of current VG, else total size.";
            return lvmInfo.pvOfVg(vg, pv) ? (pv.m_endSector - pv.m_startSector + 1) * pv.m_sectorSize : -1;
        }
    }

    auto it = DMDbusHandler::instance()->probDeviceInfo().find(pv.m_diskPath);
    if (it == DMDbusHandler::instance()->probDeviceInfo().end()) {
        qDebug() << "Device info not found for disk path:" << pv.m_diskPath << ", returning -1.";
        return -1;
    }

    DeviceInfo dev = it.value();

    long long startSec = pv.m_startSector;
    long long endSec = pv.m_endSector;
    if (startSec == 0 && pv.m_type == DEV_UNALLOCATED_PARTITION) {
        startSec = UEFI_SECTOR;
        qDebug() << "Unallocated partition at start sector 0, adjusting start sector to UEFI_SECTOR.";
    }

    if (pv.m_type == DEV_UNALLOCATED_PARTITION && pv.m_endSector == (dev.m_length - 1) && dev.m_disktype.contains("gpt")) {
        endSec -= GPTBACKUP;
        qDebug() << "Unallocated GPT partition at end of disk, adjusting end sector.";
    }

    if (pv.m_type == DEV_DISK && flag) {
        startSec -= UEFI_SECTOR;
        endSec -= GPTBACKUP;
        qDebug() << "Full disk and flag is true, adjusting start and end sectors.";
    }

    long long allSize = size;
    long long tmpSize = size % vg.m_PESize;
    if (tmpSize > 0) {
        qDebug() << "Size not divisible by PESize, rounding up.";
        allSize += vg.m_PESize;
    }
    if (allSize == vg.m_PESize) {
        qDebug() << "Size is equal to PESize, adding PESize to allSize.";
        allSize += vg.m_PESize;
    }

    long long allSec = allSize / dev.m_sectorSize + 1;

    if ((endSec - startSec + 1) < allSec) {
        qDebug() << "Not enough space for size, returning -1.";
        return -1;
    }
    endSec = startSec + allSec - 1;
    return (endSec - startSec + 1) * dev.m_sectorSize;
}

Byte_Value CreateVGWidget::getMaxSize(const VGInfo &vg, const set<PVData> &pvlist)
{
    qDebug() << "CreateVGWidget::getMaxSize called.";
    Byte_Value maxSize = 0;
    foreach (const PVData &pv, pvlist) { //这里计算设备真实大小 跟getpvsize不同
        maxSize += (pv.m_endSector - pv.m_startSector + 1) * pv.m_sectorSize;
    }
    qDebug() << "Calculated max size";
    return maxSize;
}

Byte_Value CreateVGWidget::getMinSize(const VGInfo &vg, const set<PVData> &pvlist)
{
    qDebug() << "CreateVGWidget::getMinSize called.";
    QList<PVData>diskList;
    QList<PVData>pvDiskList;
    QList<PVData>partList;
    QList<PVData>unallocList;
    QList<PVData>loopList;
    QList<PVData>metaList;

    Byte_Value vgUsed = vg.m_peUsed * vg.m_PESize;
    Byte_Value minSize = 0 ;

    if (vgUsed > getMaxSize(vg, pvlist)) { //vg使用大小超过最大值
        qDebug() << "VG used size exceeds max size, returning VG used size.";
        return vgUsed;
    }

    LVMInfo lvmInfo =  DMDbusHandler::instance()->probLVMInfo();

    foreach (PVData pv, pvlist) {
        switch (pv.m_type) {
        case DevType::DEV_DISK:
            lvmInfo.pvExists(pv) ? pvDiskList.push_back(pv) : diskList.push_back(pv);
            break;
        case DevType::DEV_PARTITION:
            partList.push_back(pv);
            break;
        case DevType::DEV_UNALLOCATED_PARTITION:
            unallocList.push_back(pv);
            break;
        case DevType::DEV_LOOP:
            loopList.push_back(pv);
            break;
        case DevType::DEV_META_DEVICES:
            metaList.push_back(pv);
            break;
        }
    }

    //如果比所有分区之和以及pv设备小 那么最小值为分区之和+pv设备
    //计算设备类型是分区时的最小值
    foreach (const PVData &pv, partList) {
        Byte_Value size = getPVSize(vg, pv);

        if (size < 0) {
            return -1;
        }
        if (size < vg.m_PESize) {
            continue; //小于一个pe大小 不计算
        }
        minSize += (pv.m_endSector - pv.m_startSector + 1) * pv.m_sectorSize;
    }
    //计算设备类型是pv时的最小值
    foreach (const PVData &pv, pvDiskList) {
        Byte_Value size = getPVSize(vg, pv, false); //获取pv设备大小

        if (size < 0) {
            return -1;
        }
        if (size < vg.m_PESize) {
            continue; //小于一个pe大小 不计算
        }
        minSize += (pv.m_endSector - pv.m_startSector + 1) * pv.m_sectorSize;
    }

    vgUsed -= minSize;

    if (vgUsed <= 0) {
        qDebug() << "VG used size is less than or equal to minimum size, returning minimum size.";
        return minSize;
    }

    //计算设备类型是未分配空间时的最小值
    foreach (const PVData &pv, unallocList) {
        Byte_Value size = getPVSize(vg, pv); //获取pv设备大小
        if (size < 0) {
            return -1;
        }
        if (size < vg.m_PESize) {
            continue; //小于一个pe大小 不计算
        }

        if (vgUsed <= size) {//vg使用<=当前pv设备时 返回
            if (vgUsed < vg.m_PESize) {
                return minSize + getDevSize(vg, pv, true, vg.m_PESize); //至少是4M
            }
            return minSize + getDevSize(vg, pv, true, vgUsed); //至少是4M
        }
        vgUsed -= size;
        minSize += getDevSize(vg, pv, true, vgUsed);
    }

    //计算设备类型是磁盘时的最小值
    foreach (const PVData &pv, diskList) {
        Byte_Value size = getPVSize(vg, pv, false); //磁盘加入pv大小
        Byte_Value size2 = getPVSize(vg, pv); //分区加入pv的大小
        if (size < 0) {
            return -1;
        }

        if (size < vg.m_PESize) {
            continue; //小于一个pe大小 不计算
        }

        if (vgUsed <= size) {
            if (vgUsed >= size2) { //大于分区加入时的大小
                return minSize + getDevSize(vg, pv, false, vgUsed); //全盘加入
            } else {
                if (vgUsed < vg.m_PESize) {
                    return minSize + getDevSize(vg, pv, false, vg.m_PESize);
                }
                return minSize + getDevSize(vg, pv, false, vgUsed);
            }
        }
        vgUsed -= size;
        minSize += getDevSize(vg, pv, true, vgUsed);;
    }

    if (vgUsed > 0) {
        qDebug() << "VG used size is greater than minimum size, returning VG used size.";
        return vg.m_peUsed * vg.m_PESize;
    }

    qDebug() << "Minimum size calculation finished, returning minimum size.";
    return minSize;
}

bool CreateVGWidget::adjudicationPVMove(const VGInfo &vg, const set<PVData> &pvlist, bool &bigDataMove, QStringList &realDelPvList)
{
    qDebug() << "CreateVGWidget::adjudicationPVMove called.";
    LUKSMap luks = DMDbusHandler::instance()->probLUKSInfo();

    bigDataMove = false;
    auto pvInfoMap = vg.m_pvInfo;
    Byte_Value size = 0;
    //获取所有删除的pv
    foreach (auto it, pvInfoMap) {
        bool isDelete = true;
        QString luksDev;
        if (luks.mapperExists(it.m_pvPath)) { //如果是映射设备
            luksDev = luks.getDevPath(it.m_pvPath);
        }

        foreach (const PVData &pv, pvlist) {
            if (it.m_pvPath == pv.m_devicePath || (luksDev ==pv.m_devicePath&&!luksDev.isEmpty())) {
                qDebug() << "PV" << it.m_pvPath << "is not deleted. break.";
                isDelete = false;
                break;
            }
        }
        if (isDelete) {
            if (it.m_pvUsedPE * it.m_PESize > 0) {   //pv中存在数据
                size += it.m_pvUsedPE * it.m_PESize; //获取pv真实使用大小
                realDelPvList.push_back(it.m_pvPath);
            }
        }
    }

    if (getMinSize(vg, pvlist) > getMaxSize(vg, pvlist)) {
        qDebug() << "VG used size exceeds max size, returning false.";
        return false;
    }

    if (((size / GIBIBYTE) >= 1)) { //判断是否存在大量数据需要移动
        qDebug() << "There are large amounts of data to move, returning true.";
        bigDataMove = true;
    }

    qDebug() << "CreateVGWidget::adjudicationPVMove finished.";
    return true;
}
