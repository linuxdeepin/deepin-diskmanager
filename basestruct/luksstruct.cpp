// Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "luksstruct.h"

#include <QDebug>

/*********************************** CRYPT_CIPHER_Support *********************************************/
bool CRYPT_CIPHER_Support::supportDecrypt(CRYPT_CIPHER_Support::Support x)
{
    qDebug() << "Checking decrypt support for cipher type:" << x;
    return (supportAllcrypt(x) || (((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::DECRYPT)));
}

bool CRYPT_CIPHER_Support::supportEncrypt(CRYPT_CIPHER_Support::Support x)
{
    qDebug() << "Checking encrypt support for cipher type:" << x;
    return (supportAllcrypt(x) || (((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::ENCRYPT)));
}

bool CRYPT_CIPHER_Support::notSupportCrypt(CRYPT_CIPHER_Support::Support x)
{
    qDebug() << "Checking no support for cipher type:" << x;
    return ((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::NOT_SUPPORT);
}

bool CRYPT_CIPHER_Support::supportAllcrypt(CRYPT_CIPHER_Support::Support x)
{
    qDebug() << "Checking all crypt support for cipher type:" << x;
    return ((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::CRYPT_ALL);
}

QDBusArgument &operator<<(QDBusArgument &argument, const CRYPT_CIPHER_Support &data)
{
    // qDebug() << "Starting CRYPT_CIPHER_Support serialization";
    argument.beginStructure();
    argument << static_cast<int>(data.aes_xts_plain64)
             << static_cast<int>(data.sm4_xts_plain64);
    argument.endStructure();
    // qDebug() << "Completed CRYPT_CIPHER_Support serialization";
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, CRYPT_CIPHER_Support &data)
{
    // qDebug() << "Starting CRYPT_CIPHER_Support deserialization";
    argument.beginStructure();
    int aes, sm4;
    argument >> aes
             >> sm4;
    data.aes_xts_plain64 = static_cast<CRYPT_CIPHER_Support::Support>(aes);
    data.sm4_xts_plain64 = static_cast<CRYPT_CIPHER_Support::Support>(sm4);
    argument.endStructure();
    // qDebug() << "Completed CRYPT_CIPHER_Support deserialization";
    return argument;
}
/*********************************** LUKS_MapperInfo *********************************************/
QDBusArgument &operator<<(QDBusArgument &argument, const LUKS_MapperInfo &data)
{
    // qDebug() << "Starting LUKS_MapperInfo serialization";
    argument.beginStructure();
    argument << static_cast<int>(data.m_luksFs)
             << data.m_mountPoints
             << data.m_uuid
             << data.m_dmName
             << data.m_dmPath
             << data.m_busy
             << data.m_fsUsed
             << data.m_fsUnused
             << data.m_Size
             << data.m_devicePath
             << static_cast<int>(data.m_crypt)
             << data.m_luskType
             << data.m_mode
             << static_cast<int>(data.m_vgflag)
             << data.m_fsLimits.max_size
             << data.m_fsLimits.min_size
             << data.m_fileSystemLabel;
    argument.endStructure();
    // qDebug() << "Completed LUKS_MapperInfo serialization";
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKS_MapperInfo &data)
{
    // qDebug() << "Starting LUKS_MapperInfo deserialization";
    argument.beginStructure();
    int luksFlag, vgFlag, cipher;
    argument >> luksFlag
             >> data.m_mountPoints
             >> data.m_uuid
             >> data.m_dmName
             >> data.m_dmPath
             >> data.m_busy
             >> data.m_fsUsed
             >> data.m_fsUnused
             >> data.m_Size
             >> data.m_devicePath
             >> cipher
             >> data.m_luskType
             >> data.m_mode
             >> vgFlag
             >> data.m_fsLimits.max_size
             >> data.m_fsLimits.min_size
             >> data.m_fileSystemLabel;
    data.m_luksFs = static_cast<FSType>(luksFlag);
    data.m_vgflag = static_cast<LVMFlag>(vgFlag);
    data.m_crypt = static_cast<CRYPT_CIPHER>(cipher);
    argument.endStructure();
    // qDebug() << "Completed LUKS_MapperInfo deserialization";
    return argument;
}
/*********************************** LUKS_INFO *********************************************/
QDBusArgument &operator<<(QDBusArgument &argument, const LUKS_INFO &data)
{
    // qDebug() << "Starting LUKS_INFO serialization";
    argument.beginStructure();
    argument << data.m_mapper
             << data.m_devicePath
             << static_cast<int>(data.m_crypt)
             << data.m_luksVersion
             << static_cast<int>(data.m_cryptErr)
             << data.m_dmUUID
             << data.m_tokenList
             << data.m_decryptErrCount
             << data.m_decryptErrorLastTime
             << data.m_decryptStr
             << data.isDecrypt
             << data.m_keySlots
             << data.m_Suspend
             << data.m_fileSystemLabel;
    argument.endStructure();
    // qDebug() << "Completed LUKS_INFO serialization";
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKS_INFO &data)
{
    // qDebug() << "Starting LUKS_INFO deserialization";
    argument.beginStructure();
    int crypt, cryptErr;
    argument >> data.m_mapper
             >> data.m_devicePath
             >> crypt
             >> data.m_luksVersion
             >> cryptErr
             >> data.m_dmUUID
             >> data.m_tokenList
             >> data.m_decryptErrCount
             >> data.m_decryptErrorLastTime
             >> data.m_decryptStr
             >> data.isDecrypt
             >> data.m_keySlots
             >> data.m_Suspend
             >> data.m_fileSystemLabel;
    data.m_crypt = static_cast<CRYPT_CIPHER>(crypt);
    data.m_cryptErr = static_cast<CRYPTError>(cryptErr);
    argument.endStructure();
    // qDebug() << "Completed LUKS_INFO deserialization";
    return argument;
}
/*********************************** LUKSMap ************************************************/
QDBusArgument &operator<<(QDBusArgument &argument, const LUKSMap &data)
{
    // qDebug() << "Starting LUKSMap serialization";
    argument.beginStructure();
    argument << data.m_luksMap
             << data.m_mapper
             << static_cast<int>(data.m_cryErr)
             << data.m_cryptSuuport;
    argument.endStructure();
    // qDebug() << "Completed LUKSMap serialization";
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKSMap &data)
{
    // qDebug() << "Starting LUKSMap deserialization";
    argument.beginStructure();
    int cryptErr;
    argument >> data.m_luksMap
             >> data.m_mapper
             >> cryptErr
             >> data.m_cryptSuuport;
    data.m_cryErr = static_cast<CRYPTError>(cryptErr);
    argument.endStructure();
    // qDebug() << "Completed LUKSMap deserialization";
    return argument;
}

LUKSMap::LUKSMap()
{
    qDebug() << "LUKSMap object created";
}

bool LUKSMap::mapperExists(const QString &path) const
{
    qDebug() << "Checking mapper existence for path:" << path;
    if (itemExists(path, m_mapper)) {
        qDebug() << "Mapper exists for path:" << path;
        return true;
    }
    
    foreach (const LUKS_MapperInfo &tmp, m_mapper) {
        if (tmp.m_dmPath == path) {
            // qDebug() << "Mapper found by dmPath for path:" << path;
            return true;
        }
    }
    qDebug() << "Mapper does not exist for path:" << path;
    return false;
}


QString LUKSMap::getMapperPath(const QString &devPath) const
{
    qDebug() << "Getting mapper path for device:" << devPath;
    QString mapperPath = getItem(devPath, m_mapper).m_dmPath;
    qDebug() << "Mapper path for device" << devPath << "is:" << mapperPath;
    return mapperPath;
}

QString LUKSMap::getDevPath(const QString &mapper) const
{
    qDebug() << "Getting device path for mapper:" << mapper;
    QString devPath = getMapper(mapper).m_devicePath;
    qDebug() << "Device path for mapper" << mapper << "is:" << devPath;
    return devPath;
}

bool LUKSMap::mapperOfDevice(const QString &mapper, const QString &dev) const
{
    qDebug() << "Checking if mapper" << mapper << "belongs to device" << dev;
    bool isMatch = mapperExists(mapper) ? getMapper(mapper).m_devicePath == dev : false;
    qDebug() << "Mapper" << mapper << (isMatch ? "matches" : "does not match") << "device" << dev;
    return isMatch;
}

bool LUKSMap::deviceExists(const QString &dev) const
{
    qDebug() << "Checking device existence:" << dev;
    bool exists = itemExists(dev, m_luksMap);
    qDebug() << "Device" << dev << (exists ? "exists" : "does not exist") << "in LUKS map";
    return exists;
}

LUKS_MapperInfo LUKSMap::getMapper(const QString &path) const
{
    // qDebug() << "Getting mapper for path:" << path;
    foreach (const LUKS_MapperInfo &tmp, m_mapper) {
        if (tmp.m_dmPath == path || tmp.m_devicePath == path) {
            qDebug() << "Mapper found for path:" << path;
            return tmp;
        }
    }
    qDebug() << "No mapper found for path:" << path;
    return LUKS_MapperInfo();
}

LUKS_MapperInfo LUKSMap::getMapper(const LUKS_MapperInfo &mapper) const
{
    // qDebug() << "Getting mapper for device path:" << mapper.m_devicePath;
    return getMapper(mapper.m_devicePath);
}


bool LUKSMap::luksExists(const QString &devPath) const
{
    bool exists = itemExists(devPath, m_luksMap);
    qDebug() << "LUKS" << (exists ? "exists" : "does not exist") << "for device path:" << devPath;
    return exists;
}

LUKS_INFO LUKSMap::getLUKS(const QString &path) const
{
    // qDebug() << "Getting LUKS for path:" << path;
    QString devicePath = deviceExists(path) ? path : getDevPath(path);
    return getItem(devicePath, m_luksMap);
}

template<class T>
bool LUKSMap::itemExists(const QString &str, const QMap<QString, T> &containers)const
{
    // qDebug() << "Checking existence of item:" << str;
    return containers.find(str) != containers.end();
}

template<class T>
T LUKSMap::getItem(const QString &str, const QMap<QString, T> &containers)const
{
    // qDebug() << "Getting item:" << str;
    return itemExists(str, containers) ? *containers.find(str) : T();
}



