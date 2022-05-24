/*
* Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     liuwenhao <liuwenhao@uniontech.com>
*
* Maintainer: liuwenhao <liuwenhao@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "luksstruct.h"
/*********************************** CRYPT_CIPHER_Support *********************************************/
bool CRYPT_CIPHER_Support::supportDecrypt(CRYPT_CIPHER_Support::Support x)
{
    return (supportAllcrypt(x) || (((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::DECRYPT)));
}

bool CRYPT_CIPHER_Support::supportEncrypt(CRYPT_CIPHER_Support::Support x)
{
    return (supportAllcrypt(x) || (((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::ENCRYPT)));
}

bool CRYPT_CIPHER_Support::notSupportCrypt(CRYPT_CIPHER_Support::Support x)
{
    return ((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::NOT_SUPPORT);
}

bool CRYPT_CIPHER_Support::supportAllcrypt(CRYPT_CIPHER_Support::Support x)
{
    return ((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::CRYPT_ALL);
}

QDBusArgument &operator<<(QDBusArgument &argument, const CRYPT_CIPHER_Support &data)
{
    argument.beginStructure();
    argument << static_cast<int>(data.aes_xts_plain64)
             << static_cast<int>(data.sm4_xts_plain64);
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, CRYPT_CIPHER_Support &data)
{
    argument.beginStructure();
    int aes, sm4;
    argument >> aes
             >> sm4;
    data.aes_xts_plain64 = static_cast<CRYPT_CIPHER_Support::Support>(aes);
    data.sm4_xts_plain64 = static_cast<CRYPT_CIPHER_Support::Support>(sm4);
    argument.endStructure();
    return argument;
}
/*********************************** LUKS_MapperInfo *********************************************/
QDBusArgument &operator<<(QDBusArgument &argument, const LUKS_MapperInfo &data)
{
    argument.beginStructure();
    argument << static_cast<int>(data.m_luksFs)
             << data.m_mountPoints
             << data.m_uuid
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
             << data.m_fsLimits.min_size;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKS_MapperInfo &data)
{
    argument.beginStructure();
    int luksFlag, vgFlag, cipher;
    argument >> luksFlag
             >> data.m_mountPoints
             >> data.m_uuid
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
             >> data.m_fsLimits.min_size;

    data.m_luksFs = static_cast<FSType>(luksFlag);
    data.m_vgflag = static_cast<LVMFlag>(vgFlag);
    data.m_crypt = static_cast<CRYPT_CIPHER>(cipher);
    argument.endStructure();
    return argument;
}
/*********************************** LUKS_INFO *********************************************/
QDBusArgument &operator<<(QDBusArgument &argument, const LUKS_INFO &data)
{
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
             << data.m_Suspend;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKS_INFO &data)
{
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
             >> data.m_Suspend;
    data.m_crypt = static_cast<CRYPT_CIPHER>(crypt);
    data.m_cryptErr = static_cast<CRYPTError>(cryptErr);
    argument.endStructure();
    return argument;
}


/*********************************** OperatorLUKSMap *********************************************/
QDBusArgument &operator<<(QDBusArgument &argument, const LUKSMap &data)
{
    argument.beginStructure();
    argument << data.m_luksMap
             << data.m_mapper
             << static_cast<int>(data.m_cryErr)
             << data.m_cryptSuuport;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKSMap &data)
{
    argument.beginStructure();
    int cryptErr;
    argument >> data.m_luksMap
             >> data.m_mapper
             >> cryptErr
             >> data.m_cryptSuuport;
    data.m_cryErr = static_cast<CRYPTError>(cryptErr);
    argument.endStructure();
    return argument;
}

LUKSMap::LUKSMap()
{

}

bool LUKSMap::mapperExists(const QString &path) const
{
    if (itemExists(path, m_mapper)) {
        return true;
    }

    foreach (const LUKS_MapperInfo &tmp, m_mapper) {
        if (tmp.m_dmPath == path) {
            return true;
        }
    }
    return  false;
}


QString LUKSMap::getMapperPath(const QString &devPath) const
{
    return getItem(devPath, m_mapper).m_dmPath;
}

QString LUKSMap::getDevPath(const QString &mapper) const
{
    return getMapper(mapper).m_devicePath;
}

bool LUKSMap::mapperOfDevice(const QString &mapper, const QString &dev) const
{
    return mapperExists(mapper) ? getMapper(mapper).m_devicePath == dev : false;
}

bool LUKSMap::isDevice(const QString &dev) const
{
    return itemExists(dev, m_luksMap);
}

LUKS_MapperInfo LUKSMap::getMapper(const QString &path) const
{
    foreach (const LUKS_MapperInfo &tmp, m_mapper) {
        if (tmp.m_dmPath == path || tmp.m_devicePath == path) {
            return tmp;
        }
    }
    return  LUKS_MapperInfo();
}

LUKS_MapperInfo LUKSMap::getMapper(const LUKS_MapperInfo &mapper) const
{
    return getMapper(mapper.m_devicePath);
}


bool LUKSMap::luksExists(const QString &devPath) const
{
    return itemExists(devPath, m_luksMap);
}

LUKS_INFO LUKSMap::getLUKS(const QString &path) const
{
    QString devicePath = isDevice(path) ? path : getDevPath(path);
    return getItem(devicePath, m_luksMap);
}

template<class T>
bool LUKSMap::itemExists(const QString &str, const QMap<QString, T> &containers)const
{
    return containers.find(str) != containers.end();
}

template<class T>
T LUKSMap::getItem(const QString &str, const QMap<QString, T> &containers)const
{
    return itemExists(str, containers) ? *containers.find(str) : T();
}



