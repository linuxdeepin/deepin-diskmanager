/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file fat16.h
 *
 * @brief FAT16(32)格式操作类
 *
 * @date 2021-01-04 14:13
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
#ifndef GPARTED_FAT16_H
#define GPARTED_FAT16_H

#include "filesystem.h"

namespace DiskManager
{

/**
 * @class fat16
 * @brief 文件格式fat32操作类
 */

class FAT16 : public FileSystem
{
    const enum FSType specificType;

public:
    FAT16( enum FSType type ) : specificType( type ) {}

    /**
     * @brief 获取文件系统支持
     * @return 文件系统
     */
    FS getFilesystemSupport();

    /**
     * @brief 设置已用空间
     * @param partition：分区信息
     */
    void setUsedSectors( Partition & partition );

    /**
     * @brief 读取表
     * @param partition：分区信息
     */
    void readLabel( Partition & partition );

    /**
     * @brief 写表
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool writeLabel( const Partition & partition);

    /**
     * @brief 读取UUID
     * @param partition：分区信息
     */
    void readUuid( Partition & partition );

    /**
     * @brief 写UUID
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool writeUuid( const Partition & partition );

    /**
     * @brief 创建
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool create( const Partition & new_partition);

    /**
     * @brief 检查修补
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool checkRepair( const Partition & partition);

    /**
     * @brief 获取文件系统限制
     * @param partition：分区信息
     * @return 文件系统限制信息
     */
    virtual FS_Limits getFilesystemLimits(const Partition &partition) const override;

};

} //DiskManager

#endif /* GPARTED_FAT16_H */
