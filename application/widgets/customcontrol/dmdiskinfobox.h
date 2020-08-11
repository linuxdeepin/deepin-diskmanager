/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DMDISKINFOBOX_H
#define DMDISKINFOBOX_H

#include "commondef.h"
#include <QObject>
#include <QDebug>

class DmDiskinfoBox : public QObject
{
    Q_OBJECT
public:
    DmDiskinfoBox(int level, QObject *parent = nullptr, QString diskpath = "", QString disksize = "", QString partitionpath = "",
                  QString partitionsize = "", QString used = "", QString unused = "",
                  Sector sectors_unallocated = 0, Sector start = 0, Sector end = 0, QString fstype = "",
                  QString mountpoints = "", QString systemlabel = "", int flag = 0);
    DmDiskinfoBox(QObject *parent = nullptr);
    ~DmDiskinfoBox();

public:
    int addChild(DmDiskinfoBox *child);
    int childCount();

public:
    int id;
    int m_level;
    QString m_diskpath;
    QString m_disksize;
    QString m_partitionpath;
    QString m_partitionsize;
    QString m_used;
    QString m_unused;
    QString m_total;
    QString m_fstype;
    QString m_mountpoints;
    QString m_syslabel;
    Sector m_sectors_unallocated;
    Sector m_start;
    Sector m_end;
    int m_flag;
    QList<DmDiskinfoBox *> childs;
};

#endif // DMDISKINFOBOX_H
