/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file gpt_header.h
 *
 * @brief gpt 分区表格式定义
 *
 * @date 2022-02-15 17:49
 *
 * Author: chenenlong  <chenenlong@uniontech.com>
 *
 * Maintainer: chenenlong  <chenenlong@uniontech.com>
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


#ifndef GPT_HEADER_H
#define GPT_HEADER_H

#include <stdint.h>
/* GPT header */
typedef struct gpt_header {
        uint64_t            signature; /* header identification */
        uint32_t            revision; /* header version */
        uint32_t            size; /* in bytes */
        uint32_t            crc32; /* header CRC checksum */
        uint32_t            reserved1; /* must be 0 */
        uint64_t            my_lba; /* LBA of block that contains this struct (LBA 1) */
        uint64_t            alternative_lba; /* backup GPT header */
        uint64_t            first_usable_lba; /* first usable logical block for partitions */
        uint64_t            last_usable_lba; /* last usable logical block for partitions */
} __attribute__ ((packed)) gpt_header_t;




#endif // GPT_HEADER_H
