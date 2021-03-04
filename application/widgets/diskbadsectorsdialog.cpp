/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskbadsectorsdialog.cpp
 *
 * @brief 坏道检测与修复主界面窗口
 *
 * @date 2020-12-22 15:56
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
#include "diskbadsectorsdialog.h"
#include "common.h"
#include "cylinderwidget.h"
#include "cylinderinfowidget.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DHorizontalLine>
#include <DFrame>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DMessageManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QStackedWidget>
#include <QSettings>
#include <QFile>

DiskBadSectorsDialog::DiskBadSectorsDialog(QWidget *parent) : DDialog(parent)
{
    initUI();
    initConnections();
}

void DiskBadSectorsDialog::initUI()
{
    setIcon(QIcon::fromTheme(appName));
    setTitle(tr("Verify or repair bad sectors")); // 坏道检测与修复
    setFixedSize(635, 757);

    m_curType = StatusType::Normal;

    DPalette palette1;
    QColor color1("#000000");
    color1.setAlphaF(0.7);
    palette1.setColor(DPalette::NoType, color1);

    DPalette palette2;
    QColor color2("#526A7F");
    palette2.setColor(DPalette::NoType, color2);

    DPalette palette3;
    QColor color3("#000000");
    color3.setAlphaF(0.85);
    palette3.setColor(DPalette::NoType, color3);

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
             << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
             << info.sector_size << info.max_prims << info.highest_busy << info.readonly
             << info.max_partition_name_length;

    DLabel *verifyLabel = new DLabel(tr("Verify:")); // 检测范围
    DFontSizeManager::instance()->bind(verifyLabel, DFontSizeManager::T6, QFont::Normal);
    verifyLabel->setFixedWidth(74);
    verifyLabel->setPalette(palette1);
//    verifyLabel->setStyleSheet("background:yellow");
    m_verifyComboBox = new DComboBox;
    m_verifyComboBox->addItem(tr("Cylinders")); // 柱面范围
    m_verifyComboBox->addItem(tr("Sectors")); // 扇区范围
    m_verifyComboBox->addItem(tr("MB")); // 容量范围
    m_verifyComboBox->setFixedSize(155, 36);

    QRegExp reg("[0-9]+$"); // 只能输入数字正则表达式
    QRegExpValidator *validator = new QRegExpValidator(reg, this);

//    QIntValidator *intValidator = new QIntValidator(0, QString("%1").arg(info.cylinders).toInt(), this);

    m_startLineEdit = new DLineEdit;
    m_startLineEdit->setFixedSize(172, 36);
    m_startLineEdit->setText("0");
    m_startLineEdit->lineEdit()->setPlaceholderText("0");
    m_startLineEdit->lineEdit()->setValidator(validator);
//    m_startLineEdit->lineEdit()->setValidator(intValidator);

    m_endLineEdit = new DLineEdit;
    m_endLineEdit->setFixedSize(172, 36);
    m_endLineEdit->setText(QString("%1").arg(info.cylinders));
    m_endLineEdit->lineEdit()->setPlaceholderText(QString("%1").arg(info.cylinders));
    m_endLineEdit->lineEdit()->setValidator(validator);

    DLabel *lineLabel = new DLabel("—");

//    m_verifyLabel = new DLabel(QString("(0-%1)").arg(info.cylinders));
//    DFontSizeManager::instance()->bind(m_verifyLabel, DFontSizeManager::T8, QFont::Normal);
//    m_verifyLabel->setPalette(palette2);

    QHBoxLayout *verifyLayout = new QHBoxLayout;
    verifyLayout->addWidget(verifyLabel);
    verifyLayout->addWidget(m_verifyComboBox);
    verifyLayout->addSpacing(10);
    verifyLayout->addWidget(m_startLineEdit);
    verifyLayout->addSpacing(10);
    verifyLayout->addWidget(lineLabel);
    verifyLayout->addSpacing(10);
    verifyLayout->addWidget(m_endLineEdit);
//    verifyLayout->addSpacing(7);
//    verifyLayout->addWidget(m_verifyLabel);
    verifyLayout->setSpacing(0);
    verifyLayout->setContentsMargins(0, 0, 0, 0);

    DLabel *methodLabel = new DLabel(tr("Method:")); // 检测方式
    DFontSizeManager::instance()->bind(methodLabel, DFontSizeManager::T6, QFont::Normal);
    methodLabel->setFixedWidth(74);
    methodLabel->setPalette(palette1);

    m_methodComboBox = new DComboBox;
    m_methodComboBox->addItem(tr("Verifying times")); // 检测次数
    m_methodComboBox->addItem(tr("Timeout")); // 超时时间
    m_methodComboBox->setFixedSize(155, 36);

    m_slider = new DSlider(Qt::Horizontal);
    m_slider->setFixedWidth(97);
    m_slider->setMinimum(1);
    m_slider->setMaximum(100);
    m_slider->setValue(50);

    QRegExp regCheckTimes("^[1-9][0-6]$"); // 只能输入两位数但首字符不能为0正则表达式
    QRegExpValidator *validatorCheckTimes = new QRegExpValidator(regCheckTimes, this);

    m_checkTimesEdit = new DLineEdit;
    m_checkTimesEdit->setText("8");
    m_checkTimesEdit->setFixedSize(100, 36);
    m_checkTimesEdit->lineEdit()->setValidator(validatorCheckTimes);

    DLabel *checkTimesLabel = new DLabel("(1-16)");
    DFontSizeManager::instance()->bind(checkTimesLabel, DFontSizeManager::T8, QFont::Normal);
    checkTimesLabel->setPalette(palette2);

    QHBoxLayout *checkTimesLayout = new QHBoxLayout;
    checkTimesLayout->addWidget(m_slider);
    checkTimesLayout->addSpacing(10);
    checkTimesLayout->addWidget(m_checkTimesEdit);
    checkTimesLayout->addSpacing(10);
    checkTimesLayout->addWidget(checkTimesLabel);
    checkTimesLayout->addStretch();
    checkTimesLayout->setSpacing(0);
    checkTimesLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *checkTimesWidget = new QWidget;
    checkTimesWidget->setLayout(checkTimesLayout);

    m_timeoutEdit = new DLineEdit;
    m_timeoutEdit->setText("3000");
    m_timeoutEdit->lineEdit()->setValidator(new QIntValidator(100, 3000, this));
    m_timeoutEdit->setFixedSize(144, 36);
    DLabel *timeoutLabel = new DLabel(tr("ms"));
    DLabel *timeoutRangeLabel = new DLabel("(100-3000)");
    DFontSizeManager::instance()->bind(timeoutRangeLabel, DFontSizeManager::T8, QFont::Normal);
    timeoutRangeLabel->setPalette(palette2);

    QHBoxLayout *timeoutLayout = new QHBoxLayout;
    timeoutLayout->addWidget(m_timeoutEdit);
    timeoutLayout->addSpacing(10);
    timeoutLayout->addWidget(timeoutLabel);
    timeoutLayout->addSpacing(10);
    timeoutLayout->addWidget(timeoutRangeLabel);
    timeoutLayout->addStretch();
    timeoutLayout->setSpacing(0);
    timeoutLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *timeoutWidget = new QWidget;
    timeoutWidget->setLayout(timeoutLayout);

    m_methodStackedWidget = new QStackedWidget;
    m_methodStackedWidget->addWidget(checkTimesWidget);
    m_methodStackedWidget->addWidget(timeoutWidget);
    m_methodStackedWidget->setFixedHeight(36);

    QHBoxLayout *methodLayout = new QHBoxLayout;
    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(m_methodComboBox);
    methodLayout->addSpacing(10);
    methodLayout->addWidget(m_methodStackedWidget);
    methodLayout->setSpacing(0);
    methodLayout->setContentsMargins(0, 0, 0, 0);

    DHorizontalLine *horizontalLine = new DHorizontalLine;

    DLabel *resultLabel = new DLabel(tr("Result:")); // 检测结果
    DFontSizeManager::instance()->bind(resultLabel, DFontSizeManager::T7, QFont::Medium);
    resultLabel->setPalette(palette3);
    m_checkInfoLabel = new DLabel;
    DFontSizeManager::instance()->bind(m_checkInfoLabel, DFontSizeManager::T7, QFont::Medium);
    m_checkInfoLabel->setPalette(palette3);

    QHBoxLayout *resultLayout = new QHBoxLayout;
    resultLayout->addWidget(resultLabel);
    resultLayout->addWidget(m_checkInfoLabel);
    resultLayout->addStretch();
    resultLayout->setSpacing(0);
    resultLayout->setContentsMargins(0, 0, 0, 0);

//    DFrame *frame = new DFrame;
    m_cylinderInfoWidget = new CylinderInfoWidget(QString("%1").arg(info.cylinders).toInt());
    m_cylinderInfoWidget->setMinimumSize(615, 380);

    CylinderWidget *excellentWidget = new CylinderWidget;
    excellentWidget->setFixedSize(20, 20);
    excellentWidget->setFrameShape(QFrame::Box);
    DLabel *excellentLabel = new DLabel(tr("Excellent"));
    DFontSizeManager::instance()->bind(excellentLabel, DFontSizeManager::T8, QFont::Normal);
    excellentLabel->setPalette(palette2);

    CylinderWidget *damagedWidget = new CylinderWidget;
    damagedWidget->setFixedSize(20, 20);
    damagedWidget->setFrameShape(QFrame::Box);
    DLabel *damagedLabel = new DLabel(tr("Damaged"));
    DFontSizeManager::instance()->bind(damagedLabel, DFontSizeManager::T8, QFont::Normal);
    damagedLabel->setPalette(palette2);

    CylinderWidget *unknownWidget = new CylinderWidget;
    unknownWidget->setFixedSize(20, 20);
    unknownWidget->setFrameShape(QFrame::Box);
    DLabel *unknownLabel = new DLabel(tr("Unknown"));
    DFontSizeManager::instance()->bind(unknownLabel, DFontSizeManager::T8, QFont::Normal);
    unknownLabel->setPalette(palette2);

    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        excellentWidget->setStyleSheet(QString("background:#2B6AE3;border:0px"));
        damagedWidget->setStyleSheet(QString("background:#C41E1E;border:0px"));
        unknownWidget->setStyleSheet(QString("background:#909090;border:0px"));
    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        excellentWidget->setStyleSheet(QString("background:#6097FF;border:0px"));
        damagedWidget->setStyleSheet(QString("background:#E23C3C;border:0px"));
        unknownWidget->setStyleSheet(QString("background:#777990;border:0px"));
    }

    QHBoxLayout *cylinderLayout = new QHBoxLayout;
    cylinderLayout->addWidget(excellentWidget);
    cylinderLayout->addSpacing(10);
    cylinderLayout->addWidget(excellentLabel);
    cylinderLayout->addStretch();
    cylinderLayout->addWidget(damagedWidget);
    cylinderLayout->addSpacing(10);
    cylinderLayout->addWidget(damagedLabel);
    cylinderLayout->addStretch();
    cylinderLayout->addWidget(unknownWidget);
    cylinderLayout->addSpacing(10);
    cylinderLayout->addWidget(unknownLabel);
    cylinderLayout->setContentsMargins(10, 10, 10, 14);

    m_exitButton = new DPushButton(tr("Exit")); // 退出
    m_exitButton->setFixedSize(147, 36);
    m_exitButton->setObjectName("exit");
    m_exitButton->setAccessibleName("exit");

    m_resetButton = new DPushButton(tr("Reset")); // 复位
    m_resetButton->setFixedSize(147, 36);
    m_resetButton->setDisabled(true);
    m_resetButton->setObjectName("reset");
    m_resetButton->setAccessibleName("reset");

    m_repairButton = new DPushButton(tr("Repair")); // 尝试修复
    m_repairButton->setFixedSize(147, 36);
    m_repairButton->setDisabled(true);
    m_repairButton->setObjectName("repair");
    m_repairButton->setAccessibleName("repair");

    m_startButton = new DSuggestButton(tr("Start Verify")); // 开始检测
    m_startButton->setFixedSize(147, 36);
    m_startButton->setObjectName("start");
    m_startButton->setAccessibleName("start");

    m_stopButton = new DPushButton(tr("Stop")); // 停止
    m_stopButton->setFixedSize(147, 36);
    m_stopButton->setObjectName("stop");
    m_stopButton->setAccessibleName("stop");

    m_continueButton = new DPushButton(tr("Continue")); // 继续
    m_continueButton->setFixedSize(147, 36);
    m_continueButton->setObjectName("continue");
    m_continueButton->setAccessibleName("continue");

    m_againButton = new DPushButton(tr("Verify Again"));  // 重新检测
    m_againButton->setFixedSize(147, 36);
    m_againButton->setObjectName("again");
    m_againButton->setAccessibleName("again");

    m_doneButton = new DSuggestButton(tr("Done"));  // 完成
    m_doneButton->setFixedSize(147, 36);
    m_doneButton->setObjectName("done");
    m_doneButton->setAccessibleName("done");

    m_buttonStackedWidget = new QStackedWidget;
    m_buttonStackedWidget->setFixedHeight(36);
    m_buttonStackedWidget->addWidget(m_startButton);
    m_buttonStackedWidget->addWidget(m_stopButton);
    m_buttonStackedWidget->addWidget(m_continueButton);
    m_buttonStackedWidget->addWidget(m_againButton);
    m_buttonStackedWidget->addWidget(m_doneButton);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_exitButton);
    buttonLayout->addSpacing(9);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addSpacing(9);
    buttonLayout->addWidget(m_repairButton);
    buttonLayout->addSpacing(9);
    buttonLayout->addWidget(m_buttonStackedWidget);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    m_progressBar = new DProgressBar;
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(8);

    m_usedTimeLabel = new DLabel(tr("Time elapsed:") + "00:00:12");
    DFontSizeManager::instance()->bind(m_usedTimeLabel, DFontSizeManager::T8, QFont::Normal);
    m_usedTimeLabel->setPalette(palette3);

    m_unusedTimeLabel = new DLabel(tr("Time left:") + "00:00:12");
    DFontSizeManager::instance()->bind(m_unusedTimeLabel, DFontSizeManager::T8, QFont::Normal);
    m_unusedTimeLabel->setPalette(palette3);

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->addWidget(m_usedTimeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(m_unusedTimeLabel);
    timeLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *progressLayout = new QVBoxLayout;
    progressLayout->addWidget(m_progressBar);
    progressLayout->addLayout(timeLayout);
    progressLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *progressWidget = new QWidget;
    progressWidget->setLayout(progressLayout);
    progressWidget->setFixedHeight(40);
    m_progressBar->hide();
    m_usedTimeLabel->hide();
    m_unusedTimeLabel->hide();

    QWidget *bottomWidget = new QWidget;
//    m_methodStackedWidget->setStyleSheet("background:red");
    QVBoxLayout *mainLayout = new QVBoxLayout(bottomWidget);
    mainLayout->addLayout(verifyLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(methodLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(horizontalLine);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(resultLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_cylinderInfoWidget);
    mainLayout->addLayout(cylinderLayout);
    mainLayout->addWidget(progressWidget);
    mainLayout->addSpacing(25);
//    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    addSpacing(10);
    addContent(bottomWidget);

    m_settings = new QSettings("/tmp/CheckData.conf", QSettings::IniFormat, this);

}

void DiskBadSectorsDialog::initConnections()
{
    connect(m_verifyComboBox, static_cast<void (DComboBox:: *)(const int)>(&DComboBox::currentIndexChanged),
            this, &DiskBadSectorsDialog::onVerifyChanged);
    connect(m_methodComboBox, static_cast<void (DComboBox:: *)(const int)>(&DComboBox::currentIndexChanged),
            this, &DiskBadSectorsDialog::onMethodChanged);
    connect(m_slider, &DSlider::valueChanged, this, &DiskBadSectorsDialog::onSliderValueChanged);
    connect(m_checkTimesEdit, &DLineEdit::textChanged, this, &DiskBadSectorsDialog::oncheckTimesChanged);
    connect(m_startButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onStartVerifyButtonClicked);
    connect(m_stopButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onStopButtonClicked);
    connect(m_continueButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onContinueButtonClicked);
    connect(m_againButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onAgainVerifyButtonClicked);
    connect(m_resetButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onResetButtonClicked);
    connect(m_repairButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onRepairButtonClicked);
    connect(m_exitButton, &DPushButton::clicked, this, &DiskBadSectorsDialog::onExitButtonClicked);
    connect(m_doneButton, &DSuggestButton::clicked, this, &DiskBadSectorsDialog::onDoneButtonClicked);
    connect(DMDbusHandler::instance(), &DMDbusHandler::checkBadBlocksCountInfo, this, &DiskBadSectorsDialog::onCheckBadBlocksInfo);
    connect(DMDbusHandler::instance(), &DMDbusHandler::repairBadBlocksInfo, this, &DiskBadSectorsDialog::onRepairBadBlocksInfo);
    connect(DMDbusHandler::instance(), &DMDbusHandler::checkBadBlocksFinished,  this, &DiskBadSectorsDialog::onCheckCoomplete);
    connect(DMDbusHandler::instance(), &DMDbusHandler::fixBadBlocksFinished,  this, &DiskBadSectorsDialog::onRepairCoomplete);
    connect(&m_timer, &QTimer::timeout, this, &DiskBadSectorsDialog::onTimeOut);
}

void DiskBadSectorsDialog::onVerifyChanged(int index)
{
    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

    m_startLineEdit->setText("0");
    m_endLineEdit->setAlert(false);
    m_startLineEdit->setAlert(false);

    switch (index) {
    case 0: {
        m_endLineEdit->lineEdit()->setPlaceholderText(QString("%1").arg(info.cylinders));
        m_endLineEdit->setText(QString("%1").arg(info.cylinders));
        break;
    }
    case 1: {
        m_endLineEdit->lineEdit()->setPlaceholderText(QString("%1").arg(info.length));
        m_endLineEdit->setText(QString("%1").arg(info.length));
        break;
    }
    case 2: {
        int value = info.length * info.sector_size / 1024 / 1024;
        m_endLineEdit->lineEdit()->setPlaceholderText(QString("%1").arg(value));
        m_endLineEdit->setText(QString("%1").arg(value));
        break;
    }
    default:
        break;
    }
}

void DiskBadSectorsDialog::onMethodChanged(int index)
{
    m_methodStackedWidget->setCurrentIndex(index);
    switch (index) {
    case 0: {
        m_timeoutEdit->setText("3000");
        m_timeoutEdit->setAlert(false);
        break;
    }
    case 1: {
        m_checkTimesEdit->setText("8");
        m_checkTimesEdit->setAlert(false);
        break;
    }
    default:
        break;
    }
}

void DiskBadSectorsDialog::onSliderValueChanged(int value)
{
    int count = QString::number((float)value / 100 * 16, 'f', 0).toInt();
    count <= 0 ? count = 1 : count;

    m_checkTimesEdit->setText(QString("%1").arg(count));
}

void DiskBadSectorsDialog::oncheckTimesChanged(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    int value = QString::number(text.toFloat() / 16, 'f', 2).toFloat() * 100;
    m_slider->setValue(value);
}

bool DiskBadSectorsDialog::inputValueIsEffective()
{
    bool isEffective = true;

    long long start = m_startLineEdit->lineEdit()->placeholderText().toLongLong();
    long long end = m_endLineEdit->lineEdit()->placeholderText().toLongLong();
    long long startValue = m_startLineEdit->text().toLongLong();
    long long endValue = m_endLineEdit->text().toLongLong();

    // 判断当前输入的检测范围的起始值是否在范围内，若不在就设为警告模式
    if (m_startLineEdit->text().isEmpty()) {
        m_startLineEdit->setAlert(true);
        isEffective = false;
    } else {
        if (startValue >= start && startValue <= end) {
            m_startLineEdit->setAlert(false);
        } else {
            m_startLineEdit->setAlert(true);
            isEffective = false;
        }
    }

    // 判断当前输入的检测范围的结束值是否在范围内，若不在就设为警告模式
    if (m_endLineEdit->text().isEmpty()) {
        m_endLineEdit->setAlert(true);
        isEffective = false;
    } else {
        if (endValue >= start && endValue <= end) {
            m_endLineEdit->setAlert(false);
        } else {
            m_endLineEdit->setAlert(true);
            isEffective = false;
        }
    }

    // 判断检测范围的起始值和结束值是否合理，若不合理就设为警告模式
    if (!m_endLineEdit->text().isEmpty() && !m_startLineEdit->text().isEmpty()) {
        if (startValue > endValue) {
            m_endLineEdit->setAlert(true);
            m_startLineEdit->setAlert(true);
            isEffective = false;
        } else {
            if(isEffective) {
                m_endLineEdit->setAlert(false);
                m_startLineEdit->setAlert(false);
            }
        }
    }

    switch (m_methodComboBox->currentIndex()) {
    case 0: { // 判断检测次数是否在范围内
        if (m_checkTimesEdit->text().isEmpty()) {
            m_checkTimesEdit->setAlert(true);
            isEffective = false;
        } else {
            int checkNumber = m_checkTimesEdit->text().toInt();
            if (checkNumber < 1 || checkNumber > 16) {
                m_checkTimesEdit->setAlert(true);
                isEffective = false;
            } else {
                m_checkTimesEdit->setAlert(false);
            }
        }
        break;
    }
    case 1: { // 判断超时时间是否在范围内
        if (m_timeoutEdit->text().isEmpty()) {
            m_timeoutEdit->setAlert(true);
            isEffective = false;
        } else {
            int checkNumber = m_timeoutEdit->text().toInt();
            if (checkNumber < 100 || checkNumber > 3000) {
                m_timeoutEdit->setAlert(true);
                isEffective = false;
            } else {
                m_timeoutEdit->setAlert(false);
            }
        }
        break;
    }
    default:
        break;
    }

    return isEffective;
}

void DiskBadSectorsDialog::onStartVerifyButtonClicked()
{
    if (!inputValueIsEffective()) {
        return;
    }

    m_curType = StatusType::Check;
    m_buttonStackedWidget->setCurrentIndex(1);
    m_progressBar->show();
    m_usedTimeLabel->show();
    m_unusedTimeLabel->show();
    m_resetButton->setDisabled(true);
    m_repairButton->setDisabled(true);
    m_progressBar->setValue(0);
    m_usedTimeLabel->setText(tr("Time elapsed:") + "00:00:00");
    m_unusedTimeLabel->setText(tr("Time left:") + "00:00:00");

    QFile file("/tmp/CheckData.conf");
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "[SettingData]\n";
        out << "[CheckData]\n";
        out << "[BadSectorsData]\n";
        out << "BadSectors=""\n";
        file.close();
    }

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

    int checkNumber = 8;
    switch (m_methodComboBox->currentIndex()) {
    case 0: {
        checkNumber = m_checkTimesEdit->text().toInt();
        break;
    }
    case 1: {
        checkNumber = m_timeoutEdit->text().toInt();
        break;
    }
    default:
        break;
    }

    switch (m_verifyComboBox->currentIndex()) {
    case 0: {
        int blockStart = m_startLineEdit->text().toInt();
        int blockEnd = m_endLineEdit->text().toInt();
        int checkSize = static_cast<int>(info.heads * info.sectors * info.sector_size);

        m_totalCheckNumber = blockEnd - blockStart + 1;
        m_cylinderInfoWidget->setCylinderNumber(m_totalCheckNumber);

        m_settings->beginGroup("SettingData");
        m_settings->setValue("BlockStart", blockStart);
        m_settings->setValue("BlockEnd", blockEnd);
        m_settings->setValue("CheckSize", checkSize);
        m_settings->setValue("CheckNumber", checkNumber);
        m_settings->endGroup();

        DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart, blockEnd, checkNumber, checkSize, 1);
        break;
    }
    case 1: {
        int blockStart = static_cast<int>(m_startLineEdit->text().toLongLong() / info.cylsize);
        int blockEnd = static_cast<int>(m_endLineEdit->text().toLongLong() / info.cylsize);
        int checkSize = static_cast<int>(info.heads * info.sectors * info.sector_size);

        m_totalCheckNumber = blockEnd - blockStart + 1;
        m_cylinderInfoWidget->setCylinderNumber(m_totalCheckNumber);

        m_settings->beginGroup("SettingData");
        m_settings->setValue("BlockStart",blockStart);
        m_settings->setValue("BlockEnd",blockEnd);
        m_settings->setValue("CheckSize",checkSize);
        m_settings->setValue("CheckNumber",checkNumber);
        m_settings->endGroup();

        DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart, blockEnd, checkNumber, checkSize, 1);
        break;
    }
    case 2: {
        int checkSize = static_cast<int>(info.heads * info.sectors * info.sector_size);
        int blockStart = static_cast<int>(m_startLineEdit->text().toLongLong() * 1024 * 1024 / checkSize);
        int blockEnd = static_cast<int>(m_endLineEdit->text().toLongLong() * 1024 * 1024 / checkSize);

        m_totalCheckNumber = blockEnd - blockStart + 1;
        m_cylinderInfoWidget->setCylinderNumber(m_totalCheckNumber);

        m_settings->beginGroup("SettingData");
        m_settings->setValue("BlockStart",blockStart);
        m_settings->setValue("BlockEnd",blockEnd);
        m_settings->setValue("CheckSize",checkSize);
        m_settings->setValue("CheckNumber",checkNumber);
        m_settings->endGroup();

        DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart, blockEnd, checkNumber, checkSize, 1);
        break;
    }
    default:
        break;
    }

}

