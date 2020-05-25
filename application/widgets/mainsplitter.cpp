#include "mainsplitter.h"
#include "devicelistwidget.h"
#include "infoshowwidget.h"

MainSplitter::MainSplitter(DWidget *parent): DSplitter(parent)
{
    setFrameShape(QFrame::NoFrame);

    initui();
    initConnection();
}

void MainSplitter::initui()
{
    setHandleWidth(1);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0
    m_devicelist = new DeviceListWidget(this);
    m_infoshow = new InfoShowWidget(this);
    m_infoshow->setFrameShape(QFrame::NoFrame);
    addWidget(m_devicelist);
    addWidget(m_infoshow);
}

void MainSplitter::initConnection()
{

}
