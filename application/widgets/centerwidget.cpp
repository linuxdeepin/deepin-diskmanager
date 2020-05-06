#include "centerwidget.h"
#include "partedproxy/dmdbushandler.h"

CenterWidget::CenterWidget(QWidget *parent) : Dtk::Widget::DWidget(parent)
{
    m_handler = DMDbusHandler::instance(this);
    m_handler->Start();
    //    m_handler->getDeviceinfo();
    //    QTimer::singleShot(5000, this, [&] {
    //        m_handler->getDeviceinfo();
    //    });
}

void CenterWidget::HandleQuit()
{
    m_handler->Quit();
}
