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
#include "luksoperator.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>

DeviceInfoMap *LUKSOperator::m_dev = nullptr;
LVMInfo *LUKSOperator::m_lvmInfo = nullptr;
CRYPTError LUKSOperator::m_cryErr = CRYPTError::CRYPT_ERR_NORMAL;
/***********************************************public****************************************************************/

LUKSOperator::LUKSOperator()
{

}

bool LUKSOperator::updateLUKSInfo(DeviceInfoMap &dev, LVMInfo &lvmInfo, LUKSMap &luks)
{
    m_dev = &dev;
    m_lvmInfo = &lvmInfo;
    luks.m_cryErr = CRYPTError::CRYPT_ERR_NORMAL;
    //初始化映射 算法
    if (!initMapper(luks) || !getCIPHERSupport(luks.m_cryptSuuport)) {
        return setLUKSErr(luks, CRYPTError::CRYPT_ERR_INIT_FAILED);
    }

    //磁盘luks信息
    for (DeviceInfoMap::iterator devIt = dev.begin(); devIt != dev.end(); ++devIt) {
        devIt.value().m_crySupport = luks.m_cryptSuuport;
        if (!lvmInfo.lvInfoExists(devIt.key()) && isLUKS(devIt.key())) { //判断是否是lv加密的映射盘
            devIt->m_luksFlag = LUKSFlag::IS_CRYPT_LUKS;
            LUKS_INFO info;
            if (!getLUKSInfo(luks, devIt.key(), info)) {
                return setLUKSErr(luks, CRYPTError::CRYPT_ERR_GET_LUKSINFO_FAILED);
            }
            luks.m_luksMap.insert(info.m_devicePath, info);
            continue;
        }
        //分区luks信息
        for (PartitionVec::iterator partIt = devIt.value().m_partition.begin(); partIt != devIt.value().m_partition.end(); ++partIt) {
            if (partIt->m_fileSystemType == FSType::FS_LUKS) { //获取分区数据  分区通过文件系统判断是否为luks
                partIt->m_luksFlag = LUKSFlag::IS_CRYPT_LUKS;
                devIt->m_luksFlag = LUKSFlag::IS_CRYPT_LUKS;
                LUKS_INFO info;
                if (!getLUKSInfo(luks, partIt->m_path, info)) {
                    return setLUKSErr(luks, CRYPTError::CRYPT_ERR_GET_LUKSINFO_FAILED);
                }
                luks.m_luksMap.insert(info.m_devicePath, info);
            }
        }
    }
    //lv luks信息
    for (QMap<QString, VGInfo>::iterator vgIt = lvmInfo.m_vgInfo.begin(); vgIt != lvmInfo.m_vgInfo.end(); ++vgIt) {
        for (QVector<LVInfo>::iterator lvIt = vgIt.value().m_lvlist.begin(); lvIt != vgIt.value().m_lvlist.end(); ++lvIt) {
            if (isLUKS(lvIt->m_lvPath)) {
                lvIt->m_luksFlag = LUKSFlag::IS_CRYPT_LUKS;
                vgIt.value().m_luksFlag = LUKSFlag::IS_CRYPT_LUKS;
                LUKS_INFO info;
                if (!getLUKSInfo(luks, lvIt->toMapperPath(), info)) {
                    return setLUKSErr(luks, CRYPTError::CRYPT_ERR_GET_LUKSINFO_FAILED);
                }
                info.m_devicePath = lvIt->m_lvPath;
                info.m_mapper.m_devicePath = lvIt->m_lvPath;
                luks.m_luksMap.insert(info.m_devicePath, info);
            }
        }
    }

    return setLUKSErr(luks, CRYPTError::CRYPT_ERR_NORMAL);
}

bool LUKSOperator::encrypt(LUKSMap &luks, LUKS_INFO &luksInfo)
{
    //判断参数是否正确
    if (!(luksInfo.m_crypt == CRYPT_CIPHER::AES_XTS_PLAIN64 || luksInfo.m_crypt == CRYPT_CIPHER::SM4_XTS_PLAIN64)
            || luksInfo.m_devicePath.isEmpty()
            || luksInfo.m_decryptStr.isEmpty()) {
        return setLUKSErr(luks, CRYPTError::CRYPT_ERR_ENCRYPT_ARGUMENT);
    }
    //加密
    if (!format(luksInfo)) {
        return setLUKSErr(luks, CRYPTError::CRYPT_ERR_ENCRYPT_FAILED);
    }
    //添加token
    if (!addToken(luksInfo, luksInfo.m_tokenList)) {
        return setLUKSErr(luks, CRYPTError::CRYPT_ERR_DECRYPT_FAILED);
    }

    return setLUKSErr(luks, CRYPTError::CRYPT_ERR_NORMAL);
}

