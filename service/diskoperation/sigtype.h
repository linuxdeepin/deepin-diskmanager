/**
 * @copyright 2021-2021 Uniontech Technology Co., Ltd.
 *
 * @file sigtype.h
 *
 * @brief 硬件刷新后发送信号代码定义
 *
 * @date 2021-06-24 13:56
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
#ifndef __SIGNAL_TYPE__H__
#define __SIGNAL_TYPE__H__


#define DISK_SIGNAL_TYPE_UMNT 1             //卸载信号
#define DISK_SIGNAL_TYPE_DEL  2             //删除信号
#define DISK_SIGNAL_TYPE_SHOW 3             //显示信号
#define DISK_SIGNAL_TYPE_CREATE_TABLE 4     //清楚分区表信号
#define DISK_SIGNAL_USBUPDATE     5         //usb刷新信号
#define DISK_SIGNAL_TYPE_CLEAR 6            //磁盘或分区清除动作信号
#define DISK_SIGNAL_TYPE_VGCREATE 7         //vg创建信号
#define DISK_SIGNAL_TYPE_PVDELETE 8         //pv删除信号
#define DISK_SIGNAL_TYPE_VGDELETE 9         //vg删除信号
#define DISK_SIGNAL_TYPE_LVDELETE 10        //lv删除信号
#define DISK_SIGNAL_TYPE_DECRYPT 11         //解密
#define DISK_SIGNAL_TYPE_CREATE_FAILED 12   //新建失败

#define DISK_SIGNAL_TYPE_AUTOMNT 100        //自动挂载



#endif
