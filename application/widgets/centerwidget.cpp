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
    m_handler =  DMDbusHandler::instance(this);
    initUi();
    initConnection();
    m_handler->getDeviceinfo();
}

CenterWidget::~CenterWidget()
{
    if (nullptr != m_bufferwin) {
        m_bufferwin->deleteLater();
    }
}

void CenterWidget::HandleQuit()
{
    m_handler->Quit();
}

TitleWidget *CenterWidget::titlewidget()
{
    return m_titlewidget;
}

void CenterWidget::initUi()
{
    m_bufferwin = new BufferWin;
    m_titlewidget = new TitleWidget;
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setContentsMargins(0, 0, 0, 0);
    mainlayout->setSpacing(0);

    m_mainspliter = new MainSplitter;
    mainlayout->addWidget(m_mainspliter);


    setLayout(mainlayout);
}

void CenterWidget::initConnection()
{
    connect(m_handler, &DMDbusHandler::sigShowSpinerWindow, this, &CenterWidget::slotshowSpinerWindow);
}

void CenterWidget::slotshowSpinerWindow(bool bshow)
{
    if (bshow) {
        m_bufferwin->Start();
    } else {
        m_bufferwin->Stop();
    }
}