bool LUKSOperator::decrypt(LUKSMap &luks, LUKS_INFO &luksInfo)
{
    //判断参数是否正确
    if (luksInfo.m_mapper.m_dmName.isEmpty()
            || luksInfo.m_devicePath.isEmpty()
            || luksInfo.m_decryptStr.isEmpty()) {
        return setLUKSErr(luks, CRYPTError::CRYPT_ERR_ENCRYPT_ARGUMENT);
    }
    return open(luksInfo) ? setLUKSErr(luks, CRYPTError::CRYPT_ERR_NORMAL)
           : setLUKSErr(luks, CRYPTError::CRYPT_ERR_DECRYPT_FAILED);

}

bool LUKSOperator::closeMapper(LUKSMap &luks, LUKS_INFO &luksInfo)
{
    if (luksInfo.isDecrypt == false) {
        return true;
    }
    //判断参数是否正确
    if (luksInfo.m_mapper.m_dmPath.isEmpty()) {
        return setLUKSErr(luks, CRYPTError::CRYPT_ERR_ENCRYPT_ARGUMENT);
    }
    return close(luksInfo) ? setLUKSErr(luks, CRYPTError::CRYPT_ERR_NORMAL)
           : setLUKSErr(luks, CRYPTError::CRYPT_ERR_DECRYPT_FAILED);
}
/***********************************************private****************************************************************/

bool LUKSOperator::isLUKS(QString devPath)
{
    QString cmd, strout, strerr;
    cmd = QString("cryptsetup isLuks %1").arg(devPath);
    return  Utils::executCmd(cmd, strout, strerr) == 0;
}

bool LUKSOperator::initMapper(LUKSMap &luks)
{
    luks.m_mapper.clear();
    QString cmd, strout, strerr;
    cmd = QString("dmsetup ls --target crypt"); //获取所有解密的设备
    if (Utils::executCmd(cmd, strout, strerr) != 0) {
        return false;
    }

    foreach (QString str, strout.split("\n")) {
        QStringList list = str.split("\t");
        if (list.count() == 2) {
            LUKS_MapperInfo mapper;
            mapper.m_dmName = list[0];
            mapper.m_dmPath = "/dev/mapper/" + mapper.m_dmName;

            QString cmd2, strout2, strerr2;
            cmd2 = QString(" cryptsetup status %1").arg(mapper.m_dmPath);
            if (Utils::executCmd(cmd2, strout2, strerr2) != 0) {
                return false;
            }

            foreach (QString str2, strout2.split("\n")) {
                QStringList list2 = str2.split(":");
                if (list2.count() == 2) {
                    if (list2[0].contains("type")) {
                        mapper.m_luskType = list2[1].trimmed();
                    }

                    if (list2[0].contains("cipher")) {
                        mapper.m_crypt = Utils::getCipher(list2[1].trimmed());
                    }

                    if (list2[0].contains("device")) {
                        mapper.m_devicePath = list2[1].trimmed();
                    }

                    if (list2[0].contains("mode")) {
                        mapper.m_mode = list2[1].trimmed();
                    }
                }
            }

            auto it = m_dev->find(mapper.m_dmPath);
            if (it != m_dev->end()) {
                auto partIt =  it.value().m_partition.begin();
                if (partIt != it.value().m_partition.end()) {
                    PartitionInfo part = *partIt;
                    mapper.m_busy = part.m_busy;
                    mapper.m_mountPoints = part.m_mountPoints;
                    mapper.m_luksFs = static_cast<FSType>(part.m_fileSystemType);
                    mapper.m_fsUsed = part.m_sectorsUsed * it.value().m_sectorSize;
                    mapper.m_fsUnused = part.m_sectorsUnused * it.value().m_sectorSize;
                    mapper.m_Size = Utils::LVMFormatSize(mapper.m_fsUsed + mapper.m_fsUnused);
                    mapper.m_uuid = part.m_uuid;
                    mapper.m_fsLimits = part.m_fsLimits;
                    if (FS_FAT32 == mapper.m_luksFs || FS_FAT16 == mapper.m_luksFs) {
                        mapper.m_fsLimits = FS_Limits(-1, -1); //fat格式不支持逻辑卷的扩展缩小
                    } else if (FS_UNALLOCATED ==   mapper.m_luksFs) { //empty fs , no limits
                        mapper.m_fsLimits = FS_Limits(0, 0);
                    }
                }
            }

            if (m_lvmInfo->pvExists(mapper.m_dmPath)) {
                PVInfo pv = m_lvmInfo->getPV(mapper.m_dmPath);
                mapper.m_vgflag = pv.joinVG() ? LVMFlag::LVM_FLAG_JOIN_VG : LVMFlag::LVM_FLAG_NOT_JOIN_VG;
            }

            luks.m_mapper.insert(mapper.m_devicePath, mapper);
        }
    }

    return true;
}

