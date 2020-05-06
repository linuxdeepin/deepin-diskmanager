#ifndef DISKMANAGERSERVICE_H
#define DISKMANAGERSERVICE_H
#include "diskoperation/partedcore.h"
#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>

class DiskManagerServicePrivate;

class DiskManagerService : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.diskmanager")
public:
    explicit DiskManagerService(QObject *parent = nullptr);

Q_SIGNALS:
    Q_SCRIPTABLE void MessageReport(const QString &msg);
    Q_SCRIPTABLE void AccessDeviceInfoOver(const DeviceInfo &info);
public Q_SLOTS:
    Q_SCRIPTABLE void Quit();
    Q_SCRIPTABLE void Start();
    Q_SCRIPTABLE DeviceInfo getDeviceinfo();
    Q_SCRIPTABLE DeviceInfoMap getalldevice();
    Q_SCRIPTABLE stCustest interfacetest();


private:
    DiskManagerServicePrivate *d_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(d_ptr), DiskManagerService)
};

#endif // DISKMANAGERSERVICE_H
