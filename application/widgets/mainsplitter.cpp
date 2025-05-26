// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "mainsplitter.h"
#include "devicelistwidget.h"
#include "infoshowwidget.h"
#include <QDebug>

MainSplitter::MainSplitter(DWidget *parent)
    : DSplitter(parent)
{
    qDebug() << "MainSplitter constructor";
    setFrameShape(QFrame::NoFrame);

    initui();
    initConnection();
}

void MainSplitter::initui()
{
    qDebug() << "Initializing MainSplitter UI";
    setHandleWidth(1);
    setChildrenCollapsible(false); //  子部件不可拉伸到 0

    qDebug() << "Creating device list widget";
    m_deviceList = new DeviceListWidget(this);
    qDebug() << "Creating info show widget";
    m_infoShow = new InfoShowWidget(this);

    setMouseTracking(true);
//    setStretchFactor(1, 1);

    m_infoShow->setFrameShape(QFrame::NoFrame);

    qDebug() << "Adding widgets to splitter";
    addWidget(m_deviceList);
    addWidget(m_infoShow);
    qDebug() << "Setting stretch factors (1:4 ratio)";
    setStretchFactor(0, 1);
    setStretchFactor(1, 4);
}

void MainSplitter::initConnection()
{
}
