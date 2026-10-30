// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "cylinderwidget.h"

#include <QDebug>
#include <QMouseEvent>

CylinderWidget::CylinderWidget(QWidget *parent) : QLabel(parent)
{
    qCDebug(appLog)  << "[CylinderWidget] Initializing cylinder widget";
}

void CylinderWidget::setUserData(const QVariant &userData)
{
    qCDebug(appLog) << "[CylinderWidget] Setting user data";
    m_userData = userData;
}

QVariant CylinderWidget::getUserData()
{
    qCDebug(appLog) << "[CylinderWidget] Getting user data";
    return m_userData;
}

#if QT_VERSION_MAJOR > 5
void CylinderWidget::enterEvent(QEnterEvent *event)
#else
void CylinderWidget::enterEvent(QEvent *event)
#endif
{
    // qCDebug(appLog) << "[CylinderWidget] enterEvent triggered.";
    emit enter();

    return QLabel::enterEvent(event);
}

void CylinderWidget::leaveEvent(QEvent *event)
{
    // qCDebug(appLog) << "[CylinderWidget] leaveEvent triggered.";
    emit leave();

    return QLabel::leaveEvent(event);
}