void DiskBadSectorsDialog::mSecsToTime(qint64 msecs, qint64 &hour, qint64 &minute, qint64 &second)
{
    hour = msecs / (60 * 60 * 1000);
    minute = (msecs % (60 * 60 * 1000)) / (60 * 1000);
    second = ((msecs % (60 * 60 * 1000)) % (60 * 1000)) / 1000;
}

void DiskBadSectorsDialog::onCheckBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo)
{
    ++m_curCheckNumber;
    m_curCheckTime += cylinderTimeConsuming.toLongLong();
    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    QString LBANumber = QString::number(cylinderNumber.toLongLong() * info.heads * info.sectors);
    m_checkInfoLabel->setText(tr("Verifying cylinder: %1").arg(cylinderNumber)); // 正在检测xxx柱面

    qint64 totalTime = m_curCheckTime / m_curCheckNumber * m_totalCheckNumber;
    int value = QString::number((float)m_curCheckTime / totalTime,'f', 2).toFloat() * 100;
    value > 99 ? value = 99 : value;

    m_progressBar->setValue(value);
    qint64 remainingTime = totalTime - m_curCheckTime;
    remainingTime < 1000 ? remainingTime = 1000 : remainingTime;

    qint64 usedHour = 0;
    qint64 usedMinute = 0;
    qint64 usedSecond = 0;
    mSecsToTime(m_curCheckTime, usedHour, usedMinute, usedSecond);
    m_usedTimeLabel->setText(tr("Time elapsed:") + QString("%1:%2:%3").arg(usedHour, 2, 10, QLatin1Char('0')).arg(usedMinute, 2, 10, QLatin1Char('0')).arg(usedSecond, 2, 10, QLatin1Char('0'))); // 时、分、秒为一位数时，十位自动补0

    qint64 remainingHour = 0;
    qint64 remainingMinute = 0;
    qint64 remainingSecond = 0;
    mSecsToTime(remainingTime, remainingHour, remainingMinute, remainingSecond);
    m_unusedTimeLabel->setText(tr("Time left:") + QString("%1:%2:%3").arg(remainingHour, 2, 10, QLatin1Char('0')).arg(remainingMinute, 2, 10, QLatin1Char('0')).arg(remainingSecond, 2, 10, QLatin1Char('0')));
//    qDebug() << "111111111" << cylinderTimeConsuming << m_curCheckTime << value << m_usedTimeLabel->text() << remainingTime << QTime::fromMSecsSinceStartOfDay(remainingTime).toString("hh:mm:ss");

    m_settings->beginGroup("CheckData");
    m_settings->setValue(cylinderNumber, QString("%1,%2,%3,%4,%5,0").arg(LBANumber).arg(cylinderNumber).arg(cylinderTimeConsuming).arg(cylinderStatus).arg(cylinderErrorInfo));
    m_settings->endGroup();

    m_settings->beginGroup("SettingData");
    m_settings->setValue("CurCylinder",cylinderNumber);
    m_settings->endGroup();

    if (cylinderStatus == "bad") {
        QString value = m_settings->value("BadSectorsData/BadSectors").toString();
        if(value.isEmpty()) {
            value = cylinderNumber;
        } else {
            value = value + "," + cylinderNumber;
        }

        m_settings->setValue("BadSectorsData/BadSectors", value);
    }

    m_cylinderInfoWidget->setCurCheckBadBlocksInfo(LBANumber, cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
}

void DiskBadSectorsDialog::onCheckCoomplete()
{
    m_progressBar->setValue(100);
    m_unusedTimeLabel->setText(tr("Time left:") + "00:00:00");
    m_buttonStackedWidget->setCurrentIndex(3);
    m_checkInfoLabel->setText(tr("Verify completed")); // 检测完成
    m_curType = StatusType::Normal;
    m_resetButton->setDisabled(false);

    int badSectorsCount = 0;
    QString value = m_settings->value("BadSectorsData/BadSectors").toString();
    if (!value.isEmpty()) {
        badSectorsCount = value.split(",").count();
    }

    if (badSectorsCount > 0) {
        m_totalRepairNumber = 0;
        m_curRepairNumber = 0;
        m_curRepairTime = 0;
        m_repairButton->setDisabled(false);
    }

    MessageBox messageBox;
    messageBox.setObjectName("messageBox");
    // 磁盘检测完毕，共检查到xxx个坏道区域   确定
    messageBox.setWarings(tr("Disk verify completed. %1 bad blocks found.").arg(badSectorsCount), "", tr("OK"));
    messageBox.exec();
}

void DiskBadSectorsDialog::onStopButtonClicked()
{
    switch (m_curType) {
    case StatusType::Check: {
        m_buttonStackedWidget->setCurrentIndex(2);
        m_resetButton->setDisabled(false);
        m_curType = StatusType::StopCheck;
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

        int checkSize = m_settings->value("SettingData/CheckSize").toInt();
        int blockStart = m_settings->value("SettingData/BlockStart").toInt();
        int blockEnd = m_settings->value("SettingData/BlockEnd").toInt();
        int checkNumber = m_settings->value("SettingData/CheckNumber").toInt();

        DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart, blockEnd, checkNumber, checkSize, 2);
        break;
    }
    case StatusType::Repair:{
        m_buttonStackedWidget->setCurrentIndex(2);
        m_curType = StatusType::StopRepair;
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        int repairSize = static_cast<int>(info.heads * info.sectors * info.sector_size);

        QString repairCylinder = m_settings->value("BadSectorsData/BadSectors").toString();
        QStringList lstBadSectors = repairCylinder.split(",");

        DMDbusHandler::instance()->repairBadBlocks(info.m_path, lstBadSectors, repairSize, 2);
        m_timer.stop();
        break;
    }
    default:
        break;
    }
}

