/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmtreeview.cpp
 *
 * @brief 实现树状图
 *
 * @date 2020-09-18 10:50
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
#include "dmtreeview.h"

#include <QDebug>

DmTreeview::DmTreeview(QWidget *parent)
    : DTreeView(parent)
{
    initUI();
    initModel();
    initDelegate();
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

QStandardItem * DmTreeview::addItem(QStandardItem *item, const DiskInfoData &data, int flag)
{
    if (data.m_level == DMDbusHandler::Disk || data.m_level == DMDbusHandler::VolumeGroup) {
        QStandardItem *standardItem = new QStandardItem();
        standardItem->setAccessibleDescription(data.m_diskPath);
        standardItem->setData(QVariant::fromValue(data), Qt::UserRole + 1);
        item->appendRow(standardItem);
        return standardItem;
    } else {
        QStandardItem *standardItem = new QStandardItem(data.m_diskPath);
        standardItem->setAccessibleDescription(data.m_partitionPath);
        standardItem->setData(QVariant::fromValue((data)), Qt::UserRole + 1);
        item->appendRow(standardItem);
        if (flag == 0) {
            setExpanded(m_model->index(0, 0), true);
        }

        return standardItem;
    }
}

QStandardItem *DmTreeview::addTopItem(const DiskInfoData &data)
{
    QStandardItem *item = new QStandardItem;
    item->setAccessibleDescription(data.m_diskPath);
    item->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    m_model->appendRow(item);
    return item;
}

void DmTreeview::initModel()
{
    m_model = new QStandardItemModel(this);
    m_sortViewFilter = new QSortFilterProxyModel(this);
    setModel(m_model);
}

void DmTreeview::initDelegate()
{
    m_delegate = new DmTreeviewDelegate(this);
    setItemDelegate(m_delegate);
}

QStandardItem *DmTreeview::getCurItem()//获取当前ｉｔｅｍ
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return nullptr;
    } else {
        return m_model->itemFromIndex(index);
    }
}

QStandardItem *DmTreeview::getItemByIndex(const QModelIndex &index)
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
    qDebug() << data.m_diskPath << data.m_diskSize << data.m_partitionSize << data.m_partitionPath << data.m_level << data.m_used << data.m_unused << data.m_start << data.m_end << data.m_fstype << data.m_mountpoints << data.m_sysLabel;

    if (data.m_level == DMDbusHandler::Other) {
        return;
    }

    m_curNum = current.row();
    m_diskNum = current.parent().row();
    m_groupNum = current.parent().parent().row();

    emit selectItem(current);
    emit curSelectChanged(data.m_diskPath, data.m_partitionPath, data.m_start, data.m_end, data.m_level);

    m_diskSize = data.m_diskSize;
}

void DmTreeview::mousePressEvent(QMouseEvent *event)
{
    DTreeView::mousePressEvent(event);

    QModelIndex curIndex = indexAt(event->pos());
    QModelIndex index = curIndex.sibling(curIndex.row(),0);

    if ((event->button() == Qt::LeftButton) && (index.isValid())) {
        setExpanded(currentIndex(), !isExpanded(currentIndex()));
    }
}

