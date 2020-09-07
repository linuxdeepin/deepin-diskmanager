/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file centerwidget.h
 *
 * @brief 主界面类
 *
 * @date 2020-09-04 11:38
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
#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include "partedproxy/dmdbushandler.h"
#include "devicelistwidget.h"

#include <DWidget>

DWIDGET_USE_NAMESPACE

class TitleWidget;
class MainSplitter;

/**
 * @class CenterWidget
 * @brief 主界面类
 */

class CenterWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(DWidget *parent = nullptr);
    ~CenterWidget();

public:

    /**
     * @brief 退出服务
     */
    void HandleQuit();

    /**
     * @brief 获取功能图标按钮窗口
     * @return 返回窗口指针
     */
    TitleWidget *getTitleWidget();

private:

    /**
     * @brief 初始化页面
     */
    void initUi();

private:
    TitleWidget *m_titleWidget;
    MainSplitter *m_mainSpliter;
};

#endif // CENTERWIDGET_H
