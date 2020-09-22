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

    QHBoxLayout *layoutName = new QHBoxLayout;
    DLabel *fileName = new DLabel(tr("Name:"), this);
    fileName->setMinimumWidth(76);
    m_fileNameEdit   = new DLineEdit(this);
    if (m_fileNameEdit->text().isEmpty())
        m_fileNameEdit->lineEdit()->setPlaceholderText(tr("Name"));
    layoutName->addWidget(fileName);
    layoutName->addWidget(m_fileNameEdit);

    QHBoxLayout *layoutFormat = new QHBoxLayout;
    DLabel *formatName = new DLabel(tr("File system:"), this);
    m_formatComboBox = new DComboBox(this);
    m_formatComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QStringList fslist = DMDbusHandler::instance()->getAllSupportFileSystem();
    fslist.removeOne("linux-swap");
    m_formatComboBox->addItems(fslist);

    if (-1 == fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType)))) {
        m_formatComboBox->setCurrentIndex(fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(11))));
    } else {
        m_formatComboBox->setCurrentIndex(fslist.indexOf(Utils::fileSystemTypeToString(static_cast<FSType>(info.m_fileSystemType))));
    }

    layoutFormat->addWidget(formatName);
    layoutFormat->addWidget(m_formatComboBox);
    mainLayout->addWidget(tipLabel);
    mainLayout->addSpacing(60);
    mainLayout->addLayout(layoutName);
    mainLayout->addLayout(layoutFormat);

    addButton(tr("Cancel"), true, ButtonNormal);
    m_okCode = addButton(tr("Format"), false, ButtonWarning);
}

void FormateDialog::initConnection()
{
    connect(this, &FormateDialog::buttonClicked, this, &FormateDialog::onButtonClicked);
}

void FormateDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == m_okCode) {
        DMDbusHandler::instance()->format(m_formatComboBox->currentText(), m_fileNameEdit->text());
    }
}
