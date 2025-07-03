// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "dmdbusinterface.h"
#include <QDebug>

/*
 * Implementation of interface class DMDBusInterface
 */

DMDBusInterface::DMDBusInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    qDebug() << "DMDBusInterface::DMDBusInterface constructor called. Service:" << service << ", Path:" << path;
}

DMDBusInterface::~DMDBusInterface()
{
    qDebug() << "DMDBusInterface::~DMDBusInterface destructor called.";
}
