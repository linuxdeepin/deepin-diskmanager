// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "decryptdialog.h"
#include "common.h"

#include <DWindowCloseButton>
#include <DMessageManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>

DecryptDialog::DecryptDialog(QWidget *parent)
    : DDBase(parent)
{
    qDebug()  << "[DecryptDialog] Constructor initialized";
    initUi();
    initConnection();
    initData();
}

void DecryptDialog::initUi()
{
    qDebug() << "[DecryptDialog] Initializing UI components";
    m_height = 240;
    setFixedSize(406, m_height);

    DPalette palette1;
    QColor color1("#000000");
    color1.setAlphaF(0.9);
    palette1.setColor(DPalette::ToolTipText, color1);

    QFont font1;
    font1.setWeight(QFont::Medium);
    font1.setFamily("Source Han Sans");
    font1.setPixelSize(14);

    m_titleText = tr("Enter the password to decrypt the disk");
    if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::PARTITION) {
        qDebug() << "[DecryptDialog] Current level is PARTITION.";
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        m_devPath = info.m_path;
        m_devName = info.m_path.remove("/dev/");
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::DISK) {
        qDebug() << "[DecryptDialog] Current level is DISK.";
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        m_devPath = info.m_path;
        m_devName = info.m_path.remove("/dev/");
    } else if (DMDbusHandler::instance()->getCurLevel() == DMDbusHandler::LOGICALVOLUME) {
        qDebug() << "[DecryptDialog] Current level is LOGICALVOLUME.";
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        m_devPath = lvInfo.m_lvPath;
        m_titleText = tr("Enter the password to decrypt the volume group");
        m_devName = QString("%1_%2").arg(lvInfo.m_vgName).arg(lvInfo.m_lvName);
    } else {
        qDebug() << "[DecryptDialog] Current level is unknown or not handled.";
    }

    m_luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(m_devPath);
    qDebug() << "[DecryptDialog] Retrieved LUKS info for device:" << m_devPath;

    setTitle(m_titleText);

    // 限制中文和中文字符
#if QT_VERSION_MAJOR > 5
    qDebug() << "[DecryptDialog] Using QRegularExpression for validator (QT_VERSION_MAJOR > 5).";
    QRegularExpression regExp("^[A-Za-z0-9`~!@#$%^&*()_-+=<>,.\\/ ]+$");
    QRegularExpressionValidator *regExpValidator = new QRegularExpressionValidator(regExp, this);
