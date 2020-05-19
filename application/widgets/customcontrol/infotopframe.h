/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     wangzhixuan <wangzhixuan@uniontech.com>
* Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
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
#ifndef INFOTOPFRAME_H
#define INFOTOPFRAME_H

#include <DFrame>
#include <DWidget>
#include <DLabel>

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

class InfoTopFrame : public DFrame
{
    Q_OBJECT
public:
    explicit InfoTopFrame(DWidget *parent = nullptr);


public:
    void setShowDiskInfo();

private:
    void InitLeftInfo();
    void InitRightInfo();

private:
    QHBoxLayout *mainLayout = nullptr;

    DLabel  *picLabel = nullptr;
    DLabel  *nameLabel = nullptr;
    DLabel  *typeLabel = nullptr;

    DLabel  *allnameLabel = nullptr;
    DLabel  *allmemoryLabel = nullptr;
};

#endif // INFOTOPFRAME_H
