/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file ddbase.cpp
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
#include "ddbase.h"
#include "common.h"

#include <QKeyEvent>

DDBase::DDBase(QWidget *parent)
    : DDialog(parent)
{
    setModal(true);
    setIcon(QIcon::fromTheme(appName));

    m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameRounded(false);
    m_mainFrame->setFrameStyle(DFrame::NoFrame);

    addContent(m_mainFrame);
    // updateGeometry();
}

void DDBase::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Escape) {
        event->ignore();
    } else {
        DDialog::keyPressEvent(event);
    }
}
