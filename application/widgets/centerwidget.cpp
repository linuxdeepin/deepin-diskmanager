// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


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
    qDebug() << "CenterWidget initializing...";
    initUi();
}

CenterWidget::~CenterWidget()
{
    qDebug() << "CenterWidget destroyed";
}

void CenterWidget::HandleQuit()
{
    qDebug() << "Handling CenterWidget quit";
}

TitleWidget *CenterWidget::getTitleWidget()
{
    return m_titleWidget;
}

void CenterWidget::initUi()
{
    qDebug() << "Initializing CenterWidget UI";

    m_titleWidget = new TitleWidget(this);
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    m_mainSpliter = new MainSplitter;
    mainlayout->addWidget(m_mainSpliter);

    setLayout(mainlayout);
}
