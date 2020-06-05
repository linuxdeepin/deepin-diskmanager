#ifndef DEVICEINFO_H
#define DEVICEINFO_H
#include "utils.h"
#include "partitioninfo.h"
#include <QVector>

//dubs使用自定义数据结构中的成员变量建议初始化，在测试中发现数据结构中包含的bool类型变量未赋值，该数据结构作为槽函数返回值通过dbus调用可能导致应用崩溃退出
class QDBusArgument;

typedef struct CUSSTRUCTTEST {
    Sector length;
    Sector heads;
    QString m_path;
} stCustest;
Q_DECLARE_METATYPE(stCustest)
QDBusArgument &operator<<(QDBusArgument &argument, const stCustest &stcus);
const QDBusArgument &operator>>(const QDBusArgument &argument, stCustest &stcus);

class DeviceInfo
{
public:
    DeviceInfo();

public:
    Sector length;
    Sector heads;
    QString m_path;
    Sector sectors;
    Sector cylinders;
    Sector cylsize;
    QString model;
    QString serial_number;
    QString disktype;
    int sector_size;
    int max_prims;
    int highest_busy;
    bool readonly;
    int max_partition_name_length;
    PartitionVec partition;
};
Q_DECLARE_METATYPE(DeviceInfo)

QDBusArgument &operator<<(QDBusArgument &argument, const DeviceInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, DeviceInfo &info);

typedef QMap<QString, DeviceInfo> DeviceInfoMap;
Q_DECLARE_METATYPE(DeviceInfoMap)

#endif // DEVICEINFO_H