void DiskBadSectorsDialog::onContinueButtonClicked()
{
    switch (m_curType) {
    case StatusType::StopCheck: {
        m_buttonStackedWidget->setCurrentIndex(1);
        m_resetButton->setDisabled(true);
        m_curType = StatusType::Check;
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

        int checkSize = m_settings->value("SettingData/CheckSize").toInt();
        int blockStart = m_settings->value("SettingData/CurCylinder").toInt();
        int blockEnd = m_settings->value("SettingData/BlockEnd").toInt();
        int checkNumber = m_settings->value("SettingData/CheckNumber").toInt();

        DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart + 1, blockEnd, checkNumber, checkSize, 3);
        break;
    }
    case StatusType::StopRepair:{
        m_buttonStackedWidget->setCurrentIndex(1);
        m_curType = StatusType::Repair;
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        int repairSize = static_cast<int>(info.heads * info.sectors * info.sector_size);

        QString repairCylinder = m_settings->value("BadSectorsData/BadSectors").toString();
        QStringList lstBadSectors = repairCylinder.split(",");

        DMDbusHandler::instance()->repairBadBlocks(info.m_path, lstBadSectors, repairSize, 3);
        m_timer.start(200);
        break;
    }
    default:
        break;
    }
}

void DiskBadSectorsDialog::onAgainVerifyButtonClicked()
{
    m_progressBar->setValue(0);
    m_usedTimeLabel->setText(tr("Time elapsed:") + "00:00:00");
    m_unusedTimeLabel->setText(tr("Time left:") + "00:00:00");
    m_checkInfoLabel->setText("");
    m_curType = StatusType::Check;
    m_buttonStackedWidget->setCurrentIndex(1);
    m_resetButton->setDisabled(true);
    m_repairButton->setDisabled(true);
    m_curCheckNumber = 0;
    m_curCheckTime = 0;

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

    int checkSize = m_settings->value("SettingData/CheckSize").toInt();
    int blockStart = m_settings->value("SettingData/BlockStart").toInt();
    int blockEnd = m_settings->value("SettingData/BlockEnd").toInt();
    int checkNumber = m_settings->value("SettingData/CheckNumber").toInt();

    QFile file("/tmp/CheckData.conf");
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QTextStream out(&file);
        out << "[SettingData]\n";
        out << "[CheckData]\n";
        out << "[BadSectorsData]\n";
        out << "BadSectors=""\n";
        file.close();
    }

