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
#ifndef LUKSENUM_H
#define LUKSENUM_H

//new by liuwh 2022/4/27
/**
 * @enum LUKSFlag
 * @brief LUKS 设备标志  判断是否为lusk设备
 */
enum LUKSFlag {
    NOT_CRYPT_LUKS = 0,             //非crypt_luks设备
    IS_CRYPT_LUKS,                  //crypt_luks设备
};



//new by liuwh 2022/4/27
/**
 * @enum CRYPTError
 * @brief CRYPTError 错误类型
 */
enum CRYPTError {
    CRYPT_ERR_NORMAL = 0,            //正常
    CRYPT_ERR_INIT_FAILED,           //初始化失败
    CRYPT_ERR_NO_CMD_SUPPORT,        //外部命令不支持
    CRYPT_ERR_ENCRYPT_FAILED,        //加密失败
    CRYPT_ERR_DECRYPT_FAILED,        //解密失败
    CRYPT_ERR_ENCRYPT_ARGUMENT,      //加密参数错误
    CRYPT_ERR_GET_LUKSINFO_FAILED,   //获取luksinfo失败
    CRYPT_ERR_DEVICE_NO_EXISTS,      //设备不存在
    CRYPT_ERR_DM_DEVICE_NO_EXISTS,   //映射设备不存在
    CRYPT_ERR_MOUNT_FAILED,          //挂载失败
    CRYPT_ERR_ADD_KEY_FAILED,        //添加key失败
    CRYPT_ERR_DEL_KEY_FAILED,        //删除key失败
    CRYPT_ERR_CRYPTTAB_FAILED,       //修改crypttab文件失败
    CRYPT_ERR_CLOSE_FAILED           //关闭设备失败
};



//new by liuwh 2022/5/13
/**
 * @struct CRYPT_CIPHER
 * @brief CRYPT_CIPHER 加密算法
 */
enum CRYPT_CIPHER {
    NOT_CRYPT = 0,                   //无加密算法
    AES_XTS_PLAIN64 = 1,             //aes 加密
    SM4_XTS_PLAIN64 = 2,             //sm4 加密

    CRYPT_UNkNOW = 100               //未知加密算法
};

#endif // LUKSENUM_H
