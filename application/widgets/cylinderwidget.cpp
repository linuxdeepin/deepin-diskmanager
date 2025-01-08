// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "cylinderwidget.h"

#include <QDebug>
#include <QMouseEvent>

CylinderWidget::CylinderWidget(QWidget *parent) : QLabel(parent)
{

}

void CylinderWidget::setUserData(const QVariant &userData)
{
    m_userData = userData;
}

QVariant CylinderWidget::getUserData()
{
    return m_userData;
}

#if QT_VERSION_MAJOR > 5
void CylinderWidget::enterEvent(QEnterEvent *event)
#else
void CylinderWidget::enterEvent(QEvent *event)
#endif
{
    emit enter();

    return QLabel::enterEvent(event);
}

void CylinderWidget::leaveEvent(QEvent *event)
{
    emit leave();

    return QLabel::leaveEvent(event);
}


