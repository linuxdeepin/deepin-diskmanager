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
#include "tipformatedialog.h"
#include <QDebug>
TipFormateDialog::TipFormateDialog(QWidget *parent) : TipDialog(parent)
{
    this->titleLable->setText(tr("XXX will be formatted"));
    this->tipLabel->setText(tr("The disk will be formatted, all data stored on the disk \n will be deleted, and cannot be undone. Please provide the \n name and format, and click Format content"));
    this->typeCombox->setItemText(0, "NTFS");
    this->mountLabel->hide();
    this->nameLineEdit->show();
    this->typeCombox->show();
    QList<QAbstractButton *> list = getButtons();
    for (int i = 0; i < list.size(); i++) {

        connect(list.at(i), &QAbstractButton::clicked, this, [ = ] {
            if (list.at(i)->text() == "Ok")
            {
                qDebug() << "XXX will be formatted";

            }
        });

    }
}
