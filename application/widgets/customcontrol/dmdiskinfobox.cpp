/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmdiskinfobox.cpp
 *
 *
 * @brief 磁盘信息数据类
 *
 * @date 2020-09-18 13:31
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "dmdiskinfobox.h"

DmDiskinfoBox::DmDiskinfoBox(int level, QObject *parent, QString diskPath, QString diskSize, int vgFlag, int luksFlag,
                             QString partitionPath, QString partitionSize, QString used, QString unused,
                             Sector sectorsUnallocated, Sector start, Sector end, QString fstype,
                             QString mountpoints, QString systemLabel, int flag)
    : QObject(parent)
    , m_level(level)
    , m_diskPath(diskPath)
    , m_diskSize(diskSize)
    , m_vgFlag(vgFlag)
    , m_luksFlag(luksFlag)
    , m_partitionPath(partitionPath)
    , m_partitionSize(partitionSize)
    , m_used(used)
    , m_unused(unused)
    , m_fstype(fstype)
    , m_mountpoints(mountpoints)
    , m_syslabel(systemLabel)
    , m_sectorsUnallocated(sectorsUnallocated)
    , m_start(start)
    , m_end(end)
    , m_flag(flag)
{
    m_childs.clear();
    m_id = 0;
}

DmDiskinfoBox::DmDiskinfoBox(QObject *parent)
    : QObject(parent)
{
    m_id = 0;
    m_level = 0;
    m_flag = 0;
}

DmDiskinfoBox::~DmDiskinfoBox()
{
    for (int i = 0; i < m_childs.length(); i++) {
        if (m_childs.at(i) != nullptr) {
            delete m_childs.at(i);
        }
    }
    m_childs.clear();
}

int DmDiskinfoBox::addChild(DmDiskinfoBox *child)
{
    child->m_level = m_level + 1;
    foreach (DmDiskinfoBox *item, m_childs) {
        if (item->m_id == child->m_id) {
            m_childs.removeOne(item);
            delete item;
        }
    }
    m_childs.append(child);
    return 0;
}

int DmDiskinfoBox::childCount()
{
    return m_childs.count();
}

