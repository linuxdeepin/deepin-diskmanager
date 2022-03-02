/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file ext2.h
 *
 * @brief 文件格式ext操作类
 *
 * @date 2020-09-15 13:52
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

#ifndef EXT2_H
#define EXT2_H
#include "filesystem.h"

namespace DiskManager {

/**
 * @class EXT2
 * @brief 文件格式ext操作类
 */

class EXT2 : public FileSystem
{
public:
    EXT2(FSType type);

    /**
     * @brief 获取文件系统支持
     * @return 文件系统
     */
    FS getFilesystemSupport() override;

    /**
     * @brief 设置已用空间
     * @param partition：分区信息
     */
    void setUsedSectors(Partition &partition) override;

    /**
     * @brief 读取表
     * @param partition：分区信息
     */
    void readLabel(Partition &partition) override;

    /**
     * @brief 写表
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool writeLabel(const Partition &partition) override;

    /**
     * @brief 读取UUID
     * @param partition：分区信息
     */
    void readUuid(Partition &partition) override;

    /**
     * @brief 写UUID
     * @param partition：分区信息
     * @return true成功false失败
     */
    virtual bool writeUuid(const Partition &partition) override;

    /**
     * @brief 创建
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool create(const Partition &new_partition) override;

    /**
     * @brief 调整大小
     * @param partition：分区信息
     * @param fill_partition：标记位
     * @return true成功false失败
     */
    bool resize(const Partition &partitionNew, bool fillPartition) override;


    /**
     * @brief 调整大小
     * @param path：设备路径
     * @param size：调整后大小
     * @param fill_partition：标记位
     * @return true成功false失败
     */
    bool resize(const QString &path,const QString &size, bool fillPartition) override;

    /**
     * @brief 检查修补
     * @param partition：分区信息
     * @return true成功false失败
     */
    bool checkRepair(const Partition &partition) override;

    /**
     * @brief 检查修补
     * @param path：设备路径
     * @return true成功false失败
     */
    bool checkRepair(const QString &devpath) override;


    /**
     * @brief 获取文件系统限制
     * @param partition：分区信息
     * @return 文件系统限制信息
     */
    virtual FS_Limits getFilesystemLimits(const Partition &partition) const override;


    /**
     * @brief 获取文件系统限制
     * @param path：设备路径
     * @return 文件系统限制信息
     */
    virtual FS_Limits getFilesystemLimits(const QString &path) const override;

private:


    Byte_Value m_fsBlockSize; // Holds file system block size for the copy_progress() callback
    bool m_forceAuto64bit;
    const FSType m_specificType;
    QString m_mkfsCmd;
};

} // namespace DiskManager

#endif // EXT2_H
