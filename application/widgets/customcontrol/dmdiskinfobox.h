/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     linxun <linxun@uniontech.com>
* Maintainer: linxun <linxun@uniontech.com>
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
#ifndef DMDISKINFOBOX_H
#define DMDISKINFOBOX_H

#include <QObject>
#include "commondef.h"

class DmDiskinfoBox
{
public:
    DmDiskinfoBox(int level, QString diskpath = "", QString disksize = "", QString partitionpath = "",
                  QString partitionsize = "", double used = 0, double unused = 0,
                  Sector sectors_unallocated = 0, Sector start = 0, Sector end = 0);
    DmDiskinfoBox() {}
    ~DmDiskinfoBox();

public:
    int addChild(DmDiskinfoBox *child);
    int childCount();
    void print();
public:
    int id;
    int m_level;
    QString m_diskpath;
    QString m_disksize;
    QString m_partitionpath;
    QString m_partitionsize;
    double m_used;
    double m_unused;
    Sector m_sectors_unallocated;
    Sector m_start;
    Sector m_end;
    QList<DmDiskinfoBox *> childs;
};

#endif // DMDISKINFOBOX_H
