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
#ifndef TIPDIALOG_H
#define TIPDIALOG_H

#include <DDialog>
#include <DLabel>
#include <DLineEdit>
#include <DComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
#include <QAbstractButton>

DWIDGET_USE_NAMESPACE

class TipDialog : public DDialog
{
    Q_OBJECT
public:
    explicit TipDialog(QWidget *parent = nullptr);
    DLabel *titleLable;
    DLabel *tipLabel;
    DLabel *mountLabel;
    DLineEdit *nameLineEdit;
    DComboBox *typeCombox;
signals:

public slots:
private:
    DFrame *mainFrame;
};

#endif // TIPDIALOG_H
