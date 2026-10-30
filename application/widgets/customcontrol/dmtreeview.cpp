// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "dmtreeview.h"

#include <QDebug>

DmTreeview::DmTreeview(QWidget *parent)
    : DTreeView(parent)
{
    qCDebug(appLog) << "DmTreeview constructor";
    initUI();
    initModel();
    initDelegate();
    qCDebug(appLog) << "DmTreeview initialized";
}

void DmTreeview::initUI()
{
    qCDebug(appLog) << "DmTreeview::initUI called";
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
    qCDebug(appLog) << "DmTreeview::initUI finished";
}

QStandardItem *DmTreeview::addItem(QStandardItem *item, const DiskInfoData &data, int flag)
{
    qCDebug(appLog) << "Adding item, level:" << data.m_level << "path:" << data.m_diskPath;
    if (data.m_level == DMDbusHandler::DISK || data.m_level == DMDbusHandler::VOLUMEGROUP) {
        qCDebug(appLog) << "Adding DISK or VOLUMEGROUP item";
        QStandardItem *standardItem = new QStandardItem();
        standardItem->setAccessibleDescription(data.m_diskPath);
        standardItem->setData(QVariant::fromValue(data), Qt::UserRole + 1);
        item->appendRow(standardItem);
        qCDebug(appLog) << "DISK or VOLUMEGROUP item added";
        return standardItem;
    } else {
        qCDebug(appLog) << "Adding other item with partition path:" << data.m_partitionPath;
        QStandardItem *standardItem = new QStandardItem(data.m_diskPath);
        standardItem->setAccessibleDescription(data.m_partitionPath);
        standardItem->setData(QVariant::fromValue((data)), Qt::UserRole + 1);
        item->appendRow(standardItem);
        if (flag == 0) {
            qCDebug(appLog) << "Expanding index (0,0)";
            setExpanded(m_model->index(0, 0), true);
        }
        qCDebug(appLog) << "Other item added";
        return standardItem;
    }
}

QStandardItem *DmTreeview::addTopItem(const DiskInfoData &data)
{
    qCDebug(appLog) << "Adding top item, path:" << data.m_diskPath;
    QStandardItem *item = new QStandardItem;
    item->setAccessibleDescription(data.m_diskPath);
    item->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    m_model->appendRow(item);
    qCDebug(appLog) << "Top item added successfully";
    return item;
}

void DmTreeview::initModel()
{
    qCDebug(appLog) << "DmTreeview::initModel called";
    m_model = new QStandardItemModel(this);
    m_sortViewFilter = new QSortFilterProxyModel(this);
    setModel(m_model);
    qCDebug(appLog) << "DmTreeview::initModel finished";
}

void DmTreeview::initDelegate()
{
    qCDebug(appLog) << "DmTreeview::initDelegate called";
    m_delegate = new DmTreeviewDelegate(this);
    setItemDelegate(m_delegate);
    qCDebug(appLog) << "DmTreeview::initDelegate finished";
}

QStandardItem *DmTreeview::getCurItem() //获取当前ｉｔｅｍ
{
    qCDebug(appLog) << "DmTreeview::getCurItem called";
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        qCDebug(appLog) << "Current index is invalid, returning nullptr";
        return nullptr;
    } else {
        qCDebug(appLog) << "Returning item from current index";
        return m_model->itemFromIndex(index);
    }
}

QStandardItem *DmTreeview::getItemByIndex(const QModelIndex &index)
{
    // qCDebug(appLog) << "DmTreeview::getItemByIndex called";
    if (index.isValid()) {
        // qCDebug(appLog) << "Index is valid, returning item from index";
        return m_model->itemFromIndex(index);
    }
    // qCDebug(appLog) << "Index is invalid, returning nullptr";
    return nullptr;
}

