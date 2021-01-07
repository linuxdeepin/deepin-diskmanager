#include "DeviceStorage.h"
#include <QDebug>

namespace DiskManager {


DeviceStorage::DeviceStorage()
    : m_Model(""), m_Vendor(""), m_MediaType(""), m_Size(""), m_RotationRate(""), m_Interface("")
    , m_SerialNumber(""), m_Version(""), m_Capabilities(""), m_Description(""), m_KeyToLshw(""), m_KeyFromStorage("")
{

}

bool DeviceStorage::setHwinfoInfo(const QMap<QString, QString> &mapInfo)
{
    // 龙芯机器中 hwinfo --disk会列出所有的分区信息
    // 存储设备不应包含分区，根据SysFS BusID 来确定是否是分区信息
    if (mapInfo.find("SysFS BusID") == mapInfo.end()) {
        return false;
    }
    setAttribute(mapInfo, "Model", m_Model);
    setAttribute(mapInfo, "Vendor", m_Vendor);

    setAttribute(mapInfo, "Attached to", m_Interface);
    QRegExp re(".*\\((.*)\\).*");
    if (re.exactMatch(m_Interface)) {
        m_Interface = re.cap(1);
        m_Interface.replace("Controller", "");
        m_Interface.replace("controller", "");
    }
    m_Interface = m_Interface.trimmed();
    setAttribute(mapInfo, "Revision", m_Version);
    setAttribute(mapInfo, "Hardware Class", m_Description);
    setAttribute(mapInfo, "Capacity", m_Size);
    // hwinfo里面显示的内容是  14 GB (15376000000 bytes) 需要处理
    m_Size.replace(QRegExp("\\(.*\\)"), "").replace(" ", "");
    if (m_Size.startsWith("0") || m_Size == "") {
        return false;
    }

    setAttribute(mapInfo, "Serial ID", m_SerialNumber);
    ///setDiskSerialID(mapInfo["Device Files"]);
    setAttribute(mapInfo, "SysFS BusID", m_KeyToLshw);
    setAttribute(mapInfo, "Device File", m_DeviceFile);

    return true;
}

bool DeviceStorage::setKLUHwinfoInfo(const QMap<QString, QString> &mapInfo)
{
    // 龙芯机器中 hwinfo --disk会列出所有的分区信息
    // 存储设备不应包含分区，根据SysFS BusID 来确定是否是分区信息
    if (mapInfo.find("SysFS BusID") == mapInfo.end()) {
        return false;
    }
    setAttribute(mapInfo, "Model", m_Model);
    setAttribute(mapInfo, "Vendor", m_Vendor);

    setAttribute(mapInfo, "Attached to", m_Interface);
    QRegExp re(".*\\((.*)\\).*");
    if (re.exactMatch(m_Interface)) {
        m_Interface = re.cap(1);
        m_Interface.replace("Controller", "");
        m_Interface.replace("controller", "");
    }
    m_Interface = m_Interface.trimmed();
    setAttribute(mapInfo, "Revision", m_Version);
    setAttribute(mapInfo, "Hardware Class", m_Description);
    setAttribute(mapInfo, "Capacity", m_Size);
    // hwinfo里面显示的内容是  14 GB (15376000000 bytes) 需要处理
    m_Size.replace(QRegExp("\\(.*\\)"), "").replace(" ", "");
    if (m_Size.startsWith("0") || m_Size == "") {
        return false;
    }


    setAttribute(mapInfo, "Serial ID", m_SerialNumber);
//    setDiskSerialID(mapInfo["Device Files"]);
    setAttribute(mapInfo, "SysFS BusID", m_KeyToLshw);
    setAttribute(mapInfo, "Device File", m_DeviceFile);

    // KLU里面的介质类型的处理方式比较特殊
    if (mapInfo["Driver"].contains("usb-storage")) {
        m_MediaType = "USB";
    }

//    loadOtherDeviceInfo(mapInfo);
    return true;
}


bool DeviceStorage::addInfoFromlshw(const QMap<QString, QString> &mapInfo)
{

    // 先获取需要进行匹配的关键字
    QStringList keys = mapInfo["bus info"].split("@");
    if (keys.size() != 2) {
        return false;
    }
    QString key = keys[1].trimmed();
    key.replace(".", ":");
    if (key != m_KeyToLshw) {
        return false;
    }


    // 获取唯一key
    QStringList words = mapInfo["bus info"].split(":");
    if (words.size() == 2) {
        m_KeyFromStorage = words[0];
        m_KeyFromStorage.replace("@", "");
    }


    // 获取基本信息
    getInfoFromLshw(mapInfo);

    return true;
}

bool DeviceStorage::addInfoFromSmartctl(const QMap<QString, QString> &mapInfo)
{
    // 获取基本信息
    getInfoFromsmartctl(mapInfo);
    return true;
}


bool DeviceStorage::setMediaType(const QString &name, const QString &value)
{
    if (!m_DeviceFile.contains(name)) {
        return false;
    }

    if (QString("0") == value) {
        m_MediaType = "SSD";
    } else if (QString("1") == value) {
        m_MediaType = "HDD";
    } else {
        m_MediaType = "UnKnow";
    }

    return true;
}

bool DeviceStorage::setKLUMediaType(const QString &name, const QString &value)
{
    if (!m_DeviceFile.contains(name)) {
        return false;
    }

    if (m_MediaType == "USB") {
        return true;
    }

    if (QString("0") == value) {
        m_MediaType = "SSD";
    } else if (QString("1") == value) {
        m_MediaType = "HDD";
    } else {
        m_MediaType = "UnKnow";
    }

    return true;
}

void DeviceStorage::getInfoFromLshw(const QMap<QString, QString> &mapInfo)
{
    setAttribute(mapInfo, "capabilities", m_Capabilities);
    setAttribute(mapInfo, "version", m_Version);
    setAttribute(mapInfo, "serial", m_SerialNumber, false);
    setAttribute(mapInfo, "product", m_Model);
    setAttribute(mapInfo, "description", m_Description);
    setAttribute(mapInfo, "size", m_Size);
    // 223GiB (240GB)
    QRegExp re(".*\\((.*)\\)$");
    if (re.exactMatch(m_Size)) {
        m_Size = re.cap(1);
    }
}

void DeviceStorage::getInfoFromsmartctl(const QMap<QString, QString> &mapInfo)
{
    qDebug() << mapInfo << endl;
    // 固件版本
    m_FirmwareVersion = mapInfo["Firmware Version"];

    // 速度
    QString sataVersion = mapInfo["SATA Version"];
    QStringList strList = sataVersion.split(",");
    if (strList.size() == 2) {
        m_Speed = strList[1].trimmed();
    }

    setAttribute(mapInfo, "Rotation Rate", m_RotationRate);

    // 通电时间
    m_PowerOnHours = mapInfo["Power_On_Hours"];
    if (m_PowerOnHours == "") {
        m_PowerOnHours = mapInfo["Power On Hours"];
    }

    // 通电次数
    m_PowerCycleCount = mapInfo["Power_Cycle_Count"];
    if (m_PowerCycleCount == "") {
        m_PowerCycleCount = mapInfo["Power Cycles"];
    }

    // 安装大小
    QString capacity = mapInfo["User Capacity"];
    if (capacity == "") {
        capacity = mapInfo["Total NVM Capacity"];
    }

    if (capacity != "") {
        QRegExp reg(".*\\[(.*)\\]$");
        if (reg.exactMatch(capacity)) {
            m_Size = reg.cap(1);
        }
    }

    // 型号
    //SATA
    if (mapInfo["Device Model"].isEmpty() == false) {
        m_Model = mapInfo["Device Model"];
    }
    //NVME
    if (mapInfo["Model Number"].isEmpty() == false) {
        m_Model = mapInfo["Model Number"];
    }

    setAttribute(mapInfo, "Serial Number", m_SerialNumber, true);
}

void DeviceStorage::setAttribute(const QMap<QString, QString> &mapInfo, const QString &key, QString &variable, bool overwrite)
{
    if (mapInfo.find(key) == mapInfo.end()) {
        return;
    }
    if (mapInfo[key] == "") {
        return;
    }
    if (overwrite) {
        variable = mapInfo[key].trimmed();
    } else {
        if (variable.isEmpty()) {
            variable = mapInfo[key].trimmed();
        }

        if (variable.contains("Unknown", Qt::CaseInsensitive)) {
            variable = mapInfo[key].trimmed();
        }
    }
}

bool DeviceStorage::getDiskInfoFromHwinfo(const QString &devicePath)
{
    QString cmd = QString("hwinfo --disk --only %1").arg(devicePath);
    QProcess proc;
    proc.start(cmd);
    proc.waitForFinished(-1);
    QString outPut = proc.readAllStandardOutput();

    QMap<QString, QString> mapInfo;

    getMapInfoFromInput(outPut, mapInfo);

    setHwinfoInfo(mapInfo);

    return true;
}

void DeviceStorage::getMapInfoFromHwinfo(const QString &info, QMap<QString, QString> &mapInfo, const QString &ch)
{
    QStringList infoList = info.split("\n");

    for (QStringList::iterator it = infoList.begin(); it != infoList.end(); ++it) {
        QStringList words = (*it).split(ch);

        if (words.size() != 2) {
            continue;
        }

        QRegExp re(".*\"(.*)\".*");
        if (re.exactMatch(words[1].trimmed())) {
            QString key = words[0].trimmed();
            QString value = re.cap(1);

            //这里是为了防止  "usb-storage", "sr"  -》 usb-storage", "sr
            if (key == "Driver") {
                value.replace("\"", "");
            }
            mapInfo[key] += value;

        } else {
            if (words[0].trimmed() == "Resolution") {
                mapInfo[words[0].trimmed()] += words[1].trimmed();
            } else {
                mapInfo[words[0].trimmed()] = words[1].trimmed();
            }
        }
    }
}

void DeviceStorage::getMapInfoFromInput(const QString &info, QMap<QString, QString> &mapInfo)
{

    QStringList items = info.split("\n\n");
    foreach (const QString &item, items) {
        if (item.isEmpty()) {
            continue;
        }

        getMapInfoFromHwinfo(item, mapInfo);
     }
}

bool DeviceStorage::getDiskInfoFromLshw(const QString &devicePath)
{
    QProcess proc;
    proc.start("sudo lshw -C disk");
    proc.waitForFinished(-1);
    QString outPut  = proc.readAllStandardOutput();

    QStringList list = outPut.split("*-disk\n");

    outPut.clear();
    for (int i =0;i<list.count();i++) {
        if(list.at(i).contains(devicePath)) {
            outPut = list.at(i);
            break;
        }
    }

    if (outPut.isEmpty()) {
        return false;
    }

    QMap<QString, QString> mapInfo;

    getMapInfoFromLshw(outPut, mapInfo);

    addInfoFromlshw(mapInfo);


    return true;
}

void DeviceStorage::getMapInfoFromLshw(const QString &info, QMap<QString, QString> &mapInfo, const QString &ch)
{
    QStringList infoList = info.split("\n");
    for (QStringList::iterator it = infoList.begin(); it != infoList.end(); ++it) {
        QStringList words = (*it).split(ch);
        if (words.size() != 2) {
            continue;
        }
        // && words[0].contains("configuration") == false && words[0].contains("resources") == false
        // 将configuration的内容进行拆分
        if (words[0].contains("configuration") == true) {
            QStringList keyValues = words[1].split(" ");

            for (QStringList::iterator it = keyValues.begin(); it != keyValues.end(); ++it) {
                QStringList attr = (*it).split("=");
                if (attr.size() != 2) {
                    continue;
                }
                mapInfo.insert(attr[0].trimmed(), attr[1].trimmed());
            }
        } else if (words[0].contains("resources") == true) {
            QStringList keyValues = words[1].split(" ");

            for (QStringList::iterator it = keyValues.begin(); it != keyValues.end(); ++it) {
                QStringList attr = (*it).split(":");
                if (attr.size() != 2) {
                    continue;
                }
                if (mapInfo.find(attr[0].trimmed()) != mapInfo.end()) {
                    mapInfo[attr[0].trimmed()] += QString("  ");
                }
                mapInfo[attr[0].trimmed()] += attr[1].trimmed();
            }
        } else {
            mapInfo.insert(words[0].trimmed(), words[1].trimmed());
        }
    }
}

bool DeviceStorage::getDiskInfoFromLsblk(const QString &devicePath)
{
    QString cmd = QString("lsblk -d -o name,rota %1").arg(devicePath);
    QProcess proc;
    proc.start(cmd);
    proc.waitForFinished(-1);

    QMap<QString, QString> mapInfo;
    QString outPut = proc.readAllStandardOutput();

    loadLsblkInfo(outPut, mapInfo);

    if (mapInfo.size() == 1) {
        setMediaType(mapInfo.firstKey(),mapInfo.value(mapInfo.firstKey()));
    } else {
        return false;
    }

    return true;
}

void DeviceStorage::loadLsblkInfo(const QString &info, QMap<QString, QString> &mapInfo)
{
    QStringList lines = info.split("\n");

    foreach (QString line, lines) {
        QStringList words = line.replace(QRegExp("[\\s]+"), " ").split(" ");
        if (words.size() != 2 || words[0] == "NAME") {
            continue;
        }
        mapInfo.insert(words[0].trimmed(), words[1].trimmed());
    }
}

bool DeviceStorage::getDiskInfoFromSmartCtl(const QString &devicePath)
{
    QString cmd = QString("smartctl --all %1").arg(devicePath);
    QProcess proc;
    proc.start(cmd);
    proc.waitForFinished(-1);
    QString outPut  = proc.readAllStandardOutput();

    QMap<QString, QString> mapInfo;

    getMapInfoFromSmartctl(mapInfo, outPut);

    addInfoFromSmartctl(mapInfo);

    return true;
}

void DeviceStorage::getMapInfoFromSmartctl(QMap<QString, QString> &mapInfo, const QString &info, const QString &ch)
{
    QString indexName;
    int startIndex = 0;

    QRegExp reg("^[\\s\\S]*[\\d]:[\\d][\\s\\S]*$");//time 08:00

    for (int i = 0; i < info.size(); ++i) {
        if (info[i] != '\n' && i != info.size() - 1) {
            continue;
        }

        QString line = info.mid(startIndex, i - startIndex);
        startIndex = i + 1;


        int index = line.indexOf(ch);
        if (index > 0 && reg.exactMatch(line) == false && false == line.contains("Error") && false == line.contains("hh:mm:SS")) {
            if (line.indexOf("(") < index && line.indexOf(")") > index) {
                continue;
            }

            if (line.indexOf("[") < index && line.indexOf("]") > index) {
                continue;
            }

            indexName = line.mid(0, index).trimmed().remove(" is");
            if (mapInfo.contains(indexName)) {
                mapInfo[indexName] += ", ";
                mapInfo[indexName] += line.mid(index + 1).trimmed();
            } else {
                mapInfo[indexName] = line.mid(index + 1).trimmed();
            }
            continue;
        }

        if (indexName.isEmpty() == false && (line.startsWith("\t\t") || line.startsWith("    ")) && line.contains(":") == false) {
            if (mapInfo.contains(indexName)) {
                mapInfo[indexName] += ", ";
                mapInfo[indexName] += line.trimmed();
            } else {
                mapInfo[indexName] = line.trimmed();
            }

            continue;
        }

        indexName = "";

        QRegExp rx("^[ ]*[0-9]+[ ]+([\\w-_]+)[ ]+0x[0-9a-fA-F-]+[ ]+[0-9]+[ ]+[0-9]+[ ]+[0-9]+[ ]+[\\w-]+[ ]+[\\w-]+[ ]+[\\w-]+[ ]+([0-9\\/w-]+[ ]*[ 0-9\\/w-()]*)$");
        if (rx.indexIn(line) > -1) {
            mapInfo[rx.cap(1)] = rx.cap(2);
            continue;
        }

        QStringList strList;

        if (line.endsWith(")")) {
            int leftBracket = line.indexOf("(");
            if (leftBracket > 0) {
                QString str = line.left(leftBracket).trimmed();
                strList = str.trimmed().split(" ");
                if (strList.size() > 2) {
                    strList.last() += line.mid(leftBracket);
                }
            }
        } else if (strList.size() == 0) {
            strList = line.trimmed().split(" ");
        }

        if (strList.size() < 5) {
            continue;
        }

        if (line.contains("Power_On_Hours")) {
            mapInfo["Power_On_Hours"] = strList.last();
            continue;
        }

        if (line.contains("Power_Cycle_Count")) {
            mapInfo["Power_Cycle_Count"] = strList.last();
            continue;
        }

        if (line.contains("Raw_Read_Error_Rate")) {
            mapInfo["Raw_Read_Error_Rate"] = strList.last();
            continue;
        }

        if (line.contains("Spin_Up_Time")) {
            mapInfo["Spin_Up_Time"] = strList.last();
            continue;
        }

        if (line.contains("Start_Stop_Count")) {
            mapInfo["Start_Stop_Count"] = strList.last();
            continue;
        }

        if (line.contains("Reallocated_Sector_Ct")) {
            mapInfo["Reallocated_Sector_Ct"] = strList.last();
            continue;
        }

        if (line.contains("Seek_Error_Rate")) {
            mapInfo["Seek_Error_Rate"] = strList.last();
            continue;
        }

        if (line.contains("Spin_Retry_Count")) {
            mapInfo["Spin_Retry_Count"] = strList.last();
            continue;
        }
        if (line.contains("Calibration_Retry_Count")) {
            mapInfo["Calibration_Retry_Count"] = strList.last();
            continue;
        }
        if (line.contains("G-Sense_Error_Rate")) {
            mapInfo["G-Sense_Error_Rate"] = strList.last();
            continue;
        }
        if (line.contains("Power-Off_Retract_Count")) {
            mapInfo["Power-Off_Retract_Count"] = strList.last();
            continue;
        }
        if (line.contains("Load_Cycle_Count")) {
            mapInfo["Load_Cycle_Count"] = strList.last();
            continue;
        }
        if (line.contains("Temperature_Celsius")) {
            mapInfo["Temperature_Celsius"] = strList.last();
            continue;
        }
        if (line.contains("Reallocated_Event_Count")) {
            mapInfo["Reallocated_Event_Count"] = strList.last();
            continue;
        }
        if (line.contains("Current_Pending_Sector")) {
            mapInfo["Current_Pending_Sector"] = strList.last();
            continue;
        }
        if (line.contains("Offline_Uncorrectable")) {
            mapInfo["Offline_Uncorrectable"] = strList.last();
            continue;
        }
        if (line.contains("UDMA_CRC_Error_Count")) {
            mapInfo["UDMA_CRC_Error_Count"] = strList.last();
            continue;
        }
        if (line.contains("Multi_Zone_Error_Rate")) {
            mapInfo["Multi_Zone_Error_Rate"] = strList.last();
            continue;
        }
    }
}

}
