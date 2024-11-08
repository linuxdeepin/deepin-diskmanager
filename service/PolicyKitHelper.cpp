// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

//
// Created by linux on 5/23/17.
//

#include "PolicyKitHelper.h"
#include <QDebug>
bool PolicyKitHelper::checkAuthorization(const QString& actionId, qint64 applicationPid)
{
    Authority::Result result;
    // 第一个参数是需要验证的action，和规则文件写的保持一致
    result = Authority::instance()->checkAuthorizationSync(actionId, UnixProcessSubject(applicationPid),
                                                           Authority::AllowUserInteraction);
    if (result == Authority::Yes) {
//        qDebug() << 111111111;
        return true;
    }else {
//        qDebug() << 22222222;
        return false;
    }
}

PolicyKitHelper::PolicyKitHelper()
{

}

PolicyKitHelper::~PolicyKitHelper()
{

}
