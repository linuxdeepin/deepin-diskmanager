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
    static bool updateLUKSInfo(DeviceInfoMap &dev, LVMInfo &lvmInfo);

private:

};

#endif // LUKSOPERATOR_H
