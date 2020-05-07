#include "centerwidget.h"
#include "titlewidget.h"
#include "mainsplitter.h"
#include "partedproxy/dmdbushandler.h"
#include <QVBoxLayout>

CenterWidget::CenterWidget(DWidget *parent) : Dtk::Widget::DWidget(parent)
{
    m_titlewidget = new TitleWidget;
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);
    m_mainspliter = new MainSplitter;
    mainlayout->addWidget(m_mainspliter);
    setLayout(mainlayout);
//    m_handler = DMDbusHandler::instance(this);
//    m_handler->Start();
    //    m_handler->getDeviceinfo();
    //    QTimer::singleShot(5000, this, [&] {
    //        m_handler->getDeviceinfo();
    //    });
}

void CenterWidget::HandleQuit()
{
    // m_handler->Quit();
}

TitleWidget *CenterWidget::titlewidget()
{
    return m_titlewidget;
}
