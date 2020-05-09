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
#include "tipdialog.h"

TipDialog::TipDialog(QWidget *parent) : DDialog(parent)
{
    this->setIcon(QIcon::fromTheme("exception-logo"));
    mainFrame = new DFrame(this);

    mainFrame->setFrameRounded(false);
    mainFrame->setFrameStyle(DFrame::NoFrame);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setSpacing(5);
    QFont font2;
    font2.setPointSize(10);
    titleLable = new DLabel(tr("XXX will be formatted"));
    titleLable->setFont(font2);
    titleLable->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPointSize(11);
    tipLabel = new DLabel(tr("The disk will be formatted, all data stored on the disk \n will be deleted, and cannot be undone. Please provide the \n name and format, and click Format content"));
    tipLabel->setFont(font);
    tipLabel->setAlignment(Qt::AlignCenter);
    mountLabel = new DLabel("Mount point:");
    QFont font3;
    font3.setPointSize(11);
    mountLabel->setFont(font3);
    mountLabel->hide();
    nameLineEdit = new DLineEdit(mainFrame);
    if (nameLineEdit->text().isEmpty()) {
        nameLineEdit->lineEdit()->setPlaceholderText(tr("name"));
    }

    typeCombox = new DComboBox(mainFrame);
    typeCombox->addItem("NTFS");
    mainLayout->addWidget(titleLable);
    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(mountLabel);
    mainLayout->addWidget(nameLineEdit);
    mainLayout->addWidget(typeCombox);
    this->addContent(mainFrame);

    this->addButton(tr("Cancel"), false, ButtonNormal);
    this->addButton(tr("Ok"), true, ButtonRecommend);
}
