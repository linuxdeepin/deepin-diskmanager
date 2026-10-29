// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "dmdbusinterface.h"
#include <QDebug>

/*
 * Implementation of interface class DMDBusInterface
 */

DMDBusInterface::DMDBusInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
    qCDebug(appLog) << "DMDBusInterface::DMDBusInterface constructor called. Service:" << service << ", Path:" << path;
}

DMDBusInterface::~DMDBusInterface()
{
    qCDebug(appLog) << "DMDBusInterface::~DMDBusInterface destructor called.";
}
