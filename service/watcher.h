/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file thread.h
 *
 * @brief 坏道检测修复线程类
 *
 * @date 2021-01-04 13:56
 *
 * Author: liweigang  <liweigang@uniontech.com>
 *
 * Maintainer: liweigang  <liweigang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef WATCHER_H
#define WATCHER_H

#include <QObject>
#include <QThread>

namespace DiskManager {

/**
 * @class ProbeThread
 * @brief 硬件信息刷新类
 */
class Watcher :public QThread
{
    Q_OBJECT
public:

    /**
     * @brief 监测是前端是否启动又退出
     */
    void run();

    void executCmd(const QString &strCmd, QString &outPut, QString &error);
};
}
#endif // WATCHER_H
