#include "dmdbushandler.h"
#include <QObject>
#include <QDBusError>
#include <QDBusPendingCallWatcher>

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
    qRegisterMetaType<DeviceInfoMap>("DeviceInfoMap");

    qDBusRegisterMetaType<DeviceInfo>();
    qDBusRegisterMetaType<DeviceInfoMap>();
    qDBusRegisterMetaType<PartitionInfo>();
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
    initConnection();
    m_dbus->Start();
}

void DMDbusHandler::initConnection()
{
    connect(m_dbus, &DMDBusInterface::MessageReport, this, &DMDbusHandler::MessageReport);
    connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::sigUpdateDeviceInfo);
    connect(m_dbus, &DMDBusInterface::sigUpdateDeviceInfo, this, &DMDbusHandler::slotUpdateDeviceInfo);
}

void DMDbusHandler::Quit()
{
    m_dbus->Quit();
}

void DMDbusHandler::getDeviceinfo()
{
    // emit sigShowSpinerWindow(true);
    m_dbus->getalldevice();
    qDebug() << __FUNCTION__ << "-------";
}

DeviceInfoMap DMDbusHandler::probDeviceInfo() const
{
    return m_devicemap;
}

void DMDbusHandler::MessageReport(const QString &msg)
{
    qDebug() << "MessageReport:" << msg;
}

void DMDbusHandler::slotUpdateDeviceInfo(const DeviceInfoMap &infomap)
{
    m_devicemap = infomap;
    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << info.sector_size;
//        qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
//                 << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
//                 << info.sector_size << info.max_prims << info.highest_busy << info.readonly
//                 << info.max_partition_name_length;
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            qDebug() << __FUNCTION__ /*<< it->device_path << it->partition_number << it->type << it->status << it->alignment << it->fstype << it->uuid
                     << it->name << it->sector_start << it->sector_end*/ << it->sectors_used << it->sectors_unused;
//                     << it->sectors_unallocated << it->significant_threshold << it->free_space_before
//                     << it->sector_size << it->fs_block_size << it->path << it->filesystem_label;
//            qDebug() << it->sector_end << it->sector_start << Utils::sector_to_unit(it->sector_size, it->sector_end - it->sector_start, SIZE_UNIT::UNIT_GIB);
        }
    }
    emit sigUpdateDeviceInfo();
    emit sigShowSpinerWindow(false);
}


