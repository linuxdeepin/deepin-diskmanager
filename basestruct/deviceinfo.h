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

//new by li 2020/7/20

typedef struct HARDDISKINFO {
    QString m_deviceModel;          //设备模型
    QString m_serialNumber;         //序列号
    QString m_deviceId;             //设备Id
    QString m_firmwareVersion;      //固件版本
    QString m_userCapacity;         //用户容量
    QString m_sectorSize;           //扇区大小
    QString m_rotationRate;         //转速
    QString m_formFactor;           //外形系数
    QString m_deviceis;             //设备
    QString m_ataVersionIs;         //ATA版本为
    QString m_sataVersionIs;        //SATA版本为
    QString m_localTime;            //当地时间
    QString m_smartSupport;         //智能支持SMART
    QString m_smartSupportOn_Off;   //智能支持SMART是否开启
    QString m_Model;              //<! 【型号】1
    QString m_Vendor;             //<! 【制造商】2 //有可能会没有
    QString m_MediaType;          //<! 【介质类型】3
    QString m_Size;               //<! 【大小】4
    QString m_RotationRate;       //<! 【转速】
    QString m_Interface;          //<! 【接口】6
    QString m_SerialNumber;       //<! 【序列号】7
    QString m_Version;            //<! 【版本】
    QString m_Capabilities;       //<! 【功能】
    QString m_Description;        //<! 【描述】
    QString m_PowerOnHours;       //<! 【通电时间】9
    QString m_PowerCycleCount;    //<! 【通电次数】10
    QString m_FirmwareVersion;    //<! 【固件版本】8
    QString m_Speed;              //<! 【速度】5
} HardDiskInfo;
Q_DECLARE_METATYPE(HardDiskInfo)
QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskInfo &inhdinfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskInfo &inhdinfo);

typedef struct HARDDISKSTATUSINFO {
    QString m_id;                   //ID
    QString m_attributeName;        //硬盘制造商定义的属性名
    QString m_flag;                 //属性操作标志
    QString m_value;                //当前值
    QString m_worst;                //最差值
    QString m_thresh;               //临界值
    QString m_type;                 //属性的类型（Pre-fail或Oldage）
    QString m_updated;              //表示属性的更新频率
    QString m_whenFailed;           //如果VALUE小于等于THRESH，会被设置成“FAILING_NOW”；如果WORST小于等于THRESH会被设置成“In_the_past”；如果都不是，会被设置成“-”。在“FAILING_NOW”情况下，需要尽快备份重要 文件，特别是属性是Pre-fail类型时。“In_the_past”代表属性已经故障了，但在运行测试的时候没问题。“-”代表这个属性从没故障过。
    QString m_rawValue;             //原始值
    //int m_flag;                   //标记位
} HardDiskStatusInfo;
Q_DECLARE_METATYPE(HardDiskStatusInfo)
QDBusArgument &operator<<(QDBusArgument &argument, const HardDiskStatusInfo &inhdinfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, HardDiskStatusInfo &inhdinfo);

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

typedef QList<HardDiskStatusInfo> HardDiskStatusInfoList;
Q_DECLARE_METATYPE(HardDiskStatusInfoList)

#endif // DEVICEINFO_H
