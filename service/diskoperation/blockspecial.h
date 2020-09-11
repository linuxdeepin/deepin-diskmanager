/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file blockspecial.h
 *
 * @brief 特殊块信息类
 *
 * @date 2020-09-09 13:40
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

#ifndef BLOCKSPECIAL_H
#define BLOCKSPECIAL_H
#include <QString>

namespace DiskManager {

/**
 * @class BlockSpecial
 * @brief 特殊块信息类
 */


class BlockSpecial
{
public:
    BlockSpecial();
    BlockSpecial(const QString &name);
    ~BlockSpecial();

    QString m_name; // E.g. Block special file {"/dev/sda1", 8, 1},
    unsigned long m_major; // plain file {"FILENAME", 0, 0} and empty object
    unsigned long m_minor; // {"", 0, 0}.

    /**
     * @brief 清空缓存信息
     */
    static void clearCache();

    /**
     * @brief 注册特殊块信息
     * @param name：设备名
     * @param major：分区最大数
     * @param minor：分区最小数
     */
    static void registerBlockSpecial(const QString &name,
                                       unsigned long major, unsigned long minor);
};

bool operator==(const BlockSpecial &lhs, const BlockSpecial &rhs);
bool operator<(const BlockSpecial &lhs, const BlockSpecial &rhs);

} // namespace DiskManager
#endif // BLOCKSPECIAL_H
