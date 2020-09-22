/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitiondialog.cpp
 *
 * @brief 是否创建分区选择窗口
 *
 * @date 2020-09-08 14:56
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
#include "partitiondialog.h"
#include "partedproxy/dmdbushandler.h"
#include "partitioninfo.h"

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QDebug>

PartitionDialog::PartitionDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
}

void PartitionDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();

    setTitle(tr("Partition %1").arg(info.m_path));
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    DLabel *tipLabel = new DLabel(tr("It will increase the number of partitions on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    mainLayout->addWidget(tipLabel);

    addButton(tr("Cancel"), false, ButtonNormal);
    m_okCode = addButton(tr("Confirm"), true, ButtonRecommend);
}
