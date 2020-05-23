#ifndef DISKMANAGERSERVICE_H
#define DISKMANAGERSERVICE_H
#include "diskoperation/partedcore.h"
#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>

namespace DiskManager {

class DiskManagerService : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.diskmanager")
public:
    explicit DiskManagerService(QObject *parent = nullptr);

Q_SIGNALS:
    Q_SCRIPTABLE void MessageReport(const QString &msg);
    Q_SCRIPTABLE void sigUpdateDeviceInfo(const DeviceInfoMap &infomap);
public Q_SLOTS:
    Q_SCRIPTABLE void Quit();
    Q_SCRIPTABLE void Start();
    Q_SCRIPTABLE DeviceInfo getDeviceinfo();
    Q_SCRIPTABLE void getalldevice();
    Q_SCRIPTABLE void setCurSelect(const PartitionInfo &info);
    Q_SCRIPTABLE bool unmount();
    Q_SCRIPTABLE bool mount(const QString &mountpath);
    Q_SCRIPTABLE QStringList getallsupportfs();
    Q_SCRIPTABLE bool format(const QString &fstype, const QString &name = QString());
    Q_SCRIPTABLE bool resize(const PartitionInfo &info);
    Q_SCRIPTABLE stCustest interfacetest();

private:
    void initConnection();
private:
    PartedCore *m_partedcore;

};

}
#endif // DISKMANAGERSERVICE_H
