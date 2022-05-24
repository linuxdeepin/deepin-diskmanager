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
#ifndef LUKSOPERATOR_H
#define LUKSOPERATOR_H

#include "lvmstruct.h"
#include "deviceinfo.h"

class LUKSOperator
{
public:
    LUKSOperator();

    /**
     * @brief 更新设备上的luks数据
     * @param dev: 设备集合
     * @param lvmInfo:lvm数据结构体
     * @return true 成功 false 失败
     */
    static bool updateLUKSInfo(DeviceInfoMap &dev, LVMInfo &lvmInfo, LUKSMap &luks);

    /**
     * @brief 加密 加密设备并且添加token
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool encrypt(LUKSMap &luks, LUKS_INFO &luksInfo);

    /**
     * @brief 解密
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool decrypt(LUKSMap &luks, LUKS_INFO &luksInfo);


    /**
     * @brief 关闭加密映射
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool closeMapper(LUKSMap &luks, LUKS_INFO &luksInfo);
private:
    /**
     * @brief 判断是否luks设备
     * @param devPath: 设备路径
     * @return true luks设备 false 非luks设备
     */
    static bool isLUKS(QString devPath);

    /**
     * @brief 初始化映射设备
     */
    static bool initMapper(LUKSMap &luks);

    /**
     * @brief 获取支持的算法
     * @param support: 算法结构体
     * @return true 获取成功 false 获取失败
     */
    static bool getCIPHERSupport(CRYPT_CIPHER_Support &support);

    /**
     * @brief 获取luks属性
     * @param devPath: 设备路径
     * @param info: luks属性结构体
     * @return true 获取成功 false 获取失败
     */
    static bool getLUKSInfo(const LUKSMap &luks, const QString& devPath, LUKS_INFO&info);

    /**
     * @brief 通过json解析luks属性
     * @param json: luks属性json字符串
     * @param info: luks属性结构体
     * @return true 获取成功 false 获取失败
     */
    static bool jsonToLUKSInfo(QString json, LUKS_INFO &info);

    /**
     * @brief 加密
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool format(const LUKS_INFO &luks);

    /**
     * @brief 打开映射
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool open(const LUKS_INFO &luks);

    /**
     * @brief 关闭加密映射
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool close(const LUKS_INFO &luks);

    /**
     * @brief 添加token
     * @param luks: luks属性结构体
     * @param list: tokens列表
     * @return true 成功 false 失败
     */
    static bool addToken(const LUKS_INFO &luks, QStringList list);

    /**
     * @brief 添加key
     * @param luks: luks属性结构体
     * @return true 成功 false 失败
     */
    static bool addKey(const LUKS_INFO &luks);

    /**
     * @brief 设置luks错误
     * @param info: luks属性结构体
     * @param err: 错误
     * @return true 正常 false 错误
     */
    static bool setLUKSErr(LUKSMap &luksInfo, const CRYPTError &err);

private:
    static DeviceInfoMap *m_dev;
    static LVMInfo *m_lvmInfo;
    static CRYPTError m_cryErr;
};

#endif // LUKSOPERATOR_H