bool LUKSOperator::getCIPHERSupport(CRYPT_CIPHER_Support &support)
{
    QString cmd, strout, strerr;
    QStringList cipherList = {"aes", "sm4"}; //后续有算法添加 在此处增加算法名称

    cmd = QString("modprobe -c");
    if (Utils::executCmd(cmd, strout, strerr) != 0) {
        return false;
    }

    for (int i = 0; i < cipherList.count(); ++i) {
        int a = 0;
        if (strout.contains(QString("crypto_%1_decrypt").arg(cipherList[i]))) {
            a = CRYPT_CIPHER_Support::Support::DECRYPT;
        }

        if (strout.contains(QString("crypto_%2_encrypt").arg(cipherList[i]))) {
            a |= CRYPT_CIPHER_Support::Support::ENCRYPT;
        }
        if (i == 0) {
            support.aes_xts_plain64 = static_cast<CRYPT_CIPHER_Support::Support>(a);
        }
        if (i == 1) {
            support.sm4_xts_plain64 = static_cast<CRYPT_CIPHER_Support::Support>(a);
        }
    }
    return true;
}

bool LUKSOperator::getLUKSInfo(const LUKSMap &luks, const QString &devPath, LUKS_INFO &info)
{
    QString cmd, strout, strerr;
    cmd = QString("cryptsetup luksDump %1 --debug-json").arg(devPath);
    if (Utils::executCmd(cmd, strout, strerr) != 0) {
        return false;
    }
    info.m_devicePath = devPath;
    //查找mapper数据 如果映射存在
    info.isDecrypt = luks.mapperExists(devPath);
    if (info.isDecrypt) {
        info.m_mapper = luks.getMapper(devPath);
    }
    //解析字符串 获取版本与uuid
    foreach (QString str, strout.split("\n")) {
        if (str.contains("Version:")) {
            sscanf(str.toStdString().c_str(), "Version:\t%d", &info.m_luksVersion);
        }

        if (str.contains("UUID:")) {
            char buf[1024] = {0};
            sscanf(str.toStdString().c_str(), "UUID:\t%s", buf);
            info.m_dmUUID = buf;
        }
    }

    //解析json 获取luks数据
    int jsonBegin = strout.indexOf("# {");
    int jsonEnd = strout.indexOf("}\nLUKS header information"); //截取json字符串
    return jsonToLUKSInfo(strout.mid(jsonBegin + 1, jsonEnd - jsonBegin + 1), info);

}

