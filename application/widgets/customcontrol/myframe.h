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
#ifndef MYFRAME_H
#define MYFRAME_H

#include <QWidget>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
DWIDGET_USE_NAMESPACE

class MyFrame : public DFrame
{
    Q_OBJECT
public:
    explicit MyFrame(QWidget *parent = nullptr);

signals:

public slots:

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // MYFRAME_H
