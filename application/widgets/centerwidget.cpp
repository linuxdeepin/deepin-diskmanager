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
    connect(m_handler, &DMDbusHandler::sigUpdateDeviceInfo, this, &CenterWidget::slotUpdateDeviceInfo);
}

void CenterWidget::slotshowSpinerWindow(bool bshow)
{
    if (bshow) {
        m_bufferwin->Start();
    } else {
        m_bufferwin->Stop();
    }
}

void CenterWidget::slotUpdateDeviceInfo(const DeviceInfoMap &infomap)
{
    qDebug() << "===========";
    slotshowSpinerWindow(false);
    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
        qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
                 << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
                 << info.sector_size << info.max_prims << info.highest_busy << info.readonly
                 << info.max_partition_name_length;
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            qDebug() << __FUNCTION__ << it->device_path << it->partition_number << it->type << it->status << it->alignment << it->fstype << it->uuid
                     << it->name << it->sector_start << it->sector_end << it->sectors_used << it->sectors_unused
                     << it->sectors_unallocated << it->significant_threshold << it->free_space_before
                     << it->sector_size << it->fs_block_size << it->path << it->filesystem_label;
        }


    }
}
