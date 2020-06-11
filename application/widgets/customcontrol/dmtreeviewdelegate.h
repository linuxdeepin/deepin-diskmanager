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
#ifndef DMTREEVIEWDELEGATE_H
#define DMTREEVIEWDELEGATE_H
#include "commondef.h"
#include <DApplicationHelper>
#include <DStyledItemDelegate>
#include <DPalette>
#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QAbstractItemView>
#include <QImage>

struct DiskInfoData {
    QString diskpath;
    QString disksize;
    QImage iconImage;
    QString partitonpath;
    QString partitionsize;
    QString used;
    QString unused;
    QString fstype;
    QString syslabel;
    QString mountpoints;
    Sector sectors_unallocated;
    Sector start;
    Sector end;

    int level;
};
Q_DECLARE_METATYPE(DiskInfoData)
DWIDGET_USE_NAMESPACE
class DmTreeviewDelegate : public DStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DmTreeviewDelegate(QAbstractItemView *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:

public slots:
    void slothandleChangeTheme();

private:
    QAbstractItemView *m_parentView {nullptr};
    DPalette m_parentPb;
};

#endif // DMTREEVIEWDELEGATE_H
