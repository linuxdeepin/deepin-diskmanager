/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file formatedialog.cpp
 *
 * @brief 格式化窗口类
 *
 * @date 2020-09-10 17:22
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
#include "formatedialog.h"
#include "partedproxy/dmdbushandler.h"

#include <DLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QHBoxLayout>

FormateDialog::FormateDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void FormateDialog::initUi()
{
    this->setFixedSize(450, 300);
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    setTitle(tr("Format %1").arg(info.m_path));
    DLabel *tipLabel = new DLabel(tr("Formatting will erase all data on the disk, which cannot be undone"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setFixedHeight(50);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);


    DLabel *fileName = new DLabel(tr("Name:"), this);
//    fileName->setMinimumWidth(76);
    m_fileNameEdit   = new DLineEdit(this);
    m_fileNameEdit->setAccessibleName("partName");
    QRegExp re("^[\u4E00-\u9FA5A-Za-z0-9_]+$");
    QRegExpValidator *validator = new QRegExpValidator(re, this);
    m_fileNameEdit->lineEdit()->setValidator(validator );
    m_fileNameEdit->setFixedHeight(36);
    if (m_fileNameEdit->text().isEmpty())
        m_fileNameEdit->lineEdit()->setPlaceholderText(tr("Name"));

    DLabel *formatName = new DLabel(tr("File system:"), this);
    m_formatComboBox = new QComboBox(this);
    m_formatComboBox->setAccessibleName("File system");
    m_formatComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QStringList fslist = DMDbusHandler::instance()->getAllSupportFileSystem();
    fslist.removeOne("linux-swap");
    m_formatComboBox->addItems(fslist);
    m_formatComboBox->setFixedHeight(36);

    if (-1 == fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType)))) {
        m_formatComboBox->setCurrentIndex(fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(11))));
    } else {
        m_formatComboBox->setCurrentIndex(fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType))));
    }

    QVBoxLayout *layoutName = new QVBoxLayout;
    layoutName->addWidget(fileName);
    layoutName->addSpacing(10);
    layoutName->addWidget(formatName);
    layoutName->setSpacing(0);
    layoutName->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layoutFormat = new QVBoxLayout;
    layoutFormat->addWidget(m_fileNameEdit);
    layoutFormat->addSpacing(10);
    layoutFormat->addWidget(m_formatComboBox);
    layoutFormat->setSpacing(0);
    layoutFormat->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(layoutName);
    layout->addLayout(layoutFormat);
    layout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(tipLabel);
//    mainLayout->addSpacing(60);
    mainLayout->addLayout(layout);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    addButton(tr("Cancel"), true, ButtonNormal);
    m_okCode = addButton(tr("Format"), false, ButtonWarning);
}

void FormateDialog::initConnection()
{
    connect(this, &FormateDialog::buttonClicked, this, &FormateDialog::onButtonClicked);
    connect(m_fileNameEdit, &DLineEdit::textChanged, this, &FormateDialog::onTextChanged);
    connect(m_formatComboBox, &DComboBox::currentTextChanged, this, &FormateDialog::onComboxFormatTextChange);
}

void FormateDialog::onTextChanged(const QString &text)
{
    if (!text.isEmpty()) {
        QByteArray byteArray = text.toUtf8();
        if (m_formatComboBox->currentText() == "fat32") {
            if (byteArray.size() > 11) {
                m_fileNameEdit->setAlert(true);
                m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

                QAbstractButton *button = getButton(m_okCode);
                button->setEnabled(false);
            } else {
                m_fileNameEdit->setAlert(false);
                m_fileNameEdit->hideAlertMessage();

                QAbstractButton *button = getButton(m_okCode);
                button->setEnabled(true);
            }
        } else {
            if (byteArray.size() > 16) {
                m_fileNameEdit->setAlert(true);
                m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

                QAbstractButton *button = getButton(m_okCode);
                button->setEnabled(false);
            } else {
                m_fileNameEdit->setAlert(false);
                m_fileNameEdit->hideAlertMessage();

                QAbstractButton *button = getButton(m_okCode);
                button->setEnabled(true);
            }
        }
    }
}

void FormateDialog::onComboxFormatTextChange(const QString &text)
{
    QByteArray byteArray = m_fileNameEdit->text().toUtf8();
    if (text == "fat32") {
        if (byteArray.size() > 11) {
            m_fileNameEdit->setAlert(true);
            m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

            QAbstractButton *button = getButton(m_okCode);
            button->setEnabled(false);
        } else {
            m_fileNameEdit->setAlert(false);
            m_fileNameEdit->hideAlertMessage();

            QAbstractButton *button = getButton(m_okCode);
            button->setEnabled(true);
        }
    } else {
        if (byteArray.size() > 16) {
            m_fileNameEdit->setAlert(true);
            m_fileNameEdit->showAlertMessage(tr("The length exceeds the limit"), -1);

            QAbstractButton *button = getButton(m_okCode);
            button->setEnabled(false);
        } else {
            m_fileNameEdit->setAlert(false);
            m_fileNameEdit->hideAlertMessage();

            QAbstractButton *button = getButton(m_okCode);
            button->setEnabled(true);
        }
    }
}

void FormateDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == m_okCode) {
        if (m_fileNameEdit->text().isEmpty()) {
            DMDbusHandler::instance()->format(m_formatComboBox->currentText(), " ");
        } else {
            DMDbusHandler::instance()->format(m_formatComboBox->currentText(), m_fileNameEdit->text());
        }
    }
}