//    m_totalCheckNumber = blockEnd - blockStart + 1;
//    m_cylinderInfoWidget->setCylinderNumber(m_totalCheckNumber);
    m_cylinderInfoWidget->againVerify(blockEnd - blockStart + 1);

    m_settings->beginGroup("SettingData");
    m_settings->setValue("BlockStart", blockStart);
    m_settings->setValue("BlockEnd", blockEnd);
    m_settings->setValue("CheckSize", checkSize);
    m_settings->setValue("CheckNumber", checkNumber);
    m_settings->endGroup();

    DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart, blockEnd, checkNumber, checkSize, 1);
}

void DiskBadSectorsDialog::onResetButtonClicked()
{
    m_progressBar->setValue(0);
    m_usedTimeLabel->setText(tr("Time elapsed:") + "00:00:00");
    m_unusedTimeLabel->setText(tr("Time left:") + "00:00:00");
    m_checkInfoLabel->setText("");
    m_curType = StatusType::Normal;
    m_buttonStackedWidget->setCurrentIndex(0);
    m_progressBar->hide();
    m_usedTimeLabel->hide();
    m_unusedTimeLabel->hide();
    m_repairButton->setDisabled(true);
    m_resetButton->setDisabled(true);
    m_totalCheckNumber = 0;
    m_curCheckNumber = 0;
    m_curCheckTime = 0;
    m_totalRepairNumber = 0;
    m_curRepairNumber = 0;
    m_curRepairTime = 0;

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    m_verifyComboBox->setCurrentIndex(0);
    m_startLineEdit->setText("0");
    m_startLineEdit->lineEdit()->setPlaceholderText("0");
    m_endLineEdit->setText(QString("%1").arg(info.cylinders));
    m_endLineEdit->lineEdit()->setPlaceholderText(QString("%1").arg(info.cylinders));

    m_methodComboBox->setCurrentIndex(0);
    m_checkTimesEdit->setText("8");
    m_timeoutEdit->setText("3000");

    int blockStart = m_startLineEdit->text().toInt();
    int blockEnd = m_endLineEdit->text().toInt();
    m_cylinderInfoWidget->reset(blockEnd - blockStart + 1);
}

