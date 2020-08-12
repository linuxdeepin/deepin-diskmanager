#ifndef DMDBUSHANDLER_H
#define DMDBUSHANDLER_H

#include "dmdbusinterface.h"
#include <QObject>
#include <QDebug>
#include <QDBusConnection>

class DMDbusHandler : public QObject
{
    Q_OBJECT
public:
    static DMDbusHandler *instance(QObject *parent = nullptr);
    ~DMDbusHandler();
    void Quit();
    void getDeviceinfo();
    const DeviceInfoMap &probDeviceInfo() const;
    PartitionVec getCurDevicePartitionArr();
    const PartitionInfo &getCurPartititonInfo();
    const DeviceInfo &getCurDeviceInfo();
    void mount(const QString &mountpath);
    void unmount();
    QStringList getallsupportfs();
    void format(const QString &fstype, const QString &name = QString());
    void resize(const PartitionInfo &info);
    void create(const PartitionVec &infovec);
    HardDiskInfo getHardDiskInfo(const QString &devicePath);
    QString getDeviceHardStatus(const QString &devicePath);
    HardDiskStatusInfoList getDeviceHardStatusInfo(const QString &devicePath);
    void deletePartition(const QString &devicePath, const QString &parttitionPath);
    void hidePartition(const QString &devicePath, const QString &parttitionPath);
    void unhidePartition(const QString &devicePath, const QString &parttitionPath);
    int getPartitionHiddenFlag(const QString &devicePath, const QString &parttitionPath);
    bool detectionPartitionTableError(const QString &devicePath);

private:
    explicit DMDbusHandler(QObject *parent = nullptr);
    void initConnection();

signals:
    void sigShowSpinerWindow(bool);
    void sigUpdateDeviceInfo();
    void sigCurSelectChanged();
    void sigDeletePartition(const QString &deleteMessage);
    void sigHidePartition(const QString &hideMessage);
    void sigShowPartition(const QString &showMessage);

public slots:
    void slotsetCurSelect(const QString &devicepath, const QString &partitionpath, Sector start, Sector end);

private slots:
    void MessageReport(const QString &msg);
    void slotUpdateDeviceInfo(const DeviceInfoMap &infomap);
    void slotDeletePartition(const QString &deleteMessage);
    void slotHidePartition(const QString &hideMessage);
    void slotShowPartition(const QString &showMessage);

private:
    DMDBusInterface *m_dbus = nullptr;
    static DMDbusHandler *m_statichandeler;
    QString m_curdevicepath;
    QString m_curpartitionpath;
    DeviceInfoMap m_devicemap;
    PartitionInfo m_curpartitioninfo;
    QStringList m_supportfs;
    HardDiskInfo m_hardDiskInfo;
    QString m_deviceHardStatus;
    HardDiskStatusInfoList m_hardDiskStatusInfoList;
    bool m_deleteResult;
    bool m_hideResult;
    bool m_unhideResult;
    int m_partitionHiddenFlag;
    bool m_partitionTableError;
};

#endif // DMDBUSHANDLER_H
