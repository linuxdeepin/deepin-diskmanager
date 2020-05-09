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
#include "partitionwidget.h"

PartitionWidget::PartitionWidget(QWidget *parent) : DDialog(parent)
{
    this->setFixedSize(800, 600);
    mainFrame = new DFrame(this);

    mainFrame->setFrameRounded(false);
    mainFrame->setFrameStyle(DFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    mainLayout->setSpacing(5);
//    mainLayout->setContentsMargins(5, 5, 5, 5);
    topFrame = new DFrame();
    botFrame = new DFrame();
    mainLayout->addWidget(topFrame, 2);
    mainLayout->addWidget(botFrame, 8);


    this->addContent(mainFrame);
}

void PartitionWidget::paintEvent(QPaintEvent *event)
{
    QRectF rectangle1(50.0, 30.0, 15.0, 15.0);
    QPainter painter;
    painter.setPen(QPen(QColor(Qt::gray), 1));
    painter.drawEllipse(rectangle1);
    DDialog::paintEvent(event);
    update();
}
