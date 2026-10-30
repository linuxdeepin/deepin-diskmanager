// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
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
    qCDebug(appLog) << "CenterWidget initializing...";
    initUi();
}

CenterWidget::~CenterWidget()
{
    qCDebug(appLog) << "CenterWidget destroyed";
}

void CenterWidget::HandleQuit()
{
    qCDebug(appLog) << "Handling CenterWidget quit";
}

TitleWidget *CenterWidget::getTitleWidget()
{
    return m_titleWidget;
}

void CenterWidget::initUi()
{
    qCDebug(appLog) << "Initializing CenterWidget UI";

    m_titleWidget = new TitleWidget(this);
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    m_mainSpliter = new MainSplitter;
    mainlayout->addWidget(m_mainSpliter);

    setLayout(mainlayout);
}
