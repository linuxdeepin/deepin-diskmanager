/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mountdialog.cpp
 *
 * @brief 实现分区挂载
 *
 * @date 2020-09-16 17:03
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
#include "mountdialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QDir>
#include <QVBoxLayout>

MountDialog::MountDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void MountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();

    setTitle(tr("Mount %1").arg(info.m_path));

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    DLabel *tipLabel = new DLabel(tr("Choose a mount point please"), this);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    DLabel *mountLabel = new DLabel(tr("Mount point:"));
    DFontSizeManager::instance()->bind(mountLabel, DFontSizeManager::T6);
//    m_fileChooserEdit = new DFileChooserEdit(this);
//    m_fileChooserEdit->setFileMode(QFileDialog::Directory);
//    m_fileChooserEdit->fileDialog()->setOption(QFileDialog::ShowDirsOnly);
//    m_fileChooserEdit->fileDialog()->setAcceptMode(QFileDialog::AcceptOpen);

    m_ComboBox = new DComboBox;
    m_ComboBox->setEditable(true);
    m_ComboBox->addItem("/mnt");
    m_ComboBox->addItem("/boot");
    m_ComboBox->addItem("/");
    m_ComboBox->addItem("/tmp");
    m_ComboBox->addItem("/var");
    m_ComboBox->addItem("/srv");
    m_ComboBox->addItem("/opt");
    m_ComboBox->addItem("/usr");
    m_ComboBox->addItem("/local");
    m_ComboBox->addItem("/media");

    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(mountLabel);
    mainLayout->addWidget(m_ComboBox);

    addButton(tr("Cancel"), true, ButtonNormal);
    m_okCode = addButton(tr("Mount"), false, ButtonRecommend);
//    getButton(okcode)->setDisabled(true);
    setOnButtonClickedClose(false);
}

void MountDialog::initConnection()
{
    connect(m_ComboBox->lineEdit(), &QLineEdit::textChanged, this, &MountDialog::onEditContentChanged);
    connect(this, &MountDialog::buttonClicked, this, &MountDialog::onButtonClicked);
}

void MountDialog::onEditContentChanged(const QString &content)
{
    if (content.isEmpty()) {
        getButton(m_okCode)->setDisabled(true);
    } else {
        QDir dir(content);
        if (dir.exists()) {
            getButton(m_okCode)->setDisabled(false);
//            m_fileChooserEdit->setAlert(false);
        } else {
            getButton(m_okCode)->setDisabled(true);
//            m_fileChooserEdit->setAlert(true);
        }
    }
}

void MountDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == m_okCode) {
        QDir dir(m_ComboBox->currentText());
        if (!dir.isEmpty()) {
            MessageBox messageBox;
            messageBox.setWarings(tr("The data under this mount point would be lost, please mount the directory to another location"), "", tr("OK"), tr("Cancel"));
            if (messageBox.exec() == 1) {
                DMDbusHandler::instance()->mount(m_ComboBox->currentText());
                close();
            }
        } else {
            DMDbusHandler::instance()->mount(m_ComboBox->currentText());
            close();
        }
    } else {
        close();
    }
}