bool DiskBadSectorsDialog::isExistMountPartition()
{
    bool isExist = false;

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    for (int i = 0; i < info.partition.size(); i++) {
        PartitionInfo partitionInfo = info.partition.at(i);

        QString mountpoints;
        for (int j = 0; j < partitionInfo.m_mountPoints.size(); j++) {
            mountpoints += partitionInfo.m_mountPoints[j];
        }

        if (!mountpoints.isEmpty()) {
            isExist = true;
            break;
        }
    }

    return isExist;
}

void DiskBadSectorsDialog::onRepairButtonClicked()
{
    if (isExistMountPartition()) {
        MessageBox warningBox;
        QString title1 = tr("The verifying disk contains mounted partitions, so you cannot repair it."); // 当前检测磁盘存在已挂载分区，无法修复坏道，
//        title1.replace("so", "so\n");
        QString title2 = tr("Please unmount partitions and then repair the disk."); // 若要修复请先卸载分区

        warningBox.setWarings(title1 + "\n" + title2, "", tr("OK"));
        warningBox.exec();

        return;
    }

    DDialog messageBox;
    messageBox.setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));
    messageBox.setTitle(tr("Warning")); // 警告
    messageBox.addSpacing(10);

    DLabel *label1 = new DLabel(tr("Bad sector repairing cannot recover files,")); // 修复坏磁道不是数据恢复的手段，
    DLabel *label2 = new DLabel(tr("but destroys data on and near bad sectors instead.")); // 修复坏磁道会破坏坏磁道及其附近磁道上的文件数据。
    DLabel *label3 = new DLabel(tr("Please back up all data before repair.")); // 请先做好数据备份。

    QVBoxLayout *Layout = new QVBoxLayout;
    Layout->addWidget(label1, 0, Qt::AlignCenter);
    Layout->addWidget(label2, 0, Qt::AlignCenter);
    Layout->addWidget(label3, 0, Qt::AlignCenter);
    Layout->setContentsMargins(0, 0, 0, 0);

    QWidget *widget = new QWidget;
    widget->setLayout(Layout);
    messageBox.addContent(widget, Qt::AlignCenter);
    messageBox.addSpacing(10);

    messageBox.addButton(tr("Cancel")); // 取消
    messageBox.addButton(tr("Start Repair")); // 开始修复
    if (messageBox.exec() == DDialog::Accepted) {
        m_curType = StatusType::Repair;
        m_repairButton->setDisabled(true);
        m_resetButton->setDisabled(true);
        m_buttonStackedWidget->setCurrentIndex(1);
        m_progressBar->setValue(0);
        m_usedTimeLabel->setText(tr("Time elapsed:") + "00:00:00");
        m_unusedTimeLabel->setText(tr("Time left:") + "00:00:00");
        m_checkInfoLabel->setText("");
        m_repairedCount = 0;
        m_curRepairNumber = 0;
        m_curRepairTime = 0;

        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        int repairSize = static_cast<int>(info.heads * info.sectors * info.sector_size);

        QString repairCylinder = m_settings->value("BadSectorsData/BadSectors").toString();
        QStringList lstBadSectors = repairCylinder.split(",");

        m_totalRepairNumber = lstBadSectors.count();
        m_usedTime = 0;
        m_unusedTime = m_totalRepairNumber * 2500;

        qint64 remainingHour = 0;
        qint64 remainingMinute = 0;
        qint64 remainingSecond = 0;
        mSecsToTime(m_unusedTime, remainingHour, remainingMinute, remainingSecond);
        m_unusedTimeLabel->setText(tr("Time left:") + QString("%1:%2:%3").arg(remainingHour, 2, 10, QLatin1Char('0')).arg(remainingMinute, 2, 10, QLatin1Char('0')).arg(remainingSecond, 2, 10, QLatin1Char('0')));

        DMDbusHandler::instance()->repairBadBlocks(info.m_path, lstBadSectors, repairSize, 1);
        m_timer.start(200);
    }
}

