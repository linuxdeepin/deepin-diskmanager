// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

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
