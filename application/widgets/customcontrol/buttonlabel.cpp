// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "buttonlabel.h"
#include <QDebug>


ButtonLabel::ButtonLabel(QWidget *parent) : QLabel(parent)
{
    qCDebug(appLog) << "ButtonLabel created";
}

void ButtonLabel::mousePressEvent(QMouseEvent *event)
{
    qCDebug(appLog)  << "ButtonLabel clicked";
    Q_UNUSED(event);

    emit clicked();
    qCDebug(appLog)  << "ButtonLabel click signal emitted";
}
