#include "dmdbushandler.h"
#include "dmdbusinterface.h"
#include <QObject>
#include <QDBusError>

DMDbusHandler *DMDbusHandler::m_statichandeler = nullptr;

DMDbusHandler *DMDbusHandler::instance(QObject *parent)
{
    if (parent != nullptr && m_statichandeler == nullptr)
        m_statichandeler = new DMDbusHandler(parent);
    return m_statichandeler;
}

DMDbusHandler::DMDbusHandler(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<DeviceInfo>("DeviceInfo");
    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<stCustest>();
    m_dbus = new DMDBusInterface("com.deepin.diskmanager",
                                 "/com/deepin/diskmanager",
                                 QDBusConnection::systemBus(),
                                 this);
//Note: when dealing with remote objects, it is not always possible to determine if it exists when creating a QDBusInterface.
    if (!m_dbus->isValid() && !m_dbus->lastError().message().isEmpty()) {
        qDebug() << "m_dbus isValid false error:" << m_dbus->lastError() << m_dbus->lastError().message();
    }
    qDebug() << "m_dbus isValid true";

    connect(m_dbus, &DMDBusInterface::MessageReport, this, &DMDbusHandler::MessageReport);

}

void DMDbusHandler::Quit()
{
    m_dbus->Quit();
}

void DMDbusHandler::Start()
{
    m_dbus->Start();
}

void DMDbusHandler::getDeviceinfo()
{
    qDebug() << __FUNCTION__ << "-------";
    QDBusPendingReply<DeviceInfoMap> reply = m_dbus->getalldevice();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << reply.error().message();
    } else {
        DeviceInfoMap map = reply.value();
        for (auto it = map.begin(); it != map.end(); it++) {
            DeviceInfo info = *it;
            qDebug() << info.m_path << info.heads << info.cylinders << info.serial_number << info.max_prims;
        }
    }


//    qDebug() << __FUNCTION__ << "-------";
//    QDBusPendingReply<DeviceInfo> reply = m_dbus->getDeviceinfo();
//    reply.waitForFinished();
//    if (reply.isError()) {
//        qDebug() << reply.error().message();
//    } else {
//        DeviceInfo info = reply.value();
//        qDebug() << info.m_path << info.heads << info.cylinders << info.serial_number << info.max_prims;
//    }
}

void DMDbusHandler::MessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}


