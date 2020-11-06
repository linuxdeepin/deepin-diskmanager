/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     AaronZhang <ya.zhang@archermind.com>
 *
 * Maintainer: AaronZhang <ya.zhang@archermind.com>
 * Maintainer: Yaobin <yao.bin@archermind.com>
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

#pragma once

#include <QMap>
#include <QString>
#include <QObject>
#include <QSet>
#include <QProcess>

class QDeviceMouse;

class DeviceInfoParser: public QObject
{
    Q_OBJECT
public:

    /**
     * @brief 单例对象
     * @return 返回单例对象
     */
    static DeviceInfoParser &instance()
    {
        static DeviceInfoParser instance;
        return instance;
    }

    DeviceInfoParser();
    ~DeviceInfoParser();

public:
    /**
     * @brief 获取root密码
     * @return true成功false失败
     */
    bool getRootPassword();

private:
    /**
     * @brief 调用外部程序
     * @param cmd 程序名称
     * @return true成功false失败
     */
    bool executeProcess(const QString &cmd);

    /**
     * @brief 执行终端命令
     * @param cmd 终端命令
     * @return true成功false失败
     */
    bool runCmd(const QString &cmd);

    /**
     * @brief 执行终端命令列表
     * @param cmdList 终端命令列表
     * @return true成功false失败
     */
    bool runCmd(const QStringList &cmdList);

    QString m_standOutput;
};

