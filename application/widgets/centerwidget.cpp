/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file centerwidget.cpp
 *
 * @brief 主界面类
 *
 * @date 2020-09-04 11:41
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
#include "centerwidget.h"
#include "titlewidget.h"
#include "mainsplitter.h"
#include "partedproxy/dmdbushandler.h"

#include <DSpinner>
#include <DWidgetUtil>

#include <QVBoxLayout>
#include <QThread>

CenterWidget::CenterWidget(DWidget *parent)
    : Dtk::Widget::DWidget(parent)
{
    initUi();
}

CenterWidget::~CenterWidget()
{
}

void CenterWidget::HandleQuit()
{
}

TitleWidget *CenterWidget::getTitleWidget()
{
    return m_titleWidget;
}

void CenterWidget::initUi()
{
    m_titleWidget = new TitleWidget(this);
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    m_mainSpliter = new MainSplitter;
    mainlayout->addWidget(m_mainSpliter);

    setLayout(mainlayout);
}
