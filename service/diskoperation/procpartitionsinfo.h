/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file procpartitionsinfo.h
 *
 * @brief 程序分区信息类
 *
 * @date 2020-09-04 11:40
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

#ifndef PROCPARTITIONSINFO_H
#define PROCPARTITIONSINFO_H
#include <QVector>
#include <QString>

namespace DiskManager {

/**
 * @class ProcPartitionsInfo
 * @brief 程序分区信息类
 */

class ProcPartitionsInfo
{
public:
    /**
     * @brief 加载分区信息缓存
     */
    static void loadCache();

    /**
     * @brief 获取设备路径
     * @return 设备路径表
     */
    static const QVector<QString> &getDevicePaths();

private:
    /**
     * @brief 是否需要初始化
     */
    static void initializeIfRequired();

    /**
     * @brief 加载进程分区信息缓存
     */
    static void loadProcPartitionsInfoCache();

    static bool procPartitionsInfoCacheInitialized;     //进程分区信息缓存初始化状态
    static QVector<QString> devicePathsCache;             //设备路径缓存
};

} // namespace DiskManager
#endif // PROCPARTITIONSINFO_H
