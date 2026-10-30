// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "cusapplication.h"
#include <QDebug>

CusApplication::CusApplication(int &argc, char **argv)
    : DApplication(argc, argv)
{
    qCDebug(appLog) << "CusApplication initialized with argc:" << argc;
}

void CusApplication::handleQuitAction()
{
    qCDebug(appLog) << "Handling quit action";
    emit handleQuitActionChanged();
    DApplication::handleQuitAction();
    qCDebug(appLog) << "Quit action handled";
}
