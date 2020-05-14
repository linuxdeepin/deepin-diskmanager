#include "devicelistwidget.h"
#include <DPalette>
#include <QVBoxLayout>
#include <QDebug>
#include "customcontrol/dmdiskinfobox.h"
DeviceListWidget::DeviceListWidget(QWidget *parent): DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::white));
    setPalette(plt);
    setMaximumWidth(360);
    setMinimumWidth(100);
    initUi();
    initConnection();
}

void DeviceListWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);
    m_treeview = new DmTreeview(this);
    layout->addWidget(m_treeview);
}

void DeviceListWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigUpdateDeviceInfo, this, &DeviceListWidget::slotUpdateDeviceInfo);
    connect(m_treeview, &DmTreeview::sigCurSelectChanged, DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged);
}


void DeviceListWidget::slotUpdateDeviceInfo()
{
    //ToDo:
//更新DmTreeview
//设置当前选项
    DeviceInfoMap infomap = DMDbusHandler::instance()->probDeviceInfo();
    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
//        qDebug() << Utils::sector_to_unit(info.sectors, info.sector_size, SIZE_UNIT::UNIT_GIB);

        double_t sectorall = (info.length * info.sector_size) / 1024.0 / 1024.0 / 1024.0;
        QString s_disksize = QString::number(sectorall, 'f', 2) + "GB";
//        QString s_disksize = QString::number(Utils::sector_to_unit(info.sectors, info.sector_size, SIZE_UNIT::UNIT_GIB)) + "GB";
        DmDiskinfoBox *m_box = new DmDiskinfoBox(0, info.m_path, s_disksize);

        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
//            qDebug() << Utils::sector_to_unit(it->sector_end - it->sector_end, it->sector_size, SIZE_UNIT::UNIT_GIB);
            QString s_pdisksize = QString::number(Utils::sector_to_unit(it->sector_end - it->sector_start, it->sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
            QString s_partitionpath = it->path;
            double s_used = (it->sectors_used * it->sector_size) / 1024.0 / 1024.0 / 1024.0;
            qDebug() << it->sectors_used;
            qDebug() << it->sectors_unused;
            //            qDebug() << s_used;

            double s_unused = (it->sectors_unused * it->sector_size) / 1024.0 / 1024.0 / 1024.0;
//            qDebug() << s_unused;
            DmDiskinfoBox *m_childbox = new DmDiskinfoBox(1, "", "", s_partitionpath, s_pdisksize, s_used, s_unused, it->sectors_unallocated,
                                                          it->sector_start, it->sector_end);
//            qDebug() << it->path << it << s_pdisksize;
            m_box->childs.append(m_childbox);
        }

        m_treeview->addTopItem(m_box);
    }
}