#else
    qDebug() << "[DecryptDialog] Using QRegExp for validator (QT_VERSION_MAJOR <= 5).";
    QRegExp regExp("^[A-Za-z0-9`~!@#$%^&*()_-+=<>,.\\/ ]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp, this);
#endif

    m_passwordEdit = new DPasswordEdit(this);
    m_passwordEdit->setAccessibleName("passwordInput");
    m_passwordEdit->lineEdit()->setValidator(regExpValidator);
    m_passwordEdit->lineEdit()->setPlaceholderText(tr("Enter a password "));
    m_passwordEdit->setFixedHeight(37);
    qDebug() << "[DecryptDialog] Password edit created.";

    m_pushButton = new DPushButton(this);
    m_pushButton->setAccessibleName("passwordHintButton");
    m_pushButton->setToolTip(tr("Password hint"));
    m_pushButton->setFixedSize(QSize(36, 36));
    m_pushButton->setIcon(Common::getIcon("passwordhint"));
    m_pushButton->setIconSize(QSize(18, 18));
    qDebug() << "[DecryptDialog] Password hint button created.";

    m_passwordHint = new DLabel(this);
    m_passwordHint->setAccessibleName("passwordHint");
    m_passwordHint->setFont(font1);
    m_passwordHint->setPalette(palette1);
    m_passwordHint->adjustSize();
    m_passwordHint->setWordWrap(true);
    qDebug() << "[DecryptDialog] Password hint label created.";

    QHBoxLayout *hintButtonLayout = new QHBoxLayout;
    hintButtonLayout->addWidget(m_pushButton);
    hintButtonLayout->setSpacing(0);
    hintButtonLayout->setContentsMargins(10, 0, 0, 0);
    QWidget *hintButtonWdiget = new QWidget(this);
    hintButtonWdiget->setLayout(hintButtonLayout);

    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->addWidget(m_passwordEdit);
    passwordLayout->addWidget(hintButtonWdiget);
    passwordLayout->setSpacing(0);
    passwordLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *passwordHintLayout = new QHBoxLayout;
    passwordHintLayout->addWidget(m_passwordHint);
    passwordHintLayout->setContentsMargins(10, 5, 10, 5);

    m_frame = new DFrame(this);
    m_frame->setBackgroundRole(DPalette::ItemBackground);
    m_frame->setLineWidth(0);
    m_frame->setLayout(passwordHintLayout);
    m_frame->hide();

    if (m_luksInfo.m_tokenList.isEmpty()) {
        qDebug() << "[DecryptDialog] LUKS token list is empty, hiding hint button widget.";
        hintButtonWdiget->hide();
    }

    QVBoxLayout *contentLayout = new QVBoxLayout;
    //contentLayout->addWidget(titleLabel, 0, Qt::AlignCenter);
    contentLayout->addSpacing(20);
    contentLayout->addLayout(passwordLayout);
    contentLayout->addSpacing(10);
    contentLayout->addWidget(m_frame);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(10, 0, 10, 0);

    m_cancelButton = new DPushButton(tr("Cancel", "button"), this);
    m_cancelButton->setAccessibleName("cancel");
    m_cancelButton->setFont(font1);
    m_cancelButton->setFixedSize(183, 36);

    m_decryptButton = new DSuggestButton(tr("Decrypt", "button"), this);
    m_decryptButton->setAccessibleName("decrypt");
    m_decryptButton->setFont(font1);
    m_decryptButton->setFixedSize(183, 36);

    DVerticalLine *line = new DVerticalLine;
    line->setObjectName("VLine");
    line->setFixedHeight(28);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addSpacing(9);
    buttonLayout->addWidget(line, 0, Qt::AlignCenter);
    buttonLayout->addSpacing(9);
    buttonLayout->addWidget(m_decryptButton);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *firstLayout = new QVBoxLayout;
    firstLayout->addLayout(contentLayout);
    firstLayout->addStretch();
    firstLayout->addLayout(buttonLayout);
    firstLayout->setSpacing(0);
    firstLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *firstWidget = new QWidget(this);
    firstWidget->setLayout(firstLayout);

    m_waterLoadingWidget = new WaterLoadingWidget(this);
    m_waterLoadingWidget->setFixedSize(128, 128);
    qDebug() << "[DecryptDialog] WaterLoadingWidget created.";

    QVBoxLayout *secondLayout = new QVBoxLayout;
    secondLayout->addSpacing(4);
    secondLayout->addWidget(m_waterLoadingWidget, 0, Qt::AlignCenter);
    secondLayout->addStretch();
    secondLayout->setSpacing(0);
    secondLayout->setContentsMargins(0, 0, 0, 10);

    QWidget *secondWidget = new QWidget(this);
    secondWidget->setLayout(secondLayout);

    m_stackedWidget = new DStackedWidget(this);
    m_stackedWidget->addWidget(firstWidget);
    m_stackedWidget->addWidget(secondWidget);
    m_stackedWidget->setCurrentIndex(0);
    qDebug() << "[DecryptDialog] Stacked widget initialized.";

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->addWidget(m_stackedWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void DecryptDialog::initConnection()
{
    qDebug() << "[DecryptDialog] Setting up signal-slot connections";
    connect(m_passwordEdit, &DPasswordEdit::textChanged, this, &DecryptDialog::onPasswordEditTextChanged);
    connect(m_pushButton, &DPushButton::clicked, this, &DecryptDialog::onPasswordHintButtonClicked);
    connect(m_decryptButton, &DSuggestButton::clicked, this, &DecryptDialog::onButtonClicked);
    connect(m_cancelButton, &DPushButton::clicked, this, &DecryptDialog::onCancelButtonClicked);
    connect(&m_timer, &QTimer::timeout, this, &DecryptDialog::onTimeOut);
    connect(DMDbusHandler::instance(), &DMDbusHandler::deCryptMessage, this, &DecryptDialog::onDecryptMessage);
}

void DecryptDialog::initData()
{
    qDebug() << "[DecryptDialog] Initializing data.";
    if (m_luksInfo.m_decryptErrCount > 0) {
        qDebug() << "[DecryptDialog] Decrypt error count is greater than 0:" << m_luksInfo.m_decryptErrCount;
        if (m_luksInfo.m_decryptErrCount >= 5) {
            qDebug() << "[DecryptDialog] Decrypt error count is 5 or more.";
            QString text = tr("Please try again %1 minutes later");
            QDateTime lastErrorTime = QDateTime::fromString(m_luksInfo.m_decryptErrorLastTime, "yyyy-MM-dd hh:mm:ss");
            QDateTime curDateTime = QDateTime::fromString(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "yyyy-MM-dd hh:mm:ss");
            qint64 sec = lastErrorTime.secsTo(curDateTime);
            int minutes = static_cast<int>(sec / 60);

            if (m_luksInfo.m_decryptErrCount == 5) {
                qDebug() << "[DecryptDialog] Decrypt error count is 5, updating data with 3 minutes delay.";
                updateData(3, minutes, text);
            } else if (m_luksInfo.m_decryptErrCount == 6) {
                qDebug() << "[DecryptDialog] Decrypt error count is 6, updating data with 5 minutes delay.";
                updateData(5, minutes, text);
            } else if (m_luksInfo.m_decryptErrCount == 7) {
                qDebug() << "[DecryptDialog] Decrypt error count is 7, updating data with 15 minutes delay.";
                updateData(15, minutes, text);
            } else if (m_luksInfo.m_decryptErrCount >= 8) {
                qDebug() << "[DecryptDialog] Decrypt error count is 8 or more, updating data with 60 minutes delay.";
                updateData(60, minutes, text);
            }
        }
    }

    m_passwordEdit->setFocus();
    qDebug() << "[DecryptDialog] Setting focus to password edit.";
}

void DecryptDialog::setPasswordEditStatus(const bool &isDisabled, const bool &isAlert, const QString &message)
{
    qDebug() << "[DecryptDialog] Setting password edit status: isDisabled=" << isDisabled << ", isAlert=" << isAlert << ", message=" << message;
    m_passwordEdit->setDisabled(isDisabled);
    m_passwordEdit->setAlert(isAlert);

    if (isAlert) {
        qDebug() << "[DecryptDialog] Password edit is in alert state, clearing and showing alert message.";
        m_passwordEdit->clear();
        m_passwordEdit->showAlertMessage(message);
    } else {
        qDebug() << "[DecryptDialog] Password edit is not in alert state, hiding alert message.";
        m_passwordEdit->hideAlertMessage();
    }
}

void DecryptDialog::updateData(const int &baseMinute, const int &minutes, const QString &message)
{
    qDebug() << "[DecryptDialog] Updating data: baseMinute=" << baseMinute << ", minutes=" << minutes << ", message=" << message;
    if (minutes < baseMinute) {
        qDebug() << "[DecryptDialog] Minutes passed is less than base minute, disabling password edit and decrypt button.";
        setPasswordEditStatus(true, true, message.arg(baseMinute - minutes));
        m_timer.start((baseMinute - minutes) * 60 * 1000);
        m_decryptButton->setDisabled(true);
    } else {
        qDebug() << "[DecryptDialog] Minutes passed is greater or equal to base minute, enabling password edit and decrypt button.";
        setPasswordEditStatus(false, false);
        m_decryptButton->setDisabled(false);
    }
}

void DecryptDialog::onPasswordEditTextChanged(const QString &)
{
    qDebug() << "[DecryptDialog] Password edit text changed.";
    if (m_passwordEdit->isAlert()) {
        qDebug() << "[DecryptDialog] Password edit was in alert state, resetting.";
        m_passwordEdit->setAlert(false);
        m_passwordEdit->hideAlertMessage();
    }
}

void DecryptDialog::onPasswordHintButtonClicked()
{
    qDebug() << "[DecryptDialog] Password hint button clicked.";
    if (!m_frame->isHidden()) {
        qDebug() << "[DecryptDialog] Hint frame is visible, hiding it.";
        m_height = 200;
        setFixedSize(406, m_height);
        m_frame->hide();
        return;
    }

    qDebug() << "[DecryptDialog] Hint frame is hidden, showing it.";
    m_passwordHint->setText(m_luksInfo.m_tokenList.join("\n"));
    int heightLabel = Common::getLabelAdjustHeight(width() - 60, m_passwordHint->text(), m_passwordHint->font());
    m_passwordHint->setFixedHeight(heightLabel);
    m_frame->show();
    m_height = 215 + heightLabel;
    setFixedSize(406, m_height);
    qDebug() << "[DecryptDialog] Updated height after showing hint:" << m_height;
}

void DecryptDialog::onCancelButtonClicked()
{
    qDebug() << "[DecryptDialog] Cancel button clicked, rejecting dialog.";
    reject();
}

void DecryptDialog::onButtonClicked()
{
    qDebug()  << "[DecryptDialog] Starting decryption process for device:" << m_devPath;
    // 密码不能为空
    if (m_passwordEdit->text().isEmpty()) {
        qWarning() << "[DecryptDialog] Password cannot be empty, showing alert.";
        m_passwordEdit->setAlert(true);
        m_passwordEdit->showAlertMessage(tr("The password cannot be empty"));
        m_passwordEdit->setFocus();
        return;
    }

    LUKS_INFO luksInfo = m_luksInfo;
    luksInfo.m_decryptStr = m_passwordEdit->text();
    luksInfo.m_devicePath = m_devPath;
    if (m_luksInfo.m_crypt == CRYPT_CIPHER::AES_XTS_PLAIN64) {
        qDebug() << "[DecryptDialog] Crypt cipher is AES_XTS_PLAIN64.";
        luksInfo.m_mapper.m_dmName = QString("%1_aesE").arg(m_devName);
    } else if (m_luksInfo.m_crypt == CRYPT_CIPHER::SM4_XTS_PLAIN64) {
        qDebug() << "[DecryptDialog] Crypt cipher is SM4_XTS_PLAIN64.";
        luksInfo.m_mapper.m_dmName = QString("%1_sm4E").arg(m_devName);
    } else {
        qDebug() << "[DecryptDialog] Crypt cipher is unknown or not handled, setting empty dmName.";
        luksInfo.m_mapper.m_dmName = "";
    }

    qDebug() << "[DecryptDialog] Calling DMDbusHandler::deCrypt with LUKS info.";
    DMDbusHandler::instance()->deCrypt(luksInfo);
    qDebug() << "[DecryptDialog] DMDbusHandler::deCrypt call completed.";

    setFixedSize(406, 226);
    m_stackedWidget->setCurrentIndex(1);
    setTitle(tr("Decrypting..."));
    DWindowCloseButton *button = findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    if (button != nullptr) {
        qDebug() << "[DecryptDialog] Found close button, disabling and hiding it.";
        button->setDisabled(true);
        button->hide();
    }
    m_waterLoadingWidget->setStartTime(1000);
    qDebug() << "[DecryptDialog] Water loading widget started.";
}

void DecryptDialog::onDecryptMessage(const LUKS_INFO &luks)
{
    qDebug() << "[DecryptDialog] Received decryption message for device:" << luks.m_devicePath << ", isDecrypt:" << luks.isDecrypt << ", Error count:" << luks.m_decryptErrCount;
    DMDbusHandler::instance()->updateLUKSInfo(m_devPath, luks);
    qDebug() << "[DecryptDialog] Updated LUKS info.";
    m_luksInfo = luks;

    if (luks.isDecrypt && luks.m_decryptErrCount == 0) {
        qDebug() << "[DecryptDialog] Decryption successful and error count is 0. Refreshing main window data.";
        DMDbusHandler::instance()->refreshMainWindowData();
        if (luks.m_cryptErr == CRYPTError::CRYPT_ERR_DECRYPT_FAILED) {
            qWarning() << "[DecryptDialog] Crypt error: DECRYPT_FAILED. Showing decryption failed message.";
            DMessageManager::instance()->sendMessage(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(),
                                                     QIcon::fromTheme("://icons/deepin/builtin/warning.svg"), tr("Decryption failed"));
            DMessageManager::instance()->setContentMargens(this->parentWidget()->parentWidget()->parentWidget()->parentWidget(),
                                                           QMargins(0, 0, 0, 20));
            reject();
        } else {
            qDebug()  << "[DecryptDialog] Decrypt successful for device:" << m_devPath << ", accepting dialog.";
            accept();
        }
    } else {
        qWarning() << "[DecryptDialog] Decrypt failed for device:" << m_devPath
                  << "Error count:" << luks.m_decryptErrCount;
        setFixedSize(406, m_height);
        m_stackedWidget->setCurrentIndex(0);
        DWindowCloseButton *button = findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
        if (button != nullptr) {
            qDebug() << "[DecryptDialog] Decrypt failed, found close button, enabling and showing it.";
            button->setDisabled(false);
            button->show();
        }

        QString text = tr("Password locked, please try again %1 minutes later");
        if (luks.m_decryptErrCount < 5 && luks.m_decryptErrCount > 0) {
            qDebug() << "[DecryptDialog] Decrypt error count less than 5, showing remaining chances.";
            text = tr("Wrong password, %1 chances left").arg(5 - luks.m_decryptErrCount);
            if ((5 - luks.m_decryptErrCount) == 1) {
                qDebug() << "[DecryptDialog] Only one chance left.";
                text = tr("Wrong password, only one chance left");
            }

            setPasswordEditStatus(false, true, text);
        } else if (luks.m_decryptErrCount == 5) {
            qDebug() << "[DecryptDialog] Decrypt error count is 5, locking password for 3 minutes.";
            setPasswordEditStatus(true, true, text.arg(3));
            m_decryptButton->setDisabled(true);
            m_timer.start(3 * 60 * 1000);
        } else if (luks.m_decryptErrCount == 6) {
            qDebug() << "[DecryptDialog] Decrypt error count is 6, locking password for 5 minutes.";
            setPasswordEditStatus(true, true, text.arg(5));
            m_decryptButton->setDisabled(true);
            m_timer.start(5 * 60 * 1000);
        } else if (luks.m_decryptErrCount == 7) {
            qDebug() << "[DecryptDialog] Decrypt error count is 7, locking password for 15 minutes.";
            setPasswordEditStatus(true, true, text.arg(15));
            m_decryptButton->setDisabled(true);
            m_timer.start(15 * 60 * 1000);
        } else if (luks.m_decryptErrCount >= 8) {
            qDebug() << "[DecryptDialog] Decrypt error count is 8 or more, locking password for 60 minutes.";
            setPasswordEditStatus(true, true, text.arg(60));
            m_decryptButton->setDisabled(true);
            m_timer.start(60 * 60 * 1000);
        }

        m_passwordEdit->setFocus();
        qDebug() << "[DecryptDialog] Setting focus to password edit after decrypt message handling.";
    }
}

void DecryptDialog::onTimeOut()
{
    qDebug() << "[DecryptDialog] Timer expired, resetting password input";
    m_timer.stop();
    setPasswordEditStatus(false, false);
    m_decryptButton->setDisabled(false);
    m_passwordEdit->setFocus();
    qDebug() << "[DecryptDialog] Password input reset and decrypt button enabled.";
}

void DecryptDialog::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << "[DecryptDialog] Key press event received:" << event->key();
    if (event->key() == Qt::Key::Key_Enter || event->key() == Qt::Key::Key_Return) {
        // qDebug() << "[DecryptDialog] Enter or Return key pressed, calling onButtonClicked().";
        onButtonClicked();
    } else if (event->key() == Qt::Key::Key_Escape) {
        // qDebug() << "[DecryptDialog] Escape key pressed, ignoring event.";
        event->ignore();
    } else {
        // qDebug() << "[DecryptDialog] Other key pressed, calling base class keyPressEvent.";
        DDialog::keyPressEvent(event);
    }
}
