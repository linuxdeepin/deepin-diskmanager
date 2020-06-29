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
#include "dmtreeview.h"
#include <QDebug>
DmTreeview::DmTreeview(QWidget *parent)
    : DTreeView(parent)
{
    initUI();
    initmodel();
    initdelegate();
}
void DmTreeview::initUI()
{
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setVerticalScrollMode(ScrollPerItem);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHeaderHidden(true);
    setIndentation(0); //去除树型节点之间的缩进
    setWindowFlags(Qt::FramelessWindowHint); //无边框
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    /* setAttribute(Qt::WA_TranslucentBackground)*/; //背景透明
}
void DmTreeview::additem(QStandardItem *item, DiskInfoData &data, int flag)
{
    QStandardItem *pItem = new QStandardItem(data.diskpath);
    pItem->setData(QVariant::fromValue((data)), Qt::UserRole + 1);
    item->appendRow(pItem);
    if (flag == 0) {
        setExpanded(m_model->index(0, 0), true);
    } else {

    }
}
QStandardItem *DmTreeview::addtopitem(DiskInfoData &data)
{
    QStandardItem *pItem = new QStandardItem;
    pItem->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    m_model->appendRow(pItem);
    return pItem;
}
void DmTreeview::initmodel()
{
    m_model = new QStandardItemModel(this);
    m_pSortViewFilter = new QSortFilterProxyModel(this);
    this->setModel(m_model);
}
void DmTreeview::initdelegate()
{
    m_delegate = new DmTreeviewDelegate(this);
    this->setItemDelegate(m_delegate);
}
QStandardItem *DmTreeview::getcuritem()//获取当前ｉｔｅｍ
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return nullptr;
    } else {
        return m_model->itemFromIndex(index);
    }
}
QStandardItem *DmTreeview::getModelByIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        return m_model->itemFromIndex(index);
    }
    return nullptr;
}
void DmTreeview::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    DiskInfoData data = current.data(Qt::UserRole + 1).value<DiskInfoData>();
    qDebug() << data.diskpath << data.disksize << data.partitionsize << data.partitonpath << data.level << data.used << data.unused << data.start << data.end << data.fstype << data.mountpoints << data.syslabel;
    curNum = current.row();
    diskNum = current.parent().row();
    emit sigselectitem(current);
    emit sigCurSelectChanged(data.diskpath, data.partitonpath, data.start, data.end);
    diskSize = data.disksize;

}
void DmTreeview::mousePressEvent(QMouseEvent *event)
{
    DTreeView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        setExpanded(currentIndex(), !isExpanded(currentIndex()));
    }
}
void DmTreeview::addItem(DmDiskinfoBox *infobox, int flag, QStandardItem *pcurItem)//增加节点
{
    QStandardItem *t_item = nullptr;
    DiskInfoData data;
    data.disksize = infobox->m_disksize;
    data.diskpath = infobox->m_diskpath;
    data.partitionsize = infobox->m_partitionsize;
    data.partitonpath = infobox->m_partitionpath;
    data.used = infobox->m_used;
    data.unused = infobox->m_unused;
    data.start = infobox->m_start;
    data.end = infobox->m_end;
    data.sectors_unallocated = infobox->m_sectors_unallocated;
    data.fstype = infobox->m_fstype;
    data.syslabel = infobox->m_syslabel;
    data.mountpoints = infobox->m_mountpoints;
    if (infobox->m_level <= 0) {
        data.level = 0;
        t_item = this->addtopitem(data);
    } else {
        if (pcurItem == nullptr) {
            pcurItem = this->getcuritem();
        }
        if (pcurItem == nullptr) {
            return;
        }
        QVariant var = pcurItem->index().data(Qt::UserRole + 1);
        DiskInfoData parent_data = var.value<DiskInfoData>();
        data.level = parent_data.level + 1;
        this->additem(pcurItem, data, flag);
    }

    foreach (auto sub, infobox->childs) {
        addSubItem(sub, t_item, flag);
    }
}

void DmTreeview::addTopItem(DmDiskinfoBox *infobox, int flag)
{
    infobox->m_level = 0;
    addItem(infobox, flag);
}

void DmTreeview::addSubItem(DmDiskinfoBox *infobox, QStandardItem *pcurItem, int flag)
{
    if (infobox->m_level < 1) {
        infobox->m_level = 0;
    }
    addItem(infobox, flag, pcurItem);
}
QModelIndex DmTreeview::setDefaultdmItem()//设置默认选中节点
{
    //QModelIndex index = m_pSortViewFilter->index(0, 0, getRootItemIndex());
    this->setCurrentIndex(model()->index(0, 0).child(0, 0));
    return model()->index(0, 0);
}

void DmTreeview::setRefreshItem(int devicenum, int num)//设置刷新后默认选择操作分区
{

    this->setCurrentIndex(model()->index(devicenum, 0).child(num, 0));
    setExpanded(model()->index(devicenum, 0), true);
}

int DmTreeview::currentNum()//返回当前选中分区
{

    return curNum;
}

int DmTreeview::currentTopNum()
{
    return diskNum;
}
QStandardItem *DmTreeview::getRootItem()//获取根节点
{
    return m_model->item(0);
}
QModelIndex DmTreeview::getRootItemIndex()
{
    return m_pSortViewFilter->mapFromSource(getRootItem()->index());
}
void DmTreeview::sort()
{
    return m_pSortViewFilter->sort(0, Qt::DescendingOrder);
}
