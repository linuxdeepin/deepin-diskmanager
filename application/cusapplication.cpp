/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file cusapplication.cpp
 *
 * @brief 自定义Application类
 *
 * @date 2020-09-18 15:17
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
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
#include "cusapplication.h"

CusApplication::CusApplication(int &argc, char **argv)
    : DApplication(argc, argv)
{
}

void CusApplication::handleQuitAction()
{
    emit handleQuitActionChanged();
    DApplication::handleQuitAction();
}
