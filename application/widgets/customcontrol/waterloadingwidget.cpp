/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file waterloadingwidget.cpp
 *
 * @brief 水滴加载动画窗口
 *
 * @date 2022-03-02 11:24
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
#include "waterloadingwidget.h"

#include <QVBoxLayout>

WaterLoadingWidget::WaterLoadingWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
    initConnection();
}

void WaterLoadingWidget::initUi()
{
    m_waterProgress = new DWaterProgress(this);
    m_waterProgress->setFixedSize(98, 98);
    m_waterProgress->setValue(0);

    m_time = new QTimer(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_waterProgress, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}

void WaterLoadingWidget::initConnection()
{
    connect(m_time, &QTimer::timeout,this, &WaterLoadingWidget::onStartWaterProgress);
}

void WaterLoadingWidget::onStartWaterProgress()
{
    int value = (m_waterProgress->value() + qrand() % 10);
    value > 99 ? value = 99 : value;
    m_waterProgress->setValue(value);
}

void WaterLoadingWidget::setStartTime(int msec)
{
    m_waterProgress->start();
    m_waterProgress->setValue(1);
    m_time->start(msec);
}

void WaterLoadingWidget::stopTimer()
{
    m_waterProgress->setValue(100);
    m_waterProgress->stop();
    m_time->stop();
}


