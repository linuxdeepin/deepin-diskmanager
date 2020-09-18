/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file bufferwin.h
 *
 * @brief 旋转动画类
 *
 * @date 2020-09-18 14:32
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
#ifndef BUFFERWIN_H
#define BUFFERWIN_H

#include <DSpinner>
#include <DWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class BufferWin
 * @brief 旋转动画类
*/

class BufferWin : public DWidget
{
    Q_OBJECT
public:
    explicit BufferWin(DWidget *parent = nullptr);

    /**
     * @brief 开启动画
    */
    void Start();

    /**
     * @brief 停止动画
    */
    void Stop();

private:

    /**
     * @brief 初始化界面
    */
    void initUi();

private:
    DSpinner *m_spinner = nullptr;
};

#endif // BUFFERWIN_H
