#include "centerwidget.h"
#include "titlewidget.h"
#include "mainsplitter.h"
#include "customcontrol/bufferwin.h"
#include "partedproxy/dmdbushandler.h"
#include <QVBoxLayout>
#include <DSpinner>
#include <DWidgetUtil>
#include <QThread>

CenterWidget::CenterWidget(DWidget *parent) : Dtk::Widget::DWidget(parent)
{
    initUi();
}

CenterWidget::~CenterWidget()
{

}

void CenterWidget::HandleQuit()
{

}

TitleWidget *CenterWidget::titlewidget()
{
    return m_titlewidget;
}

void CenterWidget::initUi()
{
    m_titlewidget = new TitleWidget(this);
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    m_mainspliter = new MainSplitter;
    mainlayout->addWidget(m_mainspliter);


    setLayout(mainlayout);
}


