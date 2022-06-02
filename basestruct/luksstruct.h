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
#ifndef LUKSSTRUCT_H
#define LUKSSTRUCT_H
#include "utils.h"
#include "luksenum.h"
#include "lvmenum.h"

/*

root@uos-PC:/home/uos/Desktop# dmsetup ls --target crypt
t1      (254:1)


root@uos-PC:/home/uos/Desktop# cryptsetup -v status tttt
/dev/mapper/tttt is active.
  type:    LUKS2
  cipher:  aes-xts-plain64
  keysize: 512 bits
  key location: keyring
  device:  /dev/sdc1
  sector size:  512
  offset:  32768 sectors
  size:    20938752 sectors
  mode:    read/write


root@uos-PC:/home/uos/Desktop# cryptsetup luksDump /dev/sdc1
LUKS header information
Version:        2
Epoch:          3
Metadata area:  16384 [bytes]
Keyslots area:  16744448 [bytes]
UUID:           e5b2cde1-6063-47a2-891d-170e03299f20
Label:          (no label)
Subsystem:      (no subsystem)
Flags:          (no flags)

Data segments:
  0: crypt
        offset: 16777216 [bytes]
        length: (whole device)
        cipher: aes-xts-plain64
        sector: 512 [bytes]

Keyslots:
  0: luks2
        Key:        512 bits
        Priority:   normal
        Cipher:     aes-xts-plain64
        Cipher key: 512 bits
        PBKDF:      argon2i
        Time cost:  8
        Memory:     1048576
        Threads:    4
        Salt:       66 21 57 6d 9a 41 e2 c9 a9 39 19 1c 4e 8a 6b 1b
                    62 55 e5 35 8a 8a cb e6 a7 34 8c dd 18 1b 11 f4
        AF stripes: 4000
        AF hash:    sha256
        Area offset:32768 [bytes]
        Area length:258048 [bytes]
        Digest ID:  0
Tokens:
Digests:
  0: pbkdf2
        Hash:       sha256
        Iterations: 157538
        Salt:       6b ad b2 af 2a 0f ff 63 03 68 7b 25 fe 8e c6 70
                    8d a0 b6 32 34 c4 88 6b 17 6b c0 8b 5f de a2 5d
        Digest:     12 1f bd e8 f1 a7 c6 34 58 b1 88 0c d3 d1 68 f0
                    53 38 8b bf 9e b2 ed b1 26 23 b2 c6 9a 8a bc f4

**/

//new by liuwh 2022/4/27
/**
 * @struct CRYPT_CIPHER_Support
 * @brief CRYPT_CIPHER_Support 加密算法支持
 */

struct CRYPT_CIPHER_Support {
public:
    enum Support {
        NOT_SUPPORT = 0x0000,    //not support
        ENCRYPT = 0x0001,        //支持加密
        DECRYPT = 0x0002,        //支持解密
        CRYPT_ALL = ENCRYPT | DECRYPT //支持加解密
    };

    Support aes_xts_plain64 = NOT_SUPPORT;
    Support sm4_xts_plain64 = NOT_SUPPORT;

    static  bool supportAllcrypt(CRYPT_CIPHER_Support::Support x);
    static  bool notSupportCrypt(CRYPT_CIPHER_Support::Support x);
    static  bool supportEncrypt(CRYPT_CIPHER_Support::Support x);
    static  bool supportDecrypt(CRYPT_CIPHER_Support::Support x);
};
DBUSStructEnd(CRYPT_CIPHER_Support)

//new by liuwh 2022/4/27
/**
 * @class LUKS_MapperInfo
 * @brief LUKS_MapperInfo luks映射盘属性
 */
