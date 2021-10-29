/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mountinfo.h
 *
 * @brief 挂载点信息类
 *
 * @date 2020-09-04 17:06
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

#ifndef MOUNTINFO_H
#define MOUNTINFO_H
#include "blockspecial.h"
#include "commondef.h"

#include <QMap>

namespace DiskManager {


/**
 * @class MountInfo
 * @brief 挂载点信息类
 */

class MountInfo
{
public:
    typedef QMap<BlockSpecial, MountEntry> MountMapping; //挂载点集合

    /**
     * @brief 加载缓存
     */
    static void loadCache(QString &rootfs);

    /**
     * @brief 是否设备挂载点
     * @param path：设备路径
     * @return true成功false失败
     */
    static bool isDevMounted(const QString &path);

    /**
     * @brief 是否设备挂载点
     * @param blockSpecial：块
     * @return true成功false失败
     */
    static bool isDevMounted(const BlockSpecial &blockSpecial);

    /**
     * @brief 是否只读的设备挂载点
     * @param path：设备路径
     * @return true成功false失败
     */
    static bool isDevMountedReadonly(const QString &path);

    /**
     * @brief 是否只读的设备挂载点
     * @param bs：块
     * @return true成功false失败
     */
    static bool isDevMountedReadonly(const BlockSpecial &blockSpecial);

    /**
     * @brief 获取挂载点信息
     * @param path：设备路径
     * @return 挂载点信息
     */
    static const QVector<QString> &getMountedMountpoints(const QString &path);

    /**
     * @brief 获取文件系统挂载点信息
     * @param path：设备路径
     * @return 文件系统挂载点信息
     */
    static const QVector<QString> &getFileSystemTableMountpoints(const QString &path);

private:
    /**
     * @brief 从文件中读取挂载点信息
     * @param filename：文件名
     * @param map：挂载点信息
     */
    static void readMountpointsFromFile(const QString &fileName, MountMapping &map, QString &rootFs);

    /**
     * @brief 添加挂载点入口
     * @param map：挂载点信息
     * @param node：节点
     * @param mountpoint：挂载点
     * @param readonly：只读标志
     */
    static void addMountpointEntry(MountMapping &map, QString &node, QString &mountPoint, bool readonly);

    /**
     * @brief 解析只读标志
     * @param str：解析只读字符串
     * @return true只读false不只读
     */
    static bool parseReadonlyFlag(const QString &str);

    /**
     * @brief 解析挂载点信息从交换文件
     * @param fileName：文件名
     * @param map：挂载点信息
     */
    static void readMountpointsFromFileSwaps(const QString &fileName, MountMapping &map);

    /**
     * @brief 有root文件系统的设备
     * @param map：挂载点信息
     * @param true成功false失败
     */
    static bool haveRootfsDev(MountMapping &map);

    /**
     * @brief 从“mount”命令读取挂载点信息
     * @param map：挂载点信息
     */
    static void readMountpointsFromMountCommand(MountMapping &map);

    /**
     * @brief 查找挂载点入口
     * @param map：挂载点信息
     * @param path：设备路径
     * @return 挂载点入口
     */
    static const MountEntry &find(const MountMapping &map, const QString &path);
};

} // namespace DiskManager
#endif // MOUNTINFO_H
