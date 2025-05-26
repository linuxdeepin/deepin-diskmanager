// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "cusapplication.h"
#include <QDebug>

CusApplication::CusApplication(int &argc, char **argv)
    : DApplication(argc, argv)
{
    qDebug() << "CusApplication initialized with argc:" << argc;
}

void CusApplication::handleQuitAction()
{
    qDebug() << "Handling quit action";
    emit handleQuitActionChanged();
    DApplication::handleQuitAction();
    qDebug() << "Quit action handled";
}
