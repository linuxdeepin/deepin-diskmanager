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
    void Quit();
    void getDeviceinfo();
    const DeviceInfoMap &probDeviceInfo()const;
    QVector<PartitionInfo> getCurDevicePartitionArr();
    const PartitionInfo &getCurPartititonInfo();
    const DeviceInfo &getCurDeviceInfo();
    void mount(const QString &mountpath);
    void unmount();
    QStringList getallsupportfs();
    bool format(const QString &fstype, const QString &name = QString());
    void resize(const PartitionInfo &info);
    void create(const QVector<PartitionInfo> &infovec);

private:
    explicit DMDbusHandler(QObject *parent = nullptr);
    void initConnection();

signals:
    void sigShowSpinerWindow(bool);
    void sigUpdateDeviceInfo();
    void sigCurSelectChanged();

public slots:
    void slotsetCurSelect(const QString &devicepath, const QString &partitionpath, Sector start, Sector end);

private slots:
    void MessageReport(const QString &msg);
    void slotUpdateDeviceInfo(const DeviceInfoMap &infomap);

private:
    DMDBusInterface        *m_dbus = nullptr;
    static DMDbusHandler    *m_statichandeler;
    QString                 m_curdevicepath;
    QString                 m_curpartitionpath;
    DeviceInfoMap           m_devicemap;
    PartitionInfo           m_curpartitioninfo;
    QStringList             m_supportfs;
};

#endif // DMDBUSHANDLER_H