bool LUKSOperator::jsonToLUKSInfo(QString jsonStr, LUKS_INFO &info)
{
    //获取对象填入value中  同时判断该value是否为object
    auto getObjectValue = [ = ](QJsonObject obj, QString key, QJsonValue & value)->bool{
        if (!obj.contains(key))
        {
            return false;
        }
        value = obj.take(key);
        return value.isObject();
    };

    //判断json字符串是否可以解析
    QJsonParseError jsonErr;
    QJsonDocument json = QJsonDocument::fromJson(jsonStr.toLocal8Bit(), &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError || !json.isObject()) {
        return false;
    }
    //判断对象是否存在
    QJsonObject obj = json.object();
    QJsonValue tokenValue;
    QJsonValue keyslotsValue;
    QJsonValue segValue;
    if (!(getObjectValue(obj, "tokens", tokenValue)
            && getObjectValue(obj, "keyslots", keyslotsValue)
            && getObjectValue(obj, "segments", segValue))) {
        return false;
    }

    //获取数据 tokens keyslots segments
    //获取密码提示可以为空
    if (!tokenValue.isNull()) {
        QJsonObject tobj = tokenValue.toObject();
        for (auto iter = tobj.begin(); iter != tobj.end(); ++iter) {
            if (!iter.value().isObject()) {
                return false;
            }
            QJsonObject tobj2 = iter.value().toObject();
            if (!tobj2.isEmpty()) {
                QJsonValue val = tobj2.take("key_description");
                if (!val.isString()) {
                    return false;
                }
                info.m_tokenList.append(val.toString());
            }
        }
    }

    //获取密码使用槽个数  密钥槽不能为空 为空或者不是对象 说明有问题
    if (keyslotsValue.isNull() || !keyslotsValue.isObject()) {
        return false;
    }
    info.m_keySlots = keyslotsValue.toObject().count();


    //获取加密格式
    if (segValue.isNull() || !segValue.isObject()) {
        return false;
    }
    QJsonObject sObj = segValue.toObject();
    if (!sObj.contains("0")) { //目前假设没有数据段0 为异常状态
        return false;
    }
    QJsonValue value2 = sObj.take("0");
    if (!value2.isObject()) {
        return false;
    }
    QJsonObject sObj2 = value2.toObject();
    if (sObj2.contains("encryption")) {
        QJsonValue value3 = sObj2.take("encryption");
        if (!value3.isString()) {
            return false;
        }
        info.m_crypt = Utils::getCipher(value3.toString().trimmed());
    }
    return true;
}

bool LUKSOperator::format(const LUKS_INFO &luks)
{
    QProcess proc;
    QStringList options;
    options << "-c" << QString("echo -n '%1' | cryptsetup --cipher %2 --key-size 256 --hash sha256 luksFormat %3 -q")
            .arg(luks.m_decryptStr)
            .arg(Utils::getCipherStr(luks.m_crypt))
            .arg(luks.m_devicePath);
    proc.start("/bin/bash", options);
    proc.waitForFinished();
    proc.waitForReadyRead();
    proc.close();
    return proc.exitCode() == 0;
}

bool LUKSOperator::open(const LUKS_INFO &luks)
{
    QProcess proc;
    QStringList options;
    options << "-c" << QString("echo -n '%1' | cryptsetup open %2 %3 -q")
            .arg(luks.m_decryptStr)
            .arg(luks.m_devicePath)
            .arg(luks.m_mapper.m_dmName);
    proc.start("/bin/bash", options);
    proc.waitForFinished();
    proc.waitForReadyRead();//
    proc.close();
    return proc.exitCode() == 0;
}

bool LUKSOperator::close(const LUKS_INFO &luks)
{
    QString cmd, strout, strerr;
    cmd = QString("cryptsetup close %1").arg(luks.m_mapper.m_dmPath);
    return Utils::executCmd(cmd, strout, strerr) == 0;
}

bool LUKSOperator::addToken(const LUKS_INFO &luks, QStringList list)
{
    foreach (QString str, list) {
        QProcess proc;
        QStringList options;
        options << "-c" << QString("echo -n '%1' |cryptsetup token add  --key-description='%2' %3 ")
                .arg(luks.m_decryptStr)
                .arg(str)
                .arg(luks.m_devicePath);;
        proc.start("/bin/bash", options);
        proc.waitForFinished();
        proc.waitForReadyRead();//
        proc.close();
        if (proc.exitCode() != 0) {
            return false;
        }
    }
    return true;
}

bool LUKSOperator::addKey(const LUKS_INFO &luks)
{
    return  true;
}


bool LUKSOperator::setLUKSErr(LUKSMap &luksInfo, const CRYPTError &err)
{
    m_cryErr = err;
    luksInfo.m_cryErr = err;
    return luksInfo.m_cryErr == CRYPTError::CRYPT_ERR_NORMAL;
}
