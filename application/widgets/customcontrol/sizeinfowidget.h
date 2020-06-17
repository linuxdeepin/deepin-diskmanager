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
#ifndef SIZEINFOWIDGET_H
#define SIZEINFOWIDGET_H

#include "partitioninfo.h"
#include "utils.h"
#include <DWidget>
#include <DApplicationHelper>
#include <DPalette>
#include <QWidget>
#include <QRect>
#include <QPainterPath>
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include <QPointF>
#include <QLine>
#include <QColor>
#include <QVector>

DWIDGET_USE_NAMESPACE

class SizeInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SizeInfoWidget(QWidget *parent = nullptr);
    SizeInfoWidget(double used, double unused, bool flag, QWidget *parent = nullptr);
    void setData(PartitionInfo info, QVector<QColor> color, QVector<double> size, bool flag);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
signals:

public slots:
    void slothandleChangeTheme();

private:
    double m_used = 0.00;
    double m_noused = 0.00;
    double m_totalsize = 0.00;
    bool m_flag = false;
    QVector<QColor> colorinfo;
    QVector<double> sizeinfo;
    DPalette m_parentPb;
    QString totalsize;
    QString usedsize;
    QString m_partitionpath;
};

#endif // SIZEINFOWIDGET_H