void DiskBadSectorsDialog::onRepairBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming)
{
    ++m_curRepairNumber;
    m_curRepairTime += cylinderTimeConsuming.toLongLong();
    m_checkInfoLabel->setText(tr("Repairing cylinder: %1").arg(cylinderNumber)); // 正在修复xxx柱面

    qint64 totalTime = m_curRepairTime / m_curRepairNumber * m_totalRepairNumber;
    m_usedTime = m_curRepairTime;
    m_unusedTime = totalTime - m_curRepairTime;
    m_unusedTime < 1000 ? m_unusedTime = 1000 : m_unusedTime;

    QString badSectors = m_settings->value("BadSectorsData/BadSectors").toString();
    QStringList lstBadSectors = badSectors.split(",");
    lstBadSectors.removeOne(cylinderNumber);
    if (lstBadSectors.count() > 0) {
        m_settings->setValue("BadSectorsData/BadSectors", lstBadSectors.join(","));
    } else {
        m_settings->setValue("BadSectorsData/BadSectors", "");
    }

    if (cylinderStatus == "good") {
        ++m_repairedCount;
        QString curCheckData = m_settings->value(QString("CheckData/%1").arg(cylinderNumber)).toString();
        QStringList lstCheckData = curCheckData.split(",");
        lstCheckData.replace(3, cylinderStatus);
        lstCheckData.replace(5, "1");
        m_settings->setValue(QString("CheckData/%1").arg(cylinderNumber), lstCheckData.join(","));

        m_cylinderInfoWidget->setCurRepairBadBlocksInfo(cylinderNumber);
    }
}

