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
#ifndef LVMOPERATOR_H
#define LVMOPERATOR_H

#include "lvmstruct.h"
#include "deviceinfo.h"

class LVMOperator
{
public:
    LVMOperator();
    static bool updateLVMInfo(LVMInfo &m_lvmInfo);
    static bool getDeviceLVMData(DeviceInfoMap &dev, const LVMInfo &m_lvmInfo);
    static bool getDeviceDataAndLVMInfo(DeviceInfoMap &dev, LVMInfo &m_lvmInfo);
    static void printError(const LVMError &error);
    static void printLVMInfo(const LVMInfo &m_lvmInfo);
    static void resetLVMInfo(LVMInfo &m_lvmInfo);
private:
    static bool updatePVInfo(LVMInfo &m_lvmInfo);
    static bool updateVGInfo(LVMInfo &m_lvmInfo);
    static bool updateLVInfo(LVMInfo &m_lvmInfo, VGInfo &info);

    static void printVGInfo(const VGInfo &info);
    static void printLVInfo(const LVInfo &info);
    static void printPVInfo(const PVInfo &info);
    static bool initSuport();

    static QString getStringListItem(const QStringList &list, int i);

public:
    static LVM_CMD_Support m_lvmSupport;
    static bool m_initSupport;
};

#endif // LVMOPERATOR_H
