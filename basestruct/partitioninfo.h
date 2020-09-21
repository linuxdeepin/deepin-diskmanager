/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitioninfo.h
 *
 * @brief 分区信息类
 *
 * @date 2020-09-21 14:48
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

#ifndef PARTITIONINFO_H
#define PARTITIONINFO_H
#include "commondef.h"

#include <QString>

/**
 * @class PartitionInfo
 * @brief 分区信息类
 */

class QDBusArgument;

class PartitionInfo
{
public:
    PartitionInfo();

    /**
     * @brief 获取扇区长度
     * @return 扇区长度
     */
    Sector getSectorLength() const;

    /**
     * @brief 获取字节长度
     * @return 字节长度
     */
    Byte_Value getByteLength() const;

    bool operator==(const PartitionInfo &info) const;

public:

    QString m_devicePath;
    int m_partitionNumber;
    int type; //PartitionType: UNALLOCATED, PRIMARY, LOGICAL, etc...
    int status; //PartitionStatus: STAT_REAL, STAT_NEW, etc..
    int alignment; //PartitionAlignment: ALIGN_CYLINDER, ALIGN_STRICT, etc
    int fstype;
    int flag;     //hidden flag
    QString uuid;
    QString name;
    Sector sector_start;
    Sector sector_end;
    Sector sectors_used;
    Sector sectors_unused;
    Sector sectors_unallocated; //Difference between the size of the partition and the file system
    Sector significant_threshold; //Threshold from intrinsic to significant unallocated sectors
    Sector free_space_before; //Free space preceding partition value
    Byte_Value sector_size; //Sector size of the disk device needed for converting to/from sectors and bytes.
    Byte_Value fs_block_size; // Block size of of the file system, or -1 when unknown.
    QString path; //partition path
    QString filesystem_label;
    bool inside_extended;
    bool busy;
    bool fs_readonly;
    QVector<QString> mountpoints;
};
Q_DECLARE_METATYPE(PartitionInfo)
QDBusArgument &operator<<(QDBusArgument &argument, const PartitionInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, PartitionInfo &info);

typedef QVector<PartitionInfo> PartitionVec;
Q_DECLARE_METATYPE(PartitionVec)

#endif // PARTITIONINFO_H
