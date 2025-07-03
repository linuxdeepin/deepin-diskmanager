// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "common.h"
#include <QDebug>

QIcon Common::getIcon(const QString &iconName)
{
    // qDebug() << "Common::getIcon called with iconName:" << iconName;
    return QIcon::fromTheme("dm_" + iconName);
}

int Common::getLabelAdjustHeight(const int &width, const QString &text, const QFont &font)
{
    // qDebug() << "Calculating label height for text:" << text.left(20) << "...";
    DLabel label;
    label.setFont(font);
    label.setWordWrap(true);
    label.setFixedWidth(width);
    label.setText(text);
    label.adjustSize();

    int height = label.height();
    // qDebug() << "Calculated label height:" << height;
    return height;
}