class LUKS_MapperInfo
{
public:
    FSType m_luksFs{FSType::FS_UNKNOWN};                    //文件系统类型 dm解密后获取
    QVector<QString> m_mountPoints;                         //挂载点
    QString m_uuid;                                         //fstab
    QString m_dmName;                                       //映射名称    sdb1_aesE
    QString m_dmPath;                                       //映射设备路径 /dev/mapper/sdb1_aesE
    bool m_busy{false};                                     //挂载标志
    long long  m_fsUsed{0};                                 //文件已经使用大小 单位byte
    long long  m_fsUnused{0};                               //文件未使用大小 单位byte
    QString m_Size;                                         //字符串类型 展示用
    QString m_devicePath;                                   //原始设备 例如/dev/sdb1 加密映射
    CRYPT_CIPHER m_crypt{CRYPT_CIPHER::NOT_CRYPT};          //加密算法
    QString m_luskType;                                     //LUKS1 LUKS2
    QString m_mode;                                         //read/write
    LVMFlag m_vgflag{LVMFlag::LVM_FLAG_NOT_PV};             //pv设备标识  当该标识为LVM_FLAG_JOIN_VG时，数据从lvminfo中获取
    FS_Limits m_fsLimits;                                   //文件系统限制 该属性在没有文件系统存在时无效
    //QString m_fileSystemLabel;                              //卷标名  TODO: 之后补上该属性
};
DBUSStructEnd(LUKS_MapperInfo)

//new by liuwh 2022/4/27
/**
 * @class LUKS_INFO
 * @brief LUKS_INFO luks原始盘属性
 */
class LUKS_INFO
{
public:
    LUKS_MapperInfo m_mapper;                               //映射盘属性
    QString m_devicePath;                                   //原始设备 例如/dev/sdb1 加密映射
    CRYPT_CIPHER m_crypt{CRYPT_CIPHER::NOT_CRYPT};          //加密算法
    int m_luksVersion{0};                                      // 1 2

    CRYPTError m_cryptErr{CRYPTError::CRYPT_ERR_NORMAL};    //crypt错误
    QString m_dmUUID;                                       //crypttab  device mapper

    QStringList m_tokenList;                                //密钥提示   luks####提示信息####
    unsigned int m_decryptErrCount{0};                      //解密错误尝试次数  8
    QString m_decryptErrorLastTime;                         //解密错误尝试时间  date
    QString m_decryptStr;                                   //用户解密密码字符串

    bool isDecrypt{false};                                  //是否解密
    int m_keySlots = 0;                                     //当前密钥槽个数

    bool m_Suspend = false;                                 //是否挂起  该属性获取待定
    QString m_fileSystemLabel;                              //卷标名
};
DBUSStructEnd(LUKS_INFO)

typedef QMap<QString, LUKS_INFO> LUKSInfoMap;
Q_DECLARE_METATYPE(LUKSInfoMap)

//new by liuwh 2022/5/17
/**
 * @class OperatorLUKSMap
 * @brief 专为操作luksmap结构体用
 */
class LUKSMap
{
public:
    LUKSMap();

    //判断是否存在
    bool mapperExists(const QString &path)const;
    //通过dev获取mapper路径
    QString getMapperPath(const QString &devPath)const;
    //通过mapper获取dev路径
    QString getDevPath(const QString &mapper)const;
    //判断映射是否属于dev
    bool mapperOfDevice(const QString &mapper, const QString &dev)const;
    //判断是否为dev
    bool isDevice(const QString &dev)const;
    //获取mapper
    LUKS_MapperInfo getMapper(const QString &path)const; //映射盘原始设备 /dev/sda1
    LUKS_MapperInfo getMapper(const LUKS_MapperInfo &mapper)const; //映射盘原始设备 /dev/sda1
    //判断luks是否存在
    bool luksExists(const QString &devPath)const;
    //获取Luks
    LUKS_INFO getLUKS(const QString &path)const; //映射盘原始设备 /dev/sda1
private:
    template<class T>
    T getItem(const QString &str, const QMap<QString, T> &containers)const;

    template<class T>
    bool itemExists(const QString &str, const QMap<QString, T> &containers)const;
public:
    LUKSInfoMap m_luksMap;                          // key:/dev/sda1  value: LUKS_INFO  Or key:/dev/vg01/lv01  value: LUKS_INFO
    CRYPTError m_cryErr;
    QMap<QString, LUKS_MapperInfo>m_mapper;         // key:/dev/sda1   value: LUKS_MapperInfo  Or  key:/dev/vg01/lv01  value:  LUKS_MapperInfo
    CRYPT_CIPHER_Support m_cryptSuuport;
};
DBUSStructEnd(LUKSMap)



#endif // LUKSSTRUCT_H
