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

LUKSOperator::LUKSOperator()
{

}

bool LUKSOperator::updateLUKSInfo(DeviceInfoMap &dev, LVMInfo &lvmInfo,LUKSInfoMap&luks)
{
    //查看所有
    for (QMap<QString, DeviceInfo>::iterator it = dev.begin(); it != dev.end(); ++it) {

    }
    return  true;
}
