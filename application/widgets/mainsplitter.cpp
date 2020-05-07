#include "mainsplitter.h"
#include "devicelistwidget.h"
#include "infoshowwidget.h"

MainSplitter::MainSplitter(DWidget *parent): DSplitter(parent)
{
    initui();
}

void MainSplitter::initui()
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0
    m_devicelist = new DeviceListWidget;
    m_infoshow = new InfoShowWidget;
    addWidget(m_devicelist);
    addWidget(m_infoshow);


}
