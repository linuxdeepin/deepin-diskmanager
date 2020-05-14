#include "devicelistwidget.h"
#include "partedproxy/dmdbushandler.h"
#include <DPalette>
#include <QVBoxLayout>
#include <QDebug>

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
}
void DeviceListWidget::add()
{
    DmDiskinfoBox *m_box = new DmDiskinfoBox();
    m_box->disklabel = "/dev/sda";
    m_box->disksize = "200GB";
    m_box->level = 0;
    DmDiskinfoBox *m_childbox = new DmDiskinfoBox() ;
    m_childbox->level = 1;
    m_childbox->disksize = "100GB";
    m_childbox->disklabel = "/dev/sda1";
    m_box->childs.append(m_childbox);
    DmDiskinfoBox *m_childbox1 = new DmDiskinfoBox() ;
    m_childbox1->level = 1;
    m_childbox1->disksize = "150GB";
    m_childbox1->disklabel = "/dev/sda2";
    m_box->childs.append(m_childbox1);
    m_treeview->addTopItem(m_box);
    DmDiskinfoBox *m_box1 = new DmDiskinfoBox();
    m_box1->disklabel = "/dev/sdb";
    m_box1->disksize = "300GB";
    m_box1->level = 0;
    DmDiskinfoBox *m_childbox2 = new DmDiskinfoBox() ;
    m_childbox2->level = 1;
    m_childbox2->disksize = "4000GB";
    m_childbox2->disklabel = "/dev/sdb1";
    m_box1->childs.append(m_childbox2);
    m_treeview->addTopItem(m_box1);
    DmDiskinfoBox *m_box2 = new DmDiskinfoBox();
    m_box2->disklabel = "/dev/sdc";
    m_box2->disksize = "350GB";
    m_box2->level = 0;
    DmDiskinfoBox *m_childbox3 = new DmDiskinfoBox() ;
    m_childbox3->level = 1;
    m_childbox3->disksize = "4000GB";
    m_childbox3->disklabel = "/dev/sdb1";
    m_box2->childs.append(m_childbox3);
    DmDiskinfoBox *m_childbox4 = new DmDiskinfoBox() ;
    m_childbox4->level = 1;
    m_box2->childs.append(m_childbox4);
    DmDiskinfoBox *m_childbox5 = new DmDiskinfoBox() ;
    m_childbox5->level = 1;
    m_box2->childs.append(m_childbox5);
    DmDiskinfoBox *m_childbox6 = new DmDiskinfoBox() ;
    m_childbox6->level = 1;
    m_box2->childs.append(m_childbox6);
    DmDiskinfoBox *m_childbox7 = new DmDiskinfoBox() ;
    m_childbox7->level = 1;
    m_box2->childs.append(m_childbox7);
    DmDiskinfoBox *m_childbox8 = new DmDiskinfoBox() ;
    m_childbox8->level = 1;
    m_box2->childs.append(m_childbox8);
//    DmDiskinfoBox m_childbox9;
//    m_childbox9.level = 1;
//    m_box2->childs.append(m_childbox9);
    m_treeview->addTopItem(m_box2);
}

void DeviceListWidget::slotUpdateDeviceInfo()
{
    //ToDo:
//更新DmTreeview
//设置当前选项
}
