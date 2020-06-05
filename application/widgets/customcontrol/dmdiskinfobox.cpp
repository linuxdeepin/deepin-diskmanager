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
#include "dmdiskinfobox.h"
DmDiskinfoBox::DmDiskinfoBox(int level, QObject *parent, QString diskpath, QString disksize,
                             QString partitionpath, QString partitionsize, QString used, QString unused,
                             Sector sectors_unallocated, Sector start, Sector end, QString fstype,
                             QString mountpoints, QString systemlabel)
    : QObject(parent)
    , m_level(level)
    , m_diskpath(diskpath)
    , m_disksize(disksize)
    , m_partitionpath(partitionpath)
    , m_partitionsize(partitionsize)
    , m_used(used)
    , m_unused(unused)
    , m_fstype(fstype)
    , m_mountpoints(mountpoints)
    , m_syslabel(systemlabel)
    , m_sectors_unallocated(sectors_unallocated)
    , m_start(start)
    , m_end(end)
{
    childs.clear();
}

DmDiskinfoBox::DmDiskinfoBox(QObject *parent)
    : QObject(parent)
{
}

DmDiskinfoBox::~DmDiskinfoBox()
{
    for (int i = 0; i < childs.length(); i++) {
        if (childs.at(i) != nullptr) {
            delete childs.at(i);
        }
    }
    childs.clear();
}

int DmDiskinfoBox::addChild(DmDiskinfoBox *child)
{
    child->m_level = m_level + 1;
    foreach (DmDiskinfoBox *item, childs) {
        if (item->id == child->id) {
            childs.removeOne(item);
            delete item;
        }
    }
    childs.append(child);
    return 0;
}

int DmDiskinfoBox::childCount()
{
    return childs.count();
}

void DmDiskinfoBox::print()
{
    qDebug() << "----asdd----Id::" << id << "  level::" << m_level
             << "  disklabel:" << m_diskpath << " disksize::" << m_disksize
             << "  partitonlabel::" << m_partitionpath << " partitionsize::" << m_partitionsize << "  childs num::" << childs.count() << endl;
}
