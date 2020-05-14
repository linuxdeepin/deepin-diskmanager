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
#include "dmtreeview.h"
#include <QDebug>
DmTreeview::DmTreeview(QWidget *parent) : DTreeView(parent)
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
    setVerticalScrollBarPolicy(Qt::  ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHeaderHidden(true);
    setIndentation(0);                   //去除树型节点之间的缩进
    setWindowFlags(Qt::FramelessWindowHint); //无边框
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    /* setAttribute(Qt::WA_TranslucentBackground)*/;//背景透明

    qDebug() << 2222222222222;
}
void DmTreeview::additem(QStandardItem *item, DiskInfoData &data)
{
    QStandardItem *pItem = new QStandardItem(data.disklabel);
    pItem->setData(QVariant::fromValue((data)), Qt::UserRole + 1);
    item->appendRow(pItem);
    // expand(m_pDataModel->indexFromItem(item));
    setExpanded(m_model->indexFromItem(item), true);

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
//    sort();
}
void DmTreeview::initdelegate()
{
    m_delegate = new DmTreeviewDelegate(this);
    this->setItemDelegate(m_delegate);


}

QStandardItem *DmTreeview::getcuritem()
{
    QModelIndex  index = currentIndex();
    if (!index.isValid()) {
        return  nullptr;
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
    qDebug() << data.disksize << data.disklabel << data.partitionsize << data.partitonlabel;
    emit sigselectitem(current);
    // emit sigCurSelectChanged();

}
void DmTreeview::mousePressEvent(QMouseEvent *event)
{
    DTreeView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        setExpanded(currentIndex(), !isExpanded(currentIndex()));
    }
}
void DmTreeview::addItem(DmDiskinfoBox *infobox, QStandardItem *pcurItem)
{
    QStandardItem *t_item;
    DiskInfoData data;
    //qDebug() << infobox->level;
    data.disksize = infobox->m_disksize;
    data.disklabel = infobox->m_diskpath;
    data.partitionsize = infobox->partitionsize;
    data.partitonlabel = infobox->partitonlabel;
    //  qDebug() << data.disksize << data.disklabel;


    //  qDebug() << infobox->disksize;
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
        //qDebug() << data.level;

        this->additem(pcurItem, data);

    }

    foreach (auto sub, infobox->childs) {
        addSubItem(sub, t_item);
    }
}

void DmTreeview::addTopItem(DmDiskinfoBox *infobox)
{
    infobox->m_level = 0;
    addItem(infobox);
}

void DmTreeview::addSubItem(DmDiskinfoBox *infobox, QStandardItem *pcurItem)
{
    if (infobox->m_level < 1) {
        infobox->m_level = 0;
    }
    addItem(infobox, pcurItem);
}

void DmTreeview::showEvent(QShowEvent *event)
{
    setDefaultdmItem();
    return DTreeView::showEvent(event);
}
QModelIndex DmTreeview::setDefaultdmItem()
{

    //QModelIndex index = m_pSortViewFilter->index(0, 0, getRootItemIndex());
    this->setCurrentIndex(model()->index(0, 0).child(0, 0));
    return model()->index(0, 0);

}
QStandardItem *DmTreeview::getRootItem()
{
    return  m_model->item(0);

}
QModelIndex DmTreeview::getRootItemIndex()
{
    return  m_pSortViewFilter->mapFromSource(getRootItem()->index());



}
void DmTreeview::sort()
{
    return m_pSortViewFilter->sort(0, Qt::DescendingOrder);
}
