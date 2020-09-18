/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file ddbase.h
 *
 * @brief 基础窗口类
 *
 * @date 2020-09-18 14:27
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
#ifndef DDBASE_H
#define DDBASE_H

#include <DDialog>
#include <DFrame>

#include <QAbstractButton>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

/**
 * @class DDBase
 * @brief 基础窗口类
*/

class DDBase : public DDialog
{
    Q_OBJECT
public:
    explicit DDBase(QWidget *parent = nullptr);

    DFrame *m_mainFrame = nullptr;
    int m_okCode = -1;
};

#endif // DDBASE_H
