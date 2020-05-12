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
#include "dmdiskinfobox.h"


#include <QDebug>
DmDiskinfoBox::DmDiskinfoBox()
{
    id = 0;
    level = 0;
    QString disklabel = "";
    QString disksize = "";
    QString partitonlabel = "";
    QString partitionsize = "";
    childs.clear();

}

int DmDiskinfoBox::addChild(DmDiskinfoBox *child)
{
    child->level = level + 1;
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
    qDebug() << "----asdd----Id::" << id << "  level::" << level
             << "  disklabel:" << disklabel << " disksize::" << disksize
             << "  partitonlabel::" << partitonlabel << " partitionsize::" << partitionsize  << "  childs num::" << childs.count() << endl;

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
