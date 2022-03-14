/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file filesystem.h
 *
 * @brief 文件系统类
 *
 * @date 2020-09-09 14:43
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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "commondef.h"
#include "../partition.h"

#include <QtMath>

namespace DiskManager {

/**
 * @class FileSystem
 * @brief 文件系统类
 */

class FileSystem
{
public:
    FileSystem();
    virtual ~FileSystem() {}

    /**
     * @brief 获取文件系统支持
     * @return 文件系统
     */
    virtual FS getFilesystemSupport() = 0;

    /**
     * @brief 文件系统繁忙状态
     * @param
     * @return true成功false失败
     */
    virtual bool isBusy(const QString &) { return false; }

    /**
     * @brief 读取表
     * @param partition：分区信息
     */
    virtual void readLabel(Partition &) {}

    /**
     * @brief 写表
     * @param partition：分区信息
     * @return true成功false失败
     */
    virtual bool writeLabel(const Partition &) { return false; }

    /**
     * @brief 读取UUID
     * @param partition：分区信息
     */
    virtual void readUuid(Partition &) {}

    /**
     * @brief 写UUID
     * @param partition：分区信息
     * @return true成功false失败
     */
    virtual bool writeUuid(const Partition &) { return false; }

    /**
     * @brief 设置已用空间
     * @param partition：分区信息
     */
    virtual void setUsedSectors(Partition &) {}

    /**
     * @brief 获取文件系统限制
     * @param partition：分区信息
     * @return 文件系统限制信息
     */
    virtual FS_Limits getFilesystemLimits(const Partition &) { return m_fsLimits; }



    /**
     * @brief 获取文件系统限制
     * @param path：设备路径
     * @return 文件系统限制信息
     */
    virtual FS_Limits getFilesystemLimits(const QString &) { return m_fsLimits; }

    /**
     * @brief 创建
     * @param partition：分区信息
     * @return true成功false失败
     */
    virtual bool create(const Partition &) { return false; }

    /**
     * @brief 扩容
     * @param partition：分区信息
     * @param
     * @return true成功false失败
     */
    virtual bool resize(const Partition &, bool) { return false; }



    /**
     * @brief 调整大小
     * @param path：设备路径
     * @param size：调整后大小
     * @param fill_partition：标记位
     * @return true成功false失败
     */
    virtual bool resize(const QString &, const QString &, bool)  { return false; }


    /**
     * @brief 检查修补
     * @param partition：分区信息
     * @return true成功false失败
     */
    virtual bool checkRepair(const Partition &) { return false; }

    /**
     * @brief 检查修补
     * @param path：设备路径
     * @return true成功false失败
     */
    virtual bool checkRepair(const QString &) { return false; }

    Sector m_totalNumOfBlock, m_numOfFreeOrUsedBlocks, m_blocksSize; //File system [T]otal num of blocks, [N]um of free (or used) blocks, block [S]ize
public:
    FS_Limits m_fsLimits;

private:
};
} // namespace DiskManager
#endif // FILESYSTEM_H
