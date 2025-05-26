// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "buttonlabel.h"
#include <QDebug>


ButtonLabel::ButtonLabel(QWidget *parent) : QLabel(parent)
{
    qDebug() << "ButtonLabel created";
}

void ButtonLabel::mousePressEvent(QMouseEvent *event)
{
    qDebug()  << "ButtonLabel clicked";
    Q_UNUSED(event);

    emit clicked();
    qDebug()  << "ButtonLabel click signal emitted";
}
