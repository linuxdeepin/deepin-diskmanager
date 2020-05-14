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
    add();
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
void DeviceListWidget::add()
{
//    DmDiskinfoBox *m_box = new DmDiskinfoBox();
//    m_box->m_diskpath = "/dev/sda";
//    m_box->m_disksize = "200GB";
//    m_box->m_level = 0;
//    DmDiskinfoBox *m_childbox = new DmDiskinfoBox() ;
//    m_childbox->m_level = 1;
//    m_childbox->m_disksize = "100GB";
//    m_childbox->m_diskpath = "/dev/sda1";
//    m_box->childs.append(m_childbox);
//    DmDiskinfoBox *m_childbox1 = new DmDiskinfoBox() ;
//    m_childbox1->m_level = 1;
//    m_childbox1->m_disksize = "150GB";
//    m_childbox1->m_diskpath = "/dev/sda2";
//    m_box->childs.append(m_childbox1);
//    m_treeview->addTopItem(m_box);
//    DmDiskinfoBox *m_box1 = new DmDiskinfoBox();
//    m_box1->m_diskpath = "/dev/sdb";
//    m_box1->m_disksize = "300GB";
//    m_box1->m_level = 0;
//    DmDiskinfoBox *m_childbox2 = new DmDiskinfoBox() ;
//    m_childbox2->m_level = 1;
//    m_childbox2->m_disksize = "4000GB";
//    m_childbox2->m_diskpath = "/dev/sdb1";
//    m_box1->childs.append(m_childbox2);
//    m_treeview->addTopItem(m_box1);
//    DmDiskinfoBox *m_box2 = new DmDiskinfoBox();
//    m_box2->m_diskpath = "/dev/sdc";
//    m_box2->m_disksize = "350GB";
//    m_box2->m_level = 0;
//    DmDiskinfoBox *m_childbox3 = new DmDiskinfoBox() ;
//    m_childbox3->m_level = 1;
//    m_childbox3->m_disksize = "4000GB";
//    m_childbox3->m_diskpath = "/dev/sdb1";
//    m_box2->childs.append(m_childbox3);
//    DmDiskinfoBox *m_childbox4 = new DmDiskinfoBox() ;
//    m_childbox4->m_level = 1;
//    m_box2->childs.append(m_childbox4);
//    DmDiskinfoBox *m_childbox5 = new DmDiskinfoBox() ;
//    m_childbox5->m_level = 1;
//    m_box2->childs.append(m_childbox5);
//    DmDiskinfoBox *m_childbox6 = new DmDiskinfoBox() ;
//    m_childbox6->m_level = 1;
//    m_box2->childs.append(m_childbox6);
//    DmDiskinfoBox *m_childbox7 = new DmDiskinfoBox() ;
//    m_childbox7->m_level = 1;
//    m_box2->childs.append(m_childbox7);
//    DmDiskinfoBox *m_childbox8 = new DmDiskinfoBox() ;
//    m_childbox8->m_level = 1;
//    m_box2->childs.append(m_childbox8);
//    DmDiskinfoBox m_childbox9;
//    m_childbox9.m_level = 1;
//    // m_box2->childs.append(m_childbox9);

//    m_treeview->addTopItem(m_box2);
}

void DeviceListWidget::slotUpdateDeviceInfo()
{
    //ToDo:
//更新DmTreeview
//设置当前选项
    DeviceInfoMap infomap = DMDbusHandler::instance()->probDeviceInfo();

}

void DeviceListWidget::slotUpdateLwInfo(const DeviceInfoMap &infomap)
{

//    qDebug() << "===========";
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
            DmDiskinfoBox *m_childbox = new DmDiskinfoBox(1, it->path, s_pdisksize);
//            qDebug() << it->path << it << s_pdisksize;
            m_box->childs.append(m_childbox);
        }

        m_treeview->addTopItem(m_box);
    }
//        DmDiskinfoBox *m_childbox = new DmDiskinfoBox() ;
//        m_childbox->level = 1;
//        m_childbox->disksize = "100GB";
//        m_childbox->disklabel = "/dev/sda1";
//        m_box->childs.append(m_childbox);
//        DmDiskinfoBox *m_childbox1 = new DmDiskinfoBox() ;
//        m_childbox1->level = 1;
//        m_childbox1->disksize = "150GB";
//        m_childbox1->disklabel = "/dev/sda2";
//        m_box->childs.append(m_childbox1);
//        m_treeview->addTopItem(m_box);
//        DmDiskinfoBox *m_box1 = new DmDiskinfoBox();
//        m_box1->disklabel = "/dev/sdb";
//        m_box1->disksize = "300GB";
//        m_box1->level = 0;

//    qDebug() << __FUNCTION__ << info.m_path << info.length << info.heads << info.sectors
//             << info.cylinders << info.cylsize << info.model << info.serial_number << info.disktype
//             << info.sector_size << info.max_prims << info.highest_busy << info.readonly
//             << info.max_partition_name_length;
//    for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
//        qDebug() << __FUNCTION__ << it->device_path << it->partition_number << it->type << it->status << it->alignment << it->fstype << it->uuid
//                 << it->name << it->sector_start << it->sector_end << it->sectors_used << it->sectors_unused
//                 << it->sectors_unallocated << it->significant_threshold << it->free_space_before
//                 << it->sector_size << it->fs_block_size << it->path << it->filesystem_label;
//    }



}
