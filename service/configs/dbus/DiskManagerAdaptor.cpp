// SPDX-FileCopyrightText: 2024 - 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DiskManagerAdaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class DiskManagerAdaptor
 */

DiskManagerAdaptor::DiskManagerAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DiskManagerAdaptor::~DiskManagerAdaptor()
{
    // destructor
}

QString DiskManagerAdaptor::DeviceList()
{
    // handle method call com.deepin.diskmanager.DeviceList
    QString out0;
    QMetaObject::invokeMethod(parent(), "DeviceList", Q_RETURN_ARG(QString, out0));
    return out0;
}

bool DiskManagerAdaptor::Install(const QString &image, const QString &device, const QString &partition, bool formatDevice)
{
    // handle method call com.deepin.diskmanager.Install
    bool out0;
    QMetaObject::invokeMethod(parent(), "Install", Q_RETURN_ARG(bool, out0), Q_ARG(QString, image), Q_ARG(QString, device), Q_ARG(QString, partition), Q_ARG(bool, formatDevice));
    return out0;
}

void DiskManagerAdaptor::Reboot()
{
    // handle method call com.deepin.diskmanager.Reboot
    QMetaObject::invokeMethod(parent(), "Reboot");
}

void DiskManagerAdaptor::Start()
{
    // handle method call com.deepin.diskmanager.Start
    QMetaObject::invokeMethod(parent(), "Start");
}

void DiskManagerAdaptor::Stop()
{
    // handle method call com.deepin.diskmanager.Stop
    QMetaObject::invokeMethod(parent(), "Stop");
}