void DmTreeview::addItem(DmDiskinfoBox *infoBox, int flag, QStandardItem *purItem)//增加节点
{
    QStandardItem *item = nullptr;
    DiskInfoData data;
    data.m_diskSize = infoBox->m_diskSize;
    data.m_diskPath = infoBox->m_diskPath;
    data.m_partitionSize = infoBox->m_partitionSize;
    data.m_partitionPath = infoBox->m_partitionPath;
    data.m_used = infoBox->m_used;
    data.m_unused = infoBox->m_unused;
    data.m_start = infoBox->m_start;
    data.m_end = infoBox->m_end;
    data.m_sectorsUnallocated = infoBox->m_sectorsUnallocated;
    data.m_fstype = infoBox->m_fstype;
    data.m_sysLabel = infoBox->m_syslabel;
    data.m_mountpoints = infoBox->m_mountpoints;
    data.m_flag = infoBox->m_flag;
    data.m_vgFlag = infoBox->m_vgFlag;

    if ((infoBox->m_level <= 0) && !isGroup) {
        data.m_level = DMDbusHandler::Disk;
        item = addTopItem(data);
        foreach (auto sub, infoBox->m_childs) {
            addSubItem(sub, item, flag);
        }
    } else if(infoBox->m_level == DMDbusHandler::Other) {
        data.m_level = DMDbusHandler::Other;
        item = addTopItem(data);
        m_lstStandardItem << item;
        foreach (auto sub, infoBox->m_childs) {
            addSubItem(sub, item, flag);
        }
    }else {
        if (purItem == nullptr) {
            purItem = getCurItem();

            if (purItem == nullptr) {
                return;
            }
        }

        if (infoBox->m_level <= 0) {

            QVariant var = purItem->index().data(Qt::UserRole + 1);
            DiskInfoData parent_data = var.value<DiskInfoData>();
            data.m_level = DMDbusHandler::Disk;

            item = addItem(purItem, data, flag);
            foreach (auto sub, infoBox->m_childs) {
                addSubItem(sub, item, flag);
            }
        } else if (infoBox->m_level == DMDbusHandler::VolumeGroup) {
            QVariant var = purItem->index().data(Qt::UserRole + 1);
            DiskInfoData parent_data = var.value<DiskInfoData>();
            data.m_level = DMDbusHandler::VolumeGroup;

            item = addItem(purItem, data, flag);
            foreach (auto sub, infoBox->m_childs) {
                addSubItem(sub, item, flag);
            }
        } else {
            QVariant var = purItem->index().data(Qt::UserRole + 1);
            DiskInfoData parent_data = var.value<DiskInfoData>();
            data.m_level = parent_data.m_level + 1;
            addItem(purItem, data, flag);
        }
    }
}

void DmTreeview::clearData()
{
    m_lstStandardItem.clear();
}

void DmTreeview::addTopItem(DmDiskinfoBox *mailBox, int flag)
{
    if (mailBox->m_level == DMDbusHandler::Other) {
        isGroup = true;
        addItem(mailBox, flag);
    } else {
        isGroup = false;
        mailBox->m_level = 0;
        addItem(mailBox, flag);
    }
}

void DmTreeview::addSubItem(DmDiskinfoBox *mailBox, QStandardItem *curItem, int flag)
{
    if (mailBox->m_level < 1) {
        mailBox->m_level = 0;
    }

    addItem(mailBox, flag, curItem);
}

QModelIndex DmTreeview::setDefaultdmItem()//设置默认选中节点
{
    //QModelIndex index = m_pSortViewFilter->index(0, 0, getRootItemIndex());
    setCurrentIndex(model()->index(0, 0).child(0, 0));
    setExpanded(model()->index(0, 0), true);

    return model()->index(0, 0);
}

void DmTreeview::setRefreshItem(int devicenum, int num)//设置刷新后默认选择操作分区
{
    if (-1 == devicenum) {
        setCurrentIndex(m_model->item(num)->index());
        setExpanded(m_model->item(num)->index(), true);
    } else {
        setCurrentIndex(model()->index(devicenum, 0).child(num, 0));
        setExpanded(model()->index(devicenum, 0), true);
    }
}

void DmTreeview::setRefreshItem(int devicenum, int num, int groupNum)
{
    if (!m_lstStandardItem.isEmpty()) {
        for (int i = 0; i < m_lstStandardItem.count(); i++) {
            QStandardItem *item = m_lstStandardItem.at(i);
            for (int j = 0; j < item->rowCount(); j++) {
                QStandardItem *child = item->child(j);
                if (-1 == groupNum) {
                    if (i == devicenum && j == num) {
                        setCurrentIndex(child->index());
                        setExpanded(item->index(), true);
                        return;
                    }
                } else {
                    for (int k = 0; k < child->rowCount(); k++) {
                        if (i == groupNum && j == devicenum && k == num) {
                            QStandardItem *childItem = child->child(k);
                            setCurrentIndex(childItem->index());
                            setExpanded(child->index(), true);
                            setExpanded(item->index(), true);
                            return;
                        }
                    }
                }
            }
        }
    }
}

int DmTreeview::getCurrentNum()//返回当前选中分区
{
    return m_curNum;
}

int DmTreeview::getCurrentTopNum()
{
    return m_diskNum;
}

int DmTreeview::getCurrentGroupNum()
{
    return m_groupNum;
}

QStandardItem *DmTreeview::getRootItem()//获取根节点
{
    return m_model->item(0);
}

QModelIndex DmTreeview::getRootItemIndex()
{
    return m_sortViewFilter->mapFromSource(getRootItem()->index());
}

void DmTreeview::sort()
{
    m_sortViewFilter->sort(0, Qt::DescendingOrder);
}