void DiskBadSectorsDialog::onRepairCoomplete()
{
    m_timer.stop();
    m_progressBar->setValue(100);
    m_unusedTimeLabel->setText(tr("Time left:") + "00:00:00");
    m_buttonStackedWidget->setCurrentIndex(4);
    m_curType = StatusType::Normal;
    m_resetButton->setDisabled(false);

    m_checkInfoLabel->setText(tr("Repair completed. Cylinder: %1 repaired.").arg(m_repairedCount)); // 修复完成，已修复XXXXXX柱面

    // 磁盘修复完毕，共修复xxx个坏道区域
    DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("://icons/deepin/builtin/ok.svg"), tr("Disk repair completed. %1 bad blocks repaired.").arg(m_repairedCount));
    DMessageManager::instance()->setContentMargens(this, QMargins(0, 0, 0, 20));
}

void DiskBadSectorsDialog::onTimeOut()
{
    m_usedTime += 200;
    m_unusedTime -= 200;

    int value = QString::number((float)m_usedTime / (m_usedTime + m_unusedTime),'f', 2).toFloat() * 100;
    value > 99 ? value = 99 : value;
    m_progressBar->setValue(value);

    m_unusedTime < 1000 ? m_unusedTime = 1000 : m_unusedTime;

    qint64 usedHour = 0;
    qint64 usedMinute = 0;
    qint64 usedSecond = 0;
    mSecsToTime(m_usedTime, usedHour, usedMinute, usedSecond);
    m_usedTimeLabel->setText(tr("Time elapsed:") + QString("%1:%2:%3").arg(usedHour, 2, 10, QLatin1Char('0')).arg(usedMinute, 2, 10, QLatin1Char('0')).arg(usedSecond, 2, 10, QLatin1Char('0')));

    qint64 remainingHour = 0;
    qint64 remainingMinute = 0;
    qint64 remainingSecond = 0;
    mSecsToTime(m_unusedTime, remainingHour, remainingMinute, remainingSecond);
    m_unusedTimeLabel->setText(tr("Time left:") + QString("%1:%2:%3").arg(remainingHour, 2, 10, QLatin1Char('0')).arg(remainingMinute, 2, 10, QLatin1Char('0')).arg(remainingSecond, 2, 10, QLatin1Char('0')));
}

