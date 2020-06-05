#include "bufferwin.h"
#include <DWidgetUtil>
#include <QVBoxLayout>

BufferWin::BufferWin(DWidget *parent)
    : DWidget(parent)
{
    setWindowFlags(/*Qt::FramelessWindowHint | Qt::Tool*/ Qt::SplashScreen);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setGeometry(100, 100, 100, 100);
    initUi();
}

void BufferWin::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_spinner = new DSpinner(this);
    layout->addWidget(m_spinner);
}

void BufferWin::Start()
{
    m_spinner->start();
    Dtk::Widget::moveToCenter(this);
    show();
}

void BufferWin::Stop()
{
    m_spinner->stop();
    hide();
}