void DmTreeview::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    qCDebug(appLog) << "DmTreeview::currentChanged called";
    Q_UNUSED(previous);
    DiskInfoData data = current.data(Qt::UserRole + 1).value<DiskInfoData>();
    qCDebug(appLog) << data.m_diskPath << data.m_diskSize << data.m_partitionSize << data.m_partitionPath << data.m_level << data.m_used << data.m_unused << data.m_start << data.m_end << data.m_fstype << data.m_mountpoints << data.m_sysLabel;

    if (data.m_level == DMDbusHandler::OTHER) {
        qCDebug(appLog) << "Ignoring OTHER level item";
        return;
    }

    m_curNum = current.row();
    m_diskNum = current.parent().row();
    m_groupNum = current.parent().parent().row();

    qCDebug(appLog) << "Selection position - curNum:" << m_curNum << "diskNum:" << m_diskNum << "groupNum:" << m_groupNum;
    
    emit selectItem(current);
    emit curSelectChanged(data.m_diskPath, data.m_partitionPath, data.m_start, data.m_end, data.m_level);

    m_diskSize = data.m_diskSize;
    qCDebug(appLog) << "Current change handled, disk size:" << m_diskSize;
}

void DmTreeview::mousePressEvent(QMouseEvent *event)
{
    qCDebug(appLog) << "DmTreeview::mousePressEvent called";
    DTreeView::mousePressEvent(event);

    QModelIndex curIndex = indexAt(event->pos());
    QModelIndex index = curIndex.sibling(curIndex.row(), 0);

    if ((event->button() == Qt::LeftButton) && (index.isValid())) {
        qCDebug(appLog) << "Left mouse button pressed and index is valid, toggling expanded state";
        setExpanded(currentIndex(), !isExpanded(currentIndex()));
    }
    qCDebug(appLog) << "DmTreeview::mousePressEvent finished";
}

void DmTreeview::addItem(DmDiskinfoBox *infoBox, int flag, QStandardItem *purItem) //增加节点
{
    qCDebug(appLog) << "DmTreeview::addItem (overload) called";
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
    data.m_luksFlag = infoBox->m_luksFlag;

    if ((infoBox->m_level <= 0) && !isGroup) {
        qCDebug(appLog) << "InfoBox level is 0 or less and not a group";
        data.m_level = DMDbusHandler::DISK;
        item = addTopItem(data);
        foreach (auto sub, infoBox->m_childs) {
            qCDebug(appLog) << "Adding sub item for DISK level";
            addSubItem(sub, item, flag);
        }
    } else if (infoBox->m_level == DMDbusHandler::OTHER) {
        qCDebug(appLog) << "InfoBox level is OTHER";
        data.m_level = DMDbusHandler::OTHER;
        item = addTopItem(data);
        m_lstStandardItem << item;
        foreach (auto sub, infoBox->m_childs) {
            qCDebug(appLog) << "Adding sub item for OTHER level";
            addSubItem(sub, item, flag);
        }
    } else {
        qCDebug(appLog) << "InfoBox level is not DISK or OTHER, or is a group";
        if (purItem == nullptr) {
            qCDebug(appLog) << "purItem is nullptr, getting current item";
            purItem = getCurItem();

            if (purItem == nullptr) {
                qCDebug(appLog) << "Current item is also nullptr, returning";
                return;
            }
        }

        if (infoBox->m_level <= 0) {
            qCDebug(appLog) << "InfoBox level is 0 or less";
            QVariant var = purItem->index().data(Qt::UserRole + 1);
            DiskInfoData parent_data = var.value<DiskInfoData>();
            data.m_level = DMDbusHandler::DISK;

            item = addItem(purItem, data, flag);
            foreach (auto sub, infoBox->m_childs) {
                qCDebug(appLog) << "Adding sub item for DISK level (nested)";
                addSubItem(sub, item, flag);
            }
        } else if (infoBox->m_level == DMDbusHandler::VOLUMEGROUP) {
            qCDebug(appLog) << "InfoBox level is VOLUMEGROUP";
            QVariant var = purItem->index().data(Qt::UserRole + 1);
            DiskInfoData parent_data = var.value<DiskInfoData>();
            data.m_level = DMDbusHandler::VOLUMEGROUP;

            item = addItem(purItem, data, flag);
            foreach (auto sub, infoBox->m_childs) {
                qCDebug(appLog) << "Adding sub item for VOLUMEGROUP level (nested)";
                addSubItem(sub, item, flag);
            }
        } else {
            qCDebug(appLog) << "InfoBox level is neither DISK nor VOLUMEGROUP (nested)";
            QVariant var = purItem->index().data(Qt::UserRole + 1);
            DiskInfoData parent_data = var.value<DiskInfoData>();
            data.m_level = parent_data.m_level + 1;
            addItem(purItem, data, flag);
        }
    }
    qCDebug(appLog) << "DmTreeview::addItem (overload) finished";
}

