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
#ifndef SIZEINFOWIDGET_H
#define SIZEINFOWIDGET_H

#include <QWidget>
#include <DWidget>
#include <QRect>
#include <QPainterPath>
#include <QPainter>
#include <QPalette>
#include <DPalette>
#include <QBrush>
#include <DApplicationHelper>
#include <QPointF>
#include <QLine>
DWIDGET_USE_NAMESPACE
class SizeInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SizeInfoWidget(QWidget *parent = nullptr);
    SizeInfoWidget(int used, int trueused, int noused, QWidget *parent = nullptr);
protected:
    virtual void paintEvent(QPaintEvent *event);
signals:

public slots:
private:
    int m_used;
    int m_tureused;
    int m_noused;
//    DPalette m_parentPb;
};

#endif // SIZEINFOWIDGET_H