void DiskBadSectorsDialog::onExitButtonClicked()
{
    close();
}

void DiskBadSectorsDialog::onDoneButtonClicked()
{
    m_curType = StatusType::Normal;
    close();
}

void DiskBadSectorsDialog::closeEvent(QCloseEvent *event)
{
    switch (m_curType) {
    case StatusType::Check: {
        MessageBox messageBox;
        messageBox.setObjectName("messageBox");
        // 正在检测中，是否退出窗口？  当前检测信息不会保留   退出   取消
        messageBox.setWarings(tr("Verifying for bad sectors, exit now?"), tr("The verified information will not be reserved"),
                              tr("Exit"), tr("Cancel"));
        if (messageBox.exec() == DDialog::Accepted) {
            DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();

            int checkSize = m_settings->value("SettingData/CheckSize").toInt();
            int blockStart = m_settings->value("SettingData/BlockStart").toInt();
            int blockEnd = m_settings->value("SettingData/BlockEnd").toInt();
            int checkNumber = m_settings->value("SettingData/CheckNumber").toInt();

            DMDbusHandler::instance()->checkBadSectors(info.m_path, blockStart, blockEnd, checkNumber, checkSize, 2);

            QFile file("/tmp/CheckData.conf");
            if (file.exists()) {
                file.remove();
            }
        } else {
            event->ignore();
        }
        break;
    }
    case StatusType::Repair:{
        MessageBox messageBox;
        messageBox.setObjectName("messageBox");
        // 正在修复中，是否退出窗口？  当前修复信息不会保留   退出   取消
        messageBox.setWarings(tr("Repairing bad sectors, exit now?"), tr("The repairing information will not be reserved"),
                              tr("Exit"), tr("Cancel"));
        if (messageBox.exec() == DDialog::Accepted) {

            DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
            int repairSize = static_cast<int>(info.heads * info.sectors * info.sector_size);

            QString repairCylinder = m_settings->value("BadSectorsData/BadSectors").toString();
            QStringList lstBadSectors = repairCylinder.split(",");

            DMDbusHandler::instance()->repairBadBlocks(info.m_path, lstBadSectors, repairSize, 2);
            m_timer.stop();

            QFile file("/tmp/CheckData.conf");
            if (file.exists()) {
                file.remove();
            }
        } else {
            event->ignore();
        }
        break;
    }
    default:
        QFile file("/tmp/CheckData.conf");
        if (file.exists()) {
            file.remove();
        }
        break;
    }
}



