#include "dmtreemanagerwidget.h"
#include <QDebug>
DmTreeManagerWidget::DmTreeManagerWidget(QWidget *parent) : DWidget(parent)
{

}
void DmTreeManagerWidget::addItem()//等待大帝给参数
{
    QStandardItem *t_item;
    DiskInfoData data;
    data.disksize = "200G";
    data.disklabel = "/dev/sda";
    t_item = m_treeview->addtopitem(data);
    qDebug() << data.disksize << data.disklabel;
    qDebug() << data.disksize << data.disklabel;
}
