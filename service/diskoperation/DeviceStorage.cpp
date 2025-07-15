// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "DeviceStorage.h"
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include "utils.h"

namespace DiskManager {


DeviceStorage::DeviceStorage()
    : m_model(""), m_vendor(""), m_mediaType(""), m_size(""), m_rotationRate(""), m_interface("")
    , m_serialNumber(""), m_version(""), m_capabilities(""), m_description(""), m_KeyToLshw(""), m_KeyFromStorage("")
{
    qDebug() << "DeviceStorage object created";
}

bool DeviceStorage::setHwinfoInfo(const QMap<QString, QString> &mapInfo)
{
    qDebug() << "Setting hwinfo info for storage device";
    // 龙芯机器中 hwinfo --disk会列出所有的分区信息
    // 存储设备不应包含分区，根据SysFS BusID 来确定是否是分区信息
    if (mapInfo.find("SysFS BusID") == mapInfo.end()) {
        qWarning() << "No SysFS BusID found in hwinfo info";
        return false;
    }
    qDebug() << "SysFS BusID found, proceeding";
    setAttribute(mapInfo, "Model", m_model);
    setAttribute(mapInfo, "Vendor", m_vendor);

    setAttribute(mapInfo, "Attached to", m_interface);
#if QT_VERSION_MAJOR > 5
    QRegularExpression re(".*\\((.*)\\).*");
    QRegularExpressionMatch match = re.match(m_interface);
    if (match.hasMatch()) {
        qDebug() << "Interface match found";
        m_interface = match.captured(1);
#else
    QRegExp re(".*\\((.*)\\).*");
    if (re.exactMatch(m_interface)) {
        qDebug() << "Interface match found";
        m_interface = re.cap(1);
#endif
        m_interface.replace("Controller", "");
        m_interface.replace("controller", "");
    }
    m_interface = m_interface.trimmed();
    setAttribute(mapInfo, "Revision", m_version);
    setAttribute(mapInfo, "Hardware Class", m_description);
    setAttribute(mapInfo, "Capacity", m_size);
    // hwinfo里面显示的内容是  14 GB (15376000000 bytes) 需要处理
#if QT_VERSION_MAJOR > 5
    m_size.replace(QRegularExpression("\\(.*\\)"), "").replace(" ", "");
#else
    m_size.replace(QRegExp("\\(.*\\)"), "").replace(" ", "");
#endif
    if (m_size.startsWith("0") || m_size == "") {
        qDebug() << "Size is 0 or empty, returning false";
        return false;
    }

    setAttribute(mapInfo, "Serial ID", m_serialNumber);
    ///setDiskSerialID(mapInfo["Device Files"]);
    setAttribute(mapInfo, "SysFS BusID", m_KeyToLshw);
    setAttribute(mapInfo, "Device File", m_DeviceFile);

    qDebug() << "Hwinfo info set successfully for device:" << m_DeviceFile;
    return true;
}

bool DeviceStorage::setKLUHwinfoInfo(const QMap<QString, QString> &mapInfo)
{
    qDebug() << "Entering DeviceStorage::setKLUHwinfoInfo";
    // 龙芯机器中 hwinfo --disk会列出所有的分区信息
    // 存储设备不应包含分区，根据SysFS BusID 来确定是否是分区信息
    if (mapInfo.find("SysFS BusID") == mapInfo.end()) {
        qDebug() << "No SysFS BusID found in KLU hwinfo, returning false";
        return false;
    }
    qDebug() << "SysFS BusID found in KLU hwinfo, proceeding";
    setAttribute(mapInfo, "Model", m_model);
    setAttribute(mapInfo, "Vendor", m_vendor);

    setAttribute(mapInfo, "Attached to", m_interface);
#if QT_VERSION_MAJOR > 5
    QRegularExpression re(".*\\((.*)\\).*");
    QRegularExpressionMatch match = re.match(m_interface);
    if (match.hasMatch()) {
        qDebug() << "KLU Interface match found";
        m_interface = match.captured(1);
#else
    QRegExp re(".*\\((.*)\\).*");
    if (re.exactMatch(m_interface)) {
        qDebug() << "KLU Interface match found";
        m_interface = re.cap(1);
#endif
        m_interface.replace("Controller", "");
        m_interface.replace("controller", "");
    }
    m_interface = m_interface.trimmed();
    setAttribute(mapInfo, "Revision", m_version);
    setAttribute(mapInfo, "Hardware Class", m_description);
    setAttribute(mapInfo, "Capacity", m_size);
    // hwinfo里面显示的内容是  14 GB (15376000000 bytes) 需要处理
#if QT_VERSION_MAJOR > 5
    m_size.replace(QRegularExpression("\\(.*\\)"), "").replace(" ", "");
#else
    m_size.replace(QRegExp("\\(.*\\)"), "").replace(" ", "");
#endif
    if (m_size.startsWith("0") || m_size == "") {
        qDebug() << "KLU size is 0 or empty, returning false";
        return false;
    }


    setAttribute(mapInfo, "Serial ID", m_serialNumber);
//    setDiskSerialID(mapInfo["Device Files"]);
    setAttribute(mapInfo, "SysFS BusID", m_KeyToLshw);
    setAttribute(mapInfo, "Device File", m_DeviceFile);

    // KLU里面的介质类型的处理方式比较特殊
    if (mapInfo["Driver"].contains("usb-storage")) {
        qDebug() << "Driver contains usb-storage, setting media type to USB";
        m_mediaType = "USB";
    }

//    loadOtherDeviceInfo(mapInfo);
    qDebug() << "Exiting DeviceStorage::setKLUHwinfoInfo";
    return true;
}


bool DeviceStorage::addInfoFromlshw(const QMap<QString, QString> &mapInfo)
{
    qDebug() << "Adding lshw info for storage device";
    // 先获取需要进行匹配的关键字
    QStringList keys = mapInfo["bus info"].split("@");
    if (keys.size() != 2) {
        qWarning() << "Invalid bus info format:" << mapInfo["bus info"];
        return false;
    }
    if (keys[0].trimmed() == "nvme") {
        qDebug() << "nvme device found, not check KeyToLshw";
    } else {
        QString key = keys[1].trimmed();
        key.replace(".", ":");
        if (key != m_KeyToLshw) {
            qDebug() << "lshw key does not match, returning false" << keys[0].trimmed() << key << m_KeyToLshw;
            return false;
        }
    }

    // 获取唯一key
    QStringList words = mapInfo["bus info"].split(":");
    if (words.size() == 2) {
        qDebug() << "Setting KeyFromStorage";
        m_KeyFromStorage = words[0];
        m_KeyFromStorage.replace("@", "");
    } else {
        qDebug() << "Cannot set KeyFromStorage, invalid format for bus info";
    }


    // 获取基本信息
    getInfoFromLshw(mapInfo);

    qDebug() << "Lshw info added successfully";
    return true;
}

bool DeviceStorage::addInfoFromSmartctl(const QMap<QString, QString> &mapInfo)
{
    qDebug() << "Entering DeviceStorage::addInfoFromSmartctl";
    // 获取基本信息
    getInfoFromsmartctl(mapInfo);
    qDebug() << "Exiting DeviceStorage::addInfoFromSmartctl";
    return true;
}


bool DeviceStorage::setMediaType(const QString &name, const QString &value)
{
    qDebug() << "Entering DeviceStorage::setMediaType for name:" << name << "value:" << value;
    if (!m_DeviceFile.contains(name)) {
        qDebug() << "Device file does not contain name, returning false";
        return false;
    }

    if (QString("0") == value) {
        qDebug() << "Setting media type to SSD";
        m_mediaType = "SSD";
    } else if (QString("1") == value) {
        qDebug() << "Setting media type to HDD";
        m_mediaType = "HDD";
    } else {
        qDebug() << "Setting media type to UnKnow";
        m_mediaType = "UnKnow";
    }

    return true;
}

bool DeviceStorage::setKLUMediaType(const QString &name, const QString &value)
{
    qDebug() << "Entering DeviceStorage::setKLUMediaType for name:" << name << "value:" << value;
    if (!m_DeviceFile.contains(name)) {
        qDebug() << "Device file does not contain name, returning false";
        return false;
    }

    if (m_mediaType == "USB") {
        qDebug() << "Media type is already USB, returning true";
        return true;
    }

    if (QString("0") == value) {
        qDebug() << "Setting media type to SSD for KLU";
        m_mediaType = "SSD";
    } else if (QString("1") == value) {
        qDebug() << "Setting media type to HDD for KLU";
        m_mediaType = "HDD";
    } else {
        qDebug() << "Setting media type to UnKnow for KLU";
        m_mediaType = "UnKnow";
    }

    return true;
}

void DeviceStorage::getInfoFromLshw(const QMap<QString, QString> &mapInfo)
{
    qDebug() << "Entering DeviceStorage::getInfoFromLshw";
    setAttribute(mapInfo, "capabilities", m_capabilities);
    setAttribute(mapInfo, "version", m_version);
    setAttribute(mapInfo, "serial", m_serialNumber, false);
    setAttribute(mapInfo, "product", m_model);
    setAttribute(mapInfo, "description", m_description);
    setAttribute(mapInfo, "size", m_size);
    // 223GiB (240GB)
#if QT_VERSION_MAJOR > 5
    QRegularExpression re(".*\\((.*)\\)$");
    QRegularExpressionMatch match = re.match(m_size);
    if (match.hasMatch()) {
        qDebug() << "Size match found in lshw info";
        m_size = match.captured(1);
    }
#else
    QRegExp re(".*\\((.*)\\)$");
    if (re.exactMatch(m_size)) {
        qDebug() << "Size match found in lshw info";
        m_size = re.cap(1);
    }
#endif
}

void DeviceStorage::getInfoFromsmartctl(const QMap<QString, QString> &mapInfo)
{
    qDebug() << "Getting smartctl info for storage device";
    // 固件版本
    m_firmwareVersion = mapInfo["Firmware Version"];

    // 速度
    QString sataVersion = mapInfo["SATA Version"];
    QStringList strList = sataVersion.split(",");
    if (strList.size() == 2) {
        qDebug() << "SATA version found, setting speed";
        m_speed = strList[1].trimmed();
    }

    setAttribute(mapInfo, "Rotation Rate", m_rotationRate);

    // 通电时间
    m_powerOnHours = mapInfo["Power_On_Hours"];
    if (m_powerOnHours == "") {
        qDebug() << "Power_On_Hours is empty, trying Power On Hours";
        m_powerOnHours = mapInfo["Power On Hours"];
    }

    // 通电次数
    m_powerCycleCount = mapInfo["Power_Cycle_Count"];
    if (m_powerCycleCount == "") {
        qDebug() << "Power_Cycle_Count is empty, trying Power Cycles";
        m_powerCycleCount = mapInfo["Power Cycles"];
    }

    // 安装大小
    QString capacity = mapInfo["User Capacity"];
    if (capacity == "") {
        capacity = mapInfo["Total NVM Capacity"];
    }

    if (capacity != "") {
#if QT_VERSION_MAJOR > 5
        QRegularExpression reg(".*\\[(.*)\\]$");
        QRegularExpressionMatch match = reg.match(capacity);
        if (match.hasMatch()) {
            m_size = match.captured(1);
        }
#else
        QRegExp reg(".*\\[(.*)\\]$");
        if (reg.exactMatch(capacity)) {
            m_size = reg.cap(1);
        }
#endif
    }

    // 型号
    //SATA
    if (mapInfo["Device Model"].isEmpty() == false) {
        m_model = mapInfo["Device Model"];
    }
    //NVME
    if (mapInfo["Model Number"].isEmpty() == false) {
        m_model = mapInfo["Model Number"];
    }

    if (mapInfo.find("Serial Number") != mapInfo.end())
        setAttribute(mapInfo, "Serial Number", m_serialNumber, true);
    else if (mapInfo.find("Serial number") != mapInfo.end()) {
        // 某些机型smart命令查询结果为Serial number
        setAttribute(mapInfo, "Serial number", m_serialNumber, true);
    }
}

void DeviceStorage::setAttribute(const QMap<QString, QString> &mapInfo, const QString &key, QString &variable, bool overwrite)
{
    // qDebug() << "Setting attribute for key:" << key;
    if (mapInfo.find(key) == mapInfo.end()) {
        qDebug() << "Key not found in map:" << key;
        return;
    }
    if (mapInfo[key] == "") {
        qDebug() << "Empty value for key:" << key;
        return;
    }
    if (overwrite) {
        qDebug() << "Overwrite is true, setting variable to map value";
        variable = mapInfo[key].trimmed();
    } else {
        if (variable.isEmpty()) {
            qDebug() << "Variable is empty, setting variable to map value";
            variable = mapInfo[key].trimmed();
        }

        if (variable.contains("Unknown", Qt::CaseInsensitive)) {
            qDebug() << "Variable contains Unknown, setting variable to map value";
            variable = mapInfo[key].trimmed();
        }
    }
}

bool DeviceStorage::getDiskInfoFromHwinfo(const QString &devicePath)
{
    qDebug() << "Getting disk info from hwinfo for device:" << devicePath;
    QString cmd = QString("hwinfo --disk --only %1").arg(devicePath);
    QString outPut;
    QString error;
    if (Utils::executCmd(cmd, outPut, error) != 0) {
        qDebug() << "Failed to execute hwinfo command, error:" << error;
        return false;
    }

    QMap<QString, QString> mapInfo;

    getMapInfoFromInput(outPut, mapInfo);

    setHwinfoInfo(mapInfo);

    //部分华为设备读取到是Serial ID为乱码
    QString cid = Utils::readContent("/proc/bootdevice/cid");
    if (!cid.isEmpty()) {
        m_serialNumber = cid;
    }
    qDebug() << "Successfully got disk info from hwinfo";
    return true;
}

void DeviceStorage::getMapInfoFromHwinfo(const QString &info, QMap<QString, QString> &mapInfo, const QString &ch)
{
    qDebug() << "DeviceStorage::getMapInfoFromHwinfo BEGIN";
    QStringList infoList = info.split("\n");

    for (QStringList::iterator it = infoList.begin(); it != infoList.end(); ++it) {
        QStringList words = (*it).split(ch);

        if (words.size() != 2) {
            qDebug() << "words.size() != 2, continue";
            continue;
        }

#if QT_VERSION_MAJOR > 5
        QRegularExpression re(".*\"(.*)\".*");
        QRegularExpressionMatch match = re.match(words[1].trimmed());
        if (match.hasMatch()) {
            qDebug() << "match.hasMatch() is true";
            QString key = words[0].trimmed();
            QString value = match.captured(1);
#else
        QRegExp re(".*\"(.*)\".*");
        if (re.exactMatch(words[1].trimmed())) {
            qDebug() << "re.exactMatch is true";
            QString key = words[0].trimmed();
            QString value = re.cap(1);
#endif

            //这里是为了防止  "usb-storage", "sr"  -》 usb-storage", "sr
            if (key == "Driver") {
                qDebug() << "key is Driver";
                value.replace("\"", "");
            }
            mapInfo[key] += value;

        } else {
            qDebug() << "match.hasMatch() is false";
            if (words[0].trimmed() == "Resolution") {
                qDebug() << "words[0] is Resolution";
                mapInfo[words[0].trimmed()] += words[1].trimmed();
            } else {
                qDebug() << "words[0] is not Resolution";
                mapInfo[words[0].trimmed()] = words[1].trimmed();
            }
        }
    }
    qDebug() << "DeviceStorage::getMapInfoFromHwinfo END";
}

void DeviceStorage::getMapInfoFromInput(const QString &info, QMap<QString, QString> &mapInfo)
{
    qDebug() << "DeviceStorage::getMapInfoFromInput BEGIN";
    QStringList items = info.split("\n\n");
    foreach (const QString &item, items) {
        if (item.isEmpty()) {
            qDebug() << "item is empty, continue";
            continue;
        }

        getMapInfoFromHwinfo(item, mapInfo);
     }
    qDebug() << "DeviceStorage::getMapInfoFromInput END";
}

bool DeviceStorage::getDiskInfoFromLshw(const QString &devicePath)
{
    qDebug() << "DeviceStorage::getDiskInfoFromLshw BEGIN";
    QString outPut, error;
    if (Utils::executCmd("lshw -C disk", outPut, error) != 0) {
        qDebug() << "Failed to execute lshw command, error:" << error;
        return false;
    }

    QString diskInfo;
    QStringList list = outPut.split("*-disk");
    foreach (const QString &item, list) {
        QStringList list2 = item.split("*-namespace");
        foreach (const QString &item2, list2) {
            if (item2.contains(devicePath)) {
                qDebug() << "devicePath found in lshw output";
                diskInfo = item2;
                break;
            }
        }
    }
    if (diskInfo.isEmpty()) {
        qDebug() << "devicePath not found in lshw output, return false";
        return false;
    }

    QMap<QString, QString> mapInfo;
    getMapInfoFromLshw(diskInfo, mapInfo);
    addInfoFromlshw(mapInfo);

    qDebug() << "DeviceStorage::getDiskInfoFromLshw END";
    return true;
}

void DeviceStorage::getMapInfoFromLshw(const QString &info, QMap<QString, QString> &mapInfo, const QString &ch)
{
    qDebug() << "DeviceStorage::getMapInfoFromLshw BEGIN";
    QStringList infoList = info.split("\n");
    for (QStringList::iterator it = infoList.begin(); it != infoList.end(); ++it) {
        QStringList words = (*it).split(ch);
        if (words.size() != 2) {
            qDebug() << "words.size() != 2, continue";
            continue;
        }
        // && words[0].contains("configuration") == false && words[0].contains("resources") == false
        // 将configuration的内容进行拆分
        if (words[0].contains("configuration") == true) {
            qDebug() << "words[0] contains configuration";
            QStringList keyValues = words[1].split(" ");

            for (QStringList::iterator it = keyValues.begin(); it != keyValues.end(); ++it) {
                QStringList attr = (*it).split("=");
                if (attr.size() != 2) {
                    qDebug() << "attr.size() != 2, continue";
                    continue;
                }
                mapInfo.insert(attr[0].trimmed(), attr[1].trimmed());
            }
        } else if (words[0].contains("resources") == true) {
            qDebug() << "words[0] contains resources";
            QStringList keyValues = words[1].split(" ");

            for (QStringList::iterator it = keyValues.begin(); it != keyValues.end(); ++it) {
                QStringList attr = (*it).split(":");
                if (attr.size() != 2) {
                    qDebug() << "attr.size() != 2, continue";
                    continue;
                }
                if (mapInfo.find(attr[0].trimmed()) != mapInfo.end()) {
                    qDebug() << "mapInfo contains attr[0].trimmed()";
                    mapInfo[attr[0].trimmed()] += QString("  ");
                }
                mapInfo[attr[0].trimmed()] += attr[1].trimmed();
            }
        } else {
            qDebug() << "words[0] does not contain configuration or resources";
            mapInfo.insert(words[0].trimmed(), words[1].trimmed());
        }
    }
    qDebug() << "DeviceStorage::getMapInfoFromLshw END";
}

bool DeviceStorage::getDiskInfoFromLsblk(const QString &devicePath)
{
    qDebug() << "DeviceStorage::getDiskInfoFromLsblk BEGIN";
    QString cmd = QString("lsblk -d -o name,rota %1").arg(devicePath);
    QString outPut, error;
    if (Utils::executCmd(cmd, outPut, error) != 0) {
        qDebug() << "Failed to execute lsblk command, error:" << error;
        return false;
    }

    QMap<QString, QString> mapInfo;

    loadLsblkInfo(outPut, mapInfo);

    if (mapInfo.size() == 1) {
        qDebug() << "mapInfo.size() is 1";
        setMediaType(mapInfo.firstKey(),mapInfo.value(mapInfo.firstKey()));
    } else {
        qDebug() << "mapInfo.size() is not 1, return false";
        return false;
    }

    qDebug() << "DeviceStorage::getDiskInfoFromLsblk END";
    return true;
}

void DeviceStorage::loadLsblkInfo(const QString &info, QMap<QString, QString> &mapInfo)
{
    qDebug() << "DeviceStorage::loadLsblkInfo BEGIN";
    QStringList lines = info.split("\n");

    foreach (QString line, lines) {
#if QT_VERSION_MAJOR > 5
        QStringList words = line.replace(QRegularExpression("[\\s]+"), " ").split(" ");
#else
        QStringList words = line.replace(QRegExp("[\\s]+"), " ").split(" ");
#endif
        if (words.size() != 2 || words[0] == "NAME") {
            qDebug() << "words.size() != 2 or words[0] is NAME, continue";
            continue;
        }
        mapInfo.insert(words[0].trimmed(), words[1].trimmed());
    }
    qDebug() << "DeviceStorage::loadLsblkInfo END";
}

bool DeviceStorage::getDiskInfoFromSmartCtl(const QString &devicePath)
{
    qDebug() << "DeviceStorage::getDiskInfoFromSmartCtl BEGIN";
    QString cmd = QString("smartctl --all %1").arg(devicePath);
    QString outPut, error;
    int exitcode = Utils::executCmd(cmd, outPut, error);

    if (outPut.contains("Please specify device type with the -d option")) {
        qDebug() << "need to specify device type";
        cmd = QString("smartctl --all -d sat %1").arg(devicePath);
        exitcode = Utils::executCmd(cmd, outPut, error);
    }

    if (exitcode != 0) {
        qDebug() << "Failed to execute smartctl command, error:" << error;
        return false;
    }

    QMap<QString, QString> mapInfo;

    getMapInfoFromSmartctl(mapInfo, outPut);

    addInfoFromSmartctl(mapInfo);

    qDebug() << "DeviceStorage::getDiskInfoFromSmartCtl END";
    return true;
}

void DeviceStorage::getDiskInfoModel(const QString &devicePath, QString &model)
{
    qDebug() << "DeviceStorage::getDiskInfoModel BEGIN";
    QString cmd = QString("smartctl --all %1").arg(devicePath);
    QString outPut, error;
    int exitcode = Utils::executCmd(cmd, outPut, error);

    if (outPut.contains("Please specify device type with the -d option")) {
        qDebug() << "need to specify device type";
        cmd = QString("smartctl --all -d sat %1").arg(devicePath);
        exitcode = Utils::executCmd(cmd, outPut, error);
    }

    if (exitcode != 0) {
        qDebug() << "Failed to execute smartctl command, error:" << error;
        return;
    }

    QStringList infoList = outPut.split("\n");
    for (int i = 0; i < infoList.size(); i++) {
        QString info = infoList[i];
        if(info.startsWith("Device Model:") || info.startsWith("Product:") || info.startsWith("Model Number:")){
            qDebug() << "found model in smartctl output";
            model = info.split(":").last().trimmed();
            return;
        }
    }

    cmd = "lshw -C disk";
    exitcode = Utils::executCmd(cmd, outPut, error);
    if (exitcode != 0) {
        qDebug() << "Failed to execute lshw command, error:" << error;
        return;
    }

    infoList = outPut.split("*-disk\n");
    for (int i =0; i < infoList.size(); i++) {
        if(infoList[i].contains(devicePath)) {
            qDebug() << "found devicePath in lshw output";
            QStringList tempList = infoList[i].split("\n");
            for (int j = 0; j < tempList.size(); j++) {
                if(tempList[j].contains("product:")){
                    qDebug() << "found model in lshw output";
                    model = tempList[j].split(":").last().trimmed();
                    return;
                }
            }
            break;
        }
    }

    cmd = "udevadm info " + devicePath + " | grep ID_MODEL=";
    proc.start("bash", QStringList() << "-c" << cmd);
    proc.waitForFinished(-1);
    outPut = proc.readAllStandardOutput();
    if (!outPut.isEmpty()) {
        auto idModel = outPut.split("=", QString::SkipEmptyParts);
        if (idModel.count() > 1)
            model = idModel.at(1).trimmed();
        if (!model.isEmpty())
            return;
    }

    qWarning() << "tried a lot but got nothing about model for" << devicePath;

    // 若未获取到型号名，返回未知
    qDebug() << "model not found, set to UnKnow";
    model = "UnKnow";
    return;
}

QString DeviceStorage::getDiskInfoMediaType(const QString &devicePath)
{
    qDebug() << "DeviceStorage::getDiskInfoMediaType BEGIN";
    QStringList deviceList = devicePath.split("/");
    if (deviceList.size() < 2) {
        qDebug() << "deviceList.size() < 2, return UnKnow";
        return "UnKnow";
    }
    QString device = deviceList[deviceList.size()-1];
    QString rotational_file = QString("/sys/block/%1/queue/rotational").arg(device);
    QString value = Utils::readContent(rotational_file).trimmed();

    if ("1" == value) {
        qDebug() << "value is 1";
        QString cmd = QString("smartctl -i %1").arg(devicePath);
        QString outPut, error;
        int exitcode = Utils::executCmd(cmd, outPut, error);
        if (outPut.contains("Solid State Device")) {
            qDebug() << "is Solid State Device";
            value = "0";
        } else if (outPut.contains("Please specify device type with the -d option")) {
            qDebug() << "need to specify device type";
            //FIXME: Hard type scsi for USB disk
            cmd = QString("smartctl -i -d scsi %1").arg(devicePath);
            exitcode = Utils::executCmd(cmd, outPut, error);
            if (!outPut.contains("Rotation Rate:")) {
                qDebug() << "not contains Rotation Rate";
                value = "0";
            }
        }
    }
    if (QString("0") == value) {
        qDebug() << "return SSD";
        return  "SSD";
    } else if (QString("1") == value) {
        qDebug() << "return HDD";
        return "HDD";
    } else {
        qDebug() << "return UnKnow";
        return "UnKnow";
    }
}

static bool isPGUX()
{
    static int isPGUX = -1;
    QString output, err;

    if (isPGUX != -1) {
        qDebug() << "isPGUX is not -1, return isPGUX";
        return 1 == isPGUX;
    }

    Utils::executeCmdWithArtList("dmidecode", QStringList() << "-t" << "11", output, err);
    QRegularExpression re("String 4: (.+?)\\s");
    QRegularExpressionMatch match = re.match(output);
    isPGUX = (match.hasMatch() && (match.captured(1) == "PGUX")) ? 1 : 0;

    return 1 == isPGUX;
}

void DeviceStorage::getDiskInfoInterface(const QString &devicePath, QString &interface, QString &model)
{
    qDebug() << "DeviceStorage::getDiskInfoInterface BEGIN";
    QString bootDevicePath("/proc/bootdevice/product_name");
    QFile file(bootDevicePath);

    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "open /proc/bootdevice/product_name success";
        if (model == file.readLine().simplified()) {
            qDebug() << "model is boot device";
            QString spec_version = Utils::readContent("/sys/block/sdd/device/spec_version").trimmed();
            if (!spec_version.isEmpty()) {
                qDebug() << "spec_version is not empty";
                if (spec_version.contains("300")) {
                    interface = "UFS 3.0";
                } else if (spec_version.contains("310")) {
                    interface = "UFS 3.1";
                } else if (spec_version.contains("400")) {
                    interface = "UFS 4.0";
                }
            }
        }
        file.close();
    }

    if (interface.isEmpty()) {
        qDebug() << "interface is empty";
        QString cmd = QString("hwinfo --disk --only %1").arg(devicePath);
        QString outPut, error;
        int exitcode = Utils::executCmd(cmd, outPut, error);
        if (exitcode != 0) {
            interface = "UnKnow";
            qDebug() << "Failed to execute hwinfo command, error:" << error;
            return;
        }
        QStringList outPutList = outPut.split("(");
        interface = outPutList[outPutList.size() - 1].split(" ")[0];
    }
    qDebug() << "DeviceStorage::getDiskInfoInterface END";
    return;
}

void DeviceStorage::updateForHWDevice(const QString &devicePath)
{
    qDebug() << "DeviceStorage::updateForHWDevice BEGIN";
    if (m_model != Utils::readContent("/proc/bootdevice/product_name").trimmed()) {
        qDebug() << "m_model is not boot device, return";
        return;
    }

    // m_firmwareVersion
    if (m_firmwareVersion.isEmpty()) {
        qDebug() << "m_firmwareVersion is empty";
        if (isPGUX()) {
            qDebug() << "is PGUX";
            m_firmwareVersion = Utils::readContent("/proc/bootdevice/rev").trimmed();
        }
    }

    // hide model and vendor
    m_model = "";
    m_vendor = "";
    qDebug() << "DeviceStorage::updateForHWDevice END";
}

void DeviceStorage::getMapInfoFromSmartctl(QMap<QString, QString> &mapInfo, const QString &info, const QString &ch)
{
    qDebug() << "DeviceStorage::getMapInfoFromSmartctl BEGIN";
    QString indexName;
    int startIndex = 0;

#if QT_VERSION_MAJOR > 5
    QRegularExpression reg("^[\\s\\S]*[\\d]:[\\d][\\s\\S]*$");//time 08:00
#else
    QRegExp reg("^[\\s\\S]*[\\d]:[\\d][\\s\\S]*$");//time 08:00
#endif

    for (int i = 0; i < info.size(); ++i) {
        if (info[i] != '\n' && i != info.size() - 1) {
            continue;
        }

        QString line = info.mid(startIndex, i - startIndex);
        startIndex = i + 1;


        int index = line.indexOf(ch);
#if QT_VERSION_MAJOR > 5
        QRegularExpressionMatch match = reg.match(line);
        if (index > 0 && !match.hasMatch() && false == line.contains("Error") && false == line.contains("hh:mm:SS")) {
#else
        if (index > 0 && reg.exactMatch(line) == false && false == line.contains("Error") && false == line.contains("hh:mm:SS")) {
#endif
            if (line.indexOf("(") < index && line.indexOf(")") > index) {
                qDebug() << "bracket contains ch, continue";
                continue;
            }

            if (line.indexOf("[") < index && line.indexOf("]") > index) {
                qDebug() << "square bracket contains ch, continue";
                continue;
            }

            indexName = line.mid(0, index).trimmed().remove(" is");
            if (mapInfo.contains(indexName)) {
                qDebug() << "mapInfo contains indexName";
                mapInfo[indexName] += ", ";
                mapInfo[indexName] += line.mid(index + 1).trimmed();
            } else {
                qDebug() << "mapInfo not contains indexName";
                mapInfo[indexName] = line.mid(index + 1).trimmed();
            }
            continue;
        }

        if (indexName.isEmpty() == false && (line.startsWith("\t\t") || line.startsWith("    ")) && line.contains(":") == false) {
            if (mapInfo.contains(indexName)) {
                qDebug() << "mapInfo contains indexName";
                mapInfo[indexName] += ", ";
                mapInfo[indexName] += line.trimmed();
            } else {
                qDebug() << "mapInfo not contains indexName";
                mapInfo[indexName] = line.trimmed();
            }

            continue;
        }

        indexName = "";
#if QT_VERSION_MAJOR > 5
        QRegularExpression rx("^[ ]*[0-9]+[ ]+([\\w-_]+)[ ]+0x[0-9a-fA-F-]+[ ]+[0-9]+[ ]+[0-9]+[ ]+[0-9]+[ ]+[\\w-]+[ ]+[\\w-]+[ ]+[\\w-]+[ ]+([0-9\\/w-]+[ ]*[ 0-9\\/w-()]*)$");
        QRegularExpressionMatch matchR = rx.match(line);
        if (matchR.hasMatch()) {
            qDebug() << "matchR.hasMatch() is true";
            mapInfo[matchR.captured(1)] = matchR.captured(2);
            continue;
        }
#else
        QRegExp rx("^[ ]*[0-9]+[ ]+([\\w-_]+)[ ]+0x[0-9a-fA-F-]+[ ]+[0-9]+[ ]+[0-9]+[ ]+[0-9]+[ ]+[\\w-]+[ ]+[\\w-]+[ ]+[\\w-]+[ ]+([0-9\\/w-]+[ ]*[ 0-9\\/w-()]*)$");
        if (rx.indexIn(line) > -1) {
            qDebug() << "rx.indexIn(line) > -1";
            mapInfo[rx.cap(1)] = rx.cap(2);
            continue;
        }
#endif

        QStringList strList;

        if (line.endsWith(")")) {
            qDebug() << "line endsWith )";
            int leftBracket = line.indexOf("(");
            if (leftBracket > 0) {
                qDebug() << "leftBracket > 0";
                QString str = line.left(leftBracket).trimmed();
                strList = str.trimmed().split(" ");
                if (strList.size() > 2) {
                    qDebug() << "strList.size() > 2";
                    strList.last() += line.mid(leftBracket);
                }
            }
        } else if (strList.size() == 0) {
            qDebug() << "strList.size() is 0";
            strList = line.trimmed().split(" ");
        }

        if (strList.size() < 5) {
            qDebug() << "strList.size() < 5, continue";
            continue;
        }

        if (line.contains("Power_On_Hours")) {
            qDebug() << "line contains Power_On_Hours";
            mapInfo["Power_On_Hours"] = strList.last();
            continue;
        }

        if (line.contains("Power_Cycle_Count")) {
            qDebug() << "line contains Power_Cycle_Count";
            mapInfo["Power_Cycle_Count"] = strList.last();
            continue;
        }

        if (line.contains("Raw_Read_Error_Rate")) {
            qDebug() << "line contains Raw_Read_Error_Rate";
            mapInfo["Raw_Read_Error_Rate"] = strList.last();
            continue;
        }

        if (line.contains("Spin_Up_Time")) {
            qDebug() << "line contains Spin_Up_Time";
            mapInfo["Spin_Up_Time"] = strList.last();
            continue;
        }

        if (line.contains("Start_Stop_Count")) {
            qDebug() << "line contains Start_Stop_Count";
            mapInfo["Start_Stop_Count"] = strList.last();
            continue;
        }

        if (line.contains("Reallocated_Sector_Ct")) {
            qDebug() << "line contains Reallocated_Sector_Ct";
            mapInfo["Reallocated_Sector_Ct"] = strList.last();
            continue;
        }

        if (line.contains("Seek_Error_Rate")) {
            qDebug() << "line contains Seek_Error_Rate";
            mapInfo["Seek_Error_Rate"] = strList.last();
            continue;
        }

        if (line.contains("Spin_Retry_Count")) {
            qDebug() << "line contains Spin_Retry_Count";
            mapInfo["Spin_Retry_Count"] = strList.last();
            continue;
        }
        if (line.contains("Calibration_Retry_Count")) {
            qDebug() << "line contains Calibration_Retry_Count";
            mapInfo["Calibration_Retry_Count"] = strList.last();
            continue;
        }
        if (line.contains("G-Sense_Error_Rate")) {
            qDebug() << "line contains G-Sense_Error_Rate";
            mapInfo["G-Sense_Error_Rate"] = strList.last();
            continue;
        }
        if (line.contains("Power-Off_Retract_Count")) {
            qDebug() << "line contains Power-Off_Retract_Count";
            mapInfo["Power-Off_Retract_Count"] = strList.last();
            continue;
        }
        if (line.contains("Load_Cycle_Count")) {
            qDebug() << "line contains Load_Cycle_Count";
            mapInfo["Load_Cycle_Count"] = strList.last();
            continue;
        }
        if (line.contains("Temperature_Celsius")) {
            qDebug() << "line contains Temperature_Celsius";
            mapInfo["Temperature_Celsius"] = strList.last();
            continue;
        }
        if (line.contains("Reallocated_Event_Count")) {
            qDebug() << "line contains Reallocated_Event_Count";
            mapInfo["Reallocated_Event_Count"] = strList.last();
            continue;
        }
        if (line.contains("Current_Pending_Sector")) {
            qDebug() << "line contains Current_Pending_Sector";
            mapInfo["Current_Pending_Sector"] = strList.last();
            continue;
        }
        if (line.contains("Offline_Uncorrectable")) {
            qDebug() << "line contains Offline_Uncorrectable";
            mapInfo["Offline_Uncorrectable"] = strList.last();
            continue;
        }
        if (line.contains("UDMA_CRC_Error_Count")) {
            qDebug() << "line contains UDMA_CRC_Error_Count";
            mapInfo["UDMA_CRC_Error_Count"] = strList.last();
            continue;
        }
        if (line.contains("Multi_Zone_Error_Rate")) {
            qDebug() << "line contains Multi_Zone_Error_Rate";
            mapInfo["Multi_Zone_Error_Rate"] = strList.last();
            continue;
        }
    }
    qDebug() << "DeviceStorage::getMapInfoFromSmartctl END";
}

}
