// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by linux on 5/23/17.
//

#include "PolicyKitHelper.h"
#include <QDebug>

bool PolicyKitHelper::checkAuthorization(const QString& actionId, const QString& appBusName)
{
    if (appBusName.isEmpty()) {
        qWarning() << "Empty appBusName provided for authorization check";
        return false;
    }

    Authority::Result result;
    result = Authority::instance()->checkAuthorizationSync(actionId, SystemBusNameSubject(appBusName),
                                                           Authority::AllowUserInteraction);
    if (result == Authority::Yes) {
        qDebug() << "Authorization granted for action:" << actionId;
        return true;
    } else {
        qWarning() << "Authorization denied for action:" << actionId;
        return false;
    }
}

PolicyKitHelper::PolicyKitHelper()
{
    qDebug() << "PolicyKitHelper instance created";

}

PolicyKitHelper::~PolicyKitHelper()
{
    qDebug() << "PolicyKitHelper instance destroyed";

}
