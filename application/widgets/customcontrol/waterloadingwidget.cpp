// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "waterloadingwidget.h"

#include <QVBoxLayout>
#if QT_VERSION_MAJOR > 5
#include <random>
#endif
#include <QDebug>

WaterLoadingWidget::WaterLoadingWidget(QWidget *parent) : QWidget(parent)
{
    qDebug() << "WaterLoadingWidget constructor";
    initUi();
    initConnection();
}

void WaterLoadingWidget::initUi()
{
    qDebug() << "WaterLoadingWidget::initUi called.";
    m_waterProgress = new DWaterProgress(this);
    m_waterProgress->setFixedSize(98, 98);
    m_waterProgress->setValue(0);

    m_time = new QTimer(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_waterProgress, 0, Qt::AlignCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    qDebug() << "UI initialized";
}

void WaterLoadingWidget::initConnection()
{
    qDebug() << "WaterLoadingWidget::initConnection called.";
    connect(m_time, &QTimer::timeout,this, &WaterLoadingWidget::onStartWaterProgress);
}

void WaterLoadingWidget::onStartWaterProgress()
{
    qDebug() << "WaterLoadingWidget::onStartWaterProgress called.";
#if QT_VERSION_MAJOR > 5
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 9);

    int value = (m_waterProgress->value() + distrib(gen));
    qDebug() << "QT_VERSION_MAJOR > 5, generated value:" << value;
#else
    int value = (m_waterProgress->value() + qrand() % 10);
    qDebug() << "QT_VERSION_MAJOR <= 5, generated value:" << value;
#endif
    value > 99 ? value = 99 : value;
    m_waterProgress->setValue(value);
    qDebug() << "Water progress value set to:" << value;
}

void WaterLoadingWidget::setStartTime(int msec)
{
    qDebug() << "Starting water progress with interval:" << msec << "ms";
    m_waterProgress->start();
    m_waterProgress->setValue(1);
    m_time->start(msec);
}

void WaterLoadingWidget::stopTimer()
{
    qDebug() << "Stopping water progress";
    m_waterProgress->setValue(100);
    m_waterProgress->stop();
    m_time->stop();
}


