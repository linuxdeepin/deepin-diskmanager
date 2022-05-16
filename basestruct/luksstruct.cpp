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

bool CRYPT_CIPHER_Support::supportDecrypt(CRYPT_CIPHER_Support::Support x)
{
    return ((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::CRYPT_ALL);
}

bool CRYPT_CIPHER_Support::supportEncrypt(CRYPT_CIPHER_Support::Support x)
{
    return ((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::NOT_SUPPORT);
}

bool CRYPT_CIPHER_Support::notSupportCrypt(CRYPT_CIPHER_Support::Support x)
{
    return supportAllcrypt(x) || (((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::ENCRYPT));
}

bool CRYPT_CIPHER_Support::supportAllcrypt(CRYPT_CIPHER_Support::Support x)
{
    return supportAllcrypt(x) || (((x) & 0xFFFF) == (CRYPT_CIPHER_Support::Support::DECRYPT));
}



QDBusArgument &operator<<(QDBusArgument &argument, const CRYPT_CIPHER_Support &data)
{
    argument.beginStructure();
    argument << static_cast<int>(data.aes_xts_plain64)
             << static_cast<int>(data.sm4_xts_plain);
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
    data.sm4_xts_plain = static_cast<CRYPT_CIPHER_Support::Support>(sm4);
    argument.endStructure();
    return argument;
}

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
             << data.m_luskType
             << data.m_mode
             << static_cast<int>(data.m_vgflag);
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, LUKS_MapperInfo &data)
{
    argument.beginStructure();
    int luksFlag, vgFlag;
    argument >> luksFlag
             >> data.m_mountPoints
             >> data.m_uuid
             >> data.m_dmPath
             >> data.m_busy
             >> data.m_fsUsed
             >> data.m_fsUnused
             >> data.m_Size
             >> data.m_devicePath
             >> data.m_luskType
             >> data.m_mode
             >> vgFlag;
    data.m_luksFs = static_cast<FSType>(luksFlag);
    data.m_vgflag = static_cast<LVMFlag>(vgFlag);
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const LUKS_INFO &data)
{
    argument.beginStructure();
    argument << data.m_mapper
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
