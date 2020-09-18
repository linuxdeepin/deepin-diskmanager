/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmtreeview.h
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
#ifndef DMTREEVIEW_H
#define DMTREEVIEW_H

#include "dmtreeviewdelegate.h"
#include "dmdiskinfobox.h"
#include "partitionwidget.h"

#include <DTreeView>

#include <QStandardItemModel>
#include <QWidget>
#include <QModelIndex>
#include <QMouseEvent>
#include <QSortFilterProxyModel>

DWIDGET_USE_NAMESPACE

/**
 * @class DmTreeview
 * @brief 树结构类
*/

class DmTreeview : public DTreeView
{
    Q_OBJECT
public:
    explicit DmTreeview(QWidget *parent = nullptr);

    /**
     * @brief 初始化界面
    */
    void initUI();

    /**
     * @brief 初始化模型
    */
    void initModel();

    /**
     * @brief 初始化Delegate
    */
    void initDelegate();

    /**
     * @brief 排序
    */
    void sort();

    /**
     * @brief 添加子节点
     * @param item 节点指针
     * @param data 节点数据
     * @param flag 是否展开根节点标志
    */
    void addItem(QStandardItem *item, DiskInfoData &data, int flag);

    /**
     * @brief 获取指向与给定索引关联的QStandardItem的指针
     * @param index 索引值
     * @return 返回QStandardItem指针
    */
    QStandardItem *getModelByIndex(const QModelIndex &index);

    /**
     * @brief 添加根节点
     * @param data 节点数据
     * @return 返回QStandardItem指针
    */
    QStandardItem *addTopItem(DiskInfoData &data);

    /**
     * @brief 获取当前节点
     * @return 返回QStandardItem指针
    */
    QStandardItem *getCurItem();

    /**
     * @brief 获取根节点
     * @return 返回QStandardItem指针
    */
    QStandardItem *getRootItem();

    /**
     * @brief 获取根节点索引值
     * @return 返回索引值
    */
    QModelIndex getRootItemIndex();

    /**
     * @brief 设置默认选中节点
     * @return 返回节点索引值
    */
    QModelIndex setDefaultdmItem();

    /**
     * @brief 设置刷新后默认选择操作分区
     * @param devicenum 设备树索引
     * @param num 设备树子节点索引
    */
    void setRefreshItem(int devicenum, int num);

    /**
     * @brief 获取当前选中分区索引值
     * @return 当前选中分区索引值
    */
    int currentNum();

    /**
     * @brief 获取当前选中分区的父节点索引值
     * @return 当前选中分区父节点索引值
    */
    int currentTopNum();

    /**
     * @brief 添加子节点
     * @param infoBox 节点数据
     * @param flag 是否展开根节点标志
     * @param purItem 节点指针
    */
    void addItem(DmDiskinfoBox *infoBox, int flag, QStandardItem *purItem = nullptr);

    /**
     * @brief 添加根节点
     * @param mailbox 节点数据
     * @param flag 是否展开根节点标志
    */
    void addTopItem(DmDiskinfoBox *mailBox, int flag);

    /**
     * @brief 添加节点
     * @param mailbox 节点数据
     * @param curItem 节点指针
     * @param flag 是否展开根节点标志
    */
    void addSubItem(DmDiskinfoBox *mailBox, QStandardItem *curItem, int flag);

    QStandardItemModel *m_model {nullptr};

signals:
    void selectItem(const QModelIndex &index);
    void curSelectChanged(const QString &devicepath, const QString &partitionpath, Sector start, Sector end);
    //    void sigSendInfo(const QString &devicepath, const QString &disksize, const QString &partitionpath, const QString &partsize, const QString &style, Sector start, Sector end);

public slots:

private:
    QAbstractItemDelegate *m_delegate {nullptr};
    QSortFilterProxyModel *m_sortViewFilter {nullptr};
    QString diskSize;
    int curNum = 0;
    int diskNum = 0;

protected:

    /**
     * @brief 重写当前索引改变函数
    */
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

    /**
     * @brief 重写点击事件
    */
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DMTREEVIEW_H
