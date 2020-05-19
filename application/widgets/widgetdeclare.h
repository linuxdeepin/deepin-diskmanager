/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     wangzhixuan <wangzhixuan@uniontech.com>
* Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef WIDGETDECLARE_H
#define WIDGETDECLARE_H

#include <QString>
#include <QIcon>

namespace  {
const QString app_name = "deepin-diskmanager";

static QIcon getIcon(const QString &iconName)
{
    return QIcon::fromTheme("dm_" + iconName);
}

}

#endif // WIDGETDECLARE_H
