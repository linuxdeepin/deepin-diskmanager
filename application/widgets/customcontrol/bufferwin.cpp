/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file bufferwin.cpp
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
#include "bufferwin.h"

#include <DWidgetUtil>

#include <QVBoxLayout>

BufferWin::BufferWin(DWidget *parent)
    : DWidget(parent)
{
    setWindowFlags(/*Qt::FramelessWindowHint | Qt::Tool*/ Qt::SplashScreen);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setGeometry(100, 100, 100, 100);

    initUi();
}

void BufferWin::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_spinner = new DSpinner(this);
    layout->addWidget(m_spinner);
}

void BufferWin::Start()
{
    m_spinner->start();
    Dtk::Widget::moveToCenter(this);
    show();
}

void BufferWin::Stop()
{
    m_spinner->stop();
    hide();
}
