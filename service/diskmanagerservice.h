#ifndef DISKMANAGERSERVICE_H
#define DISKMANAGERSERVICE_H
#include "diskoperation/partedcore.h"
#include <QObject>
#include <QDBusContext>
#include <QScopedPointer>

namespace DiskManager {

class DiskManagerService : public QObject
    , protected QDBusContext
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
    Q_SCRIPTABLE bool create(const PartitionVec &infovec);
    Q_SCRIPTABLE stCustest interfacetest();
    /**
     * @brief 获取硬盘硬件信息
     * @param 返回硬盘信息表
     */
    Q_SCRIPTABLE HardDiskInfo onGetDeviceHardInfo(const QString &devicepath);
    //Q_SCRIPTABLE HardDiskInfo onGetDeviceHardInfo();
    /**
     * @brief 获取硬盘健康状态
     * @param 返回硬盘健康状态
     */
    Q_SCRIPTABLE QString onGetDeviceHardStatus(const QString &devicepath);
    //Q_SCRIPTABLE QString onGetDeviceHardStatus();
    /**
     * @brief 获取硬盘健康信息
     * @param 返回硬盘健康信息
     */
    Q_SCRIPTABLE HardDiskStatusInfoList onGetDeviceHardStatusInfo(const QString &devicepath);
    //Q_SCRIPTABLE HardDiskStatusInfoList onGetDeviceHardStatusInfo();
    /**
     * @brief 个人测试使用
     * @param 无
     */
    Q_SCRIPTABLE void test();
private:
    void initConnection();

private:
    PartedCore *m_partedcore;
};

} // namespace DiskManager
#endif // DISKMANAGERSERVICE_H
