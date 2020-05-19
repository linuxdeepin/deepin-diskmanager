/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     zhangkai <zhangkai@uniontech.com>
* Maintainer: zhangkai <zhangkai@uniontech.com>
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
#ifndef PARTCHARTSHOWING_H
#define PARTCHARTSHOWING_H
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



class PartChartShowing : public QWidget
{
    Q_OBJECT
public:
    explicit PartChartShowing(QWidget *parent = nullptr);
//    PartChartShowing(int used, int trueused, int noused, QWidget *parent = nullptr);
    void getData(const int &flag, const QString &totalsize);
signals:

public slots:


    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    void addPaint(QPainter *painter);
    void remPaint(QPainter *painter);
private:
    int m_flag = 0;
    QString m_totalsize;
    double total = 0.00;
};

#endif // PARTCHARTSHOWING_H
