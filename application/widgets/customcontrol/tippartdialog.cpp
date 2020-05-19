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
#include "tippartdialog.h"
#include <QDebug>
TipPartDialog::TipPartDialog(QWidget *parent) : TipDialog(parent)
{



}

void TipPartDialog::getFlagShow(const int &showFlag)
{
    m_flag = showFlag;
    switch (showFlag) {
    case 0:
        showPartDialog();
        break;
    case 1:
        showFormateDialog();
        break;
    default:
        break;
    }
}

void TipPartDialog::showPartDialog()
{
    this->setFixedSize(350, 200);
    this->titleLable->setText(tr("XXX system space is in operation"));
    this->tipLabel->setText(tr("The disk is in use. Partition the space in the disk.\n Click OK to confirm the operation"));
    this->mountLabel->hide();
    this->nameLineEdit->hide();
    this->typeCombox->hide();
    this->show();
}

void TipPartDialog::showFormateDialog()
{
    this->setFixedSize(450, 300);
    this->titleLable->setText(tr("XXX will be formatted"));
    this->tipLabel->setText(tr("The disk will be formatted, all data stored on the disk \n will be deleted, and cannot be undone. Please provide the \n name and format, and click Format content"));
    this->typeCombox->setItemText(0, "NTFS");
    this->mountLabel->hide();
    this->nameLineEdit->show();
    this->typeCombox->show();
    this->show();
}
