// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


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
    qDebug() << "DmDiskinfoBox initialized";
}

DmDiskinfoBox::DmDiskinfoBox(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Creating empty DmDiskinfoBox";
    m_id = 0;
    m_level = 0;
    m_flag = 0;
    qDebug() << "Empty DmDiskinfoBox initialized";
}

DmDiskinfoBox::~DmDiskinfoBox()
{
    qDebug() << "Destroying DmDiskinfoBox, child count:" << m_childs.count();
    for (int i = 0; i < m_childs.length(); i++) {
        if (m_childs.at(i) != nullptr) {
            qDebug() << "Deleting child item:" << i;
            delete m_childs.at(i);
        }
    }
    m_childs.clear();
    qDebug() << "DmDiskinfoBox destroyed";
}

int DmDiskinfoBox::addChild(DmDiskinfoBox *child)
{
    qDebug() << "Adding child, current level:" << m_level
                       << "child id:" << child->m_id;
    child->m_level = m_level + 1;
    foreach (DmDiskinfoBox *item, m_childs) {
        if (item->m_id == child->m_id) {
            qDebug() << "Replacing existing child with same id";
            m_childs.removeOne(item);
            delete item;
        }
    }
    m_childs.append(child);
    qDebug() << "Child added, new child count:" << m_childs.count();
    return 0;
}

int DmDiskinfoBox::childCount()
{
    qDebug() << "Getting child count:" << m_childs.count();
    return m_childs.count();
}

