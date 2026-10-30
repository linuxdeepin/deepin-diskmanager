// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by linux on 5/23/17.
//

#include "ddlog.h"
#include "PolicyKitHelper.h"
#include <QDebug>

bool PolicyKitHelper::checkAuthorization(const QString& actionId, const QString& appBusName)
{
    if (appBusName.isEmpty()) {
        qCWarning(serviceLog) << "Empty appBusName provided for authorization check";
        return false;
    }

    Authority::Result result;
    result = Authority::instance()->checkAuthorizationSync(actionId, SystemBusNameSubject(appBusName),
                                                           Authority::AllowUserInteraction);
    if (result == Authority::Yes) {
        qCDebug(serviceLog) << "Authorization granted for action:" << actionId;
        return true;
    } else {
        qCWarning(serviceLog) << "Authorization denied for action:" << actionId;
        return false;
    }
}

PolicyKitHelper::PolicyKitHelper()
{
    qCDebug(serviceLog) << "PolicyKitHelper instance created";

}

PolicyKitHelper::~PolicyKitHelper()
{
    qCDebug(serviceLog) << "PolicyKitHelper instance destroyed";

}