void DmTreeview::clearData()
{
    qCDebug(appLog) << "DmTreeview::clearData called";
    m_lstStandardItem.clear();
    qCDebug(appLog) << "DmTreeview::clearData finished";
}

void DmTreeview::addTopItem(DmDiskinfoBox *mailBox, int flag)
{
    qCDebug(appLog) << "DmTreeview::addTopItem (overload) called";
    if (mailBox->m_level == DMDbusHandler::OTHER) {
        qCDebug(appLog) << "Mailbox level is OTHER, setting isGroup to true";
        isGroup = true;
        addItem(mailBox, flag);
    } else {
        qCDebug(appLog) << "Mailbox level is not OTHER, setting isGroup to false and level to 0";
        isGroup = false;
        mailBox->m_level = 0;
        addItem(mailBox, flag);
    }
    qCDebug(appLog) << "DmTreeview::addTopItem (overload) finished";
}

void DmTreeview::addSubItem(DmDiskinfoBox *mailBox, QStandardItem *curItem, int flag)
{
    qCDebug(appLog) << "DmTreeview::addSubItem called";
    if (mailBox->m_level < 1) {
        qCDebug(appLog) << "Mailbox level is less than 1, setting to 0";
        mailBox->m_level = 0;
    }

    addItem(mailBox, flag, curItem);
    qCDebug(appLog) << "DmTreeview::addSubItem finished";
}

QModelIndex DmTreeview::setDefaultdmItem() //设置默认选中节点
{
    qCDebug(appLog) << "DmTreeview::setDefaultdmItem called";
    //QModelIndex index = m_pSortViewFilter->index(0, 0, getRootItemIndex());
#if QT_VERSION_MAJOR > 5
    qCDebug(appLog) << "QT_VERSION_MAJOR > 5, setting current index";
    setCurrentIndex(model()->index(0, 0, model()->index(0, 0)));
#else
    qCDebug(appLog) << "QT_VERSION_MAJOR <= 5, setting current index";
    setCurrentIndex(model()->index(0, 0).child(0, 0));
#endif
    setExpanded(model()->index(0, 0), true);
    qCDebug(appLog) << "DmTreeview::setDefaultdmItem finished, returning index";
    return model()->index(0, 0);
}

void DmTreeview::setRefreshItem(int devicenum, int num) //设置刷新后默认选择操作分区
{
    qCDebug(appLog) << "DmTreeview::setRefreshItem called with devicenum:" << devicenum << "num:" << num;
    if (-1 == devicenum) {
        qCDebug(appLog) << "devicenum is -1, setting current index to item num";
        setCurrentIndex(m_model->item(num)->index());
        setExpanded(m_model->item(num)->index(), true);
    } else {
#if QT_VERSION_MAJOR > 5
        qCDebug(appLog) << "QT_VERSION_MAJOR > 5, setting current index with devicenum and num";
        setCurrentIndex(model()->index(devicenum, 0).model()->index(num, 0));
#else
        setCurrentIndex(model()->index(devicenum, 0).child(num, 0));
#endif
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

int DmTreeview::getCurrentNum() //返回当前选中分区
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

QStandardItem *DmTreeview::getRootItem() //获取根节点
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
