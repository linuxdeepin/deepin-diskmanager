#include "dmtreeview.h"
#include <QDebug>
#include "dmtreemanagerwidget.h"
DmTreeview::DmTreeview(QWidget *parent) : DTreeView(parent)
{
    initUI();
    initmodel();
    initdelegate();
    QStandardItem *t_item;
    DiskInfoData data;
    data.disksize = "200G";
    data.disklabel = "/dev/sda";
    qDebug() << data.level;
    t_item = this->addtopitem(data);
    qDebug() << data.level;
    t_item = this->addtopitem(data);
//    this->additem(t_item, data);
    qDebug() << data.level;
//    this->addtopitem(data);
    qDebug() << data.level;
//    this->addtopitem(data);
    qDebug() << data.disksize << data.disklabel;
    qDebug() << data.disksize << data.disklabel;
}
void DmTreeview::initUI()
{
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setVerticalScrollMode(ScrollPerItem);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHeaderHidden(true);
    setIndentation(0);                   //去除树型节点之间的缩进
    setWindowFlags(Qt::FramelessWindowHint); //无边框
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // setAttribute(Qt::WA_TranslucentBackground);//背景透明
    qDebug() << 2222222222222;
}
void DmTreeview::additem(QStandardItem *t_item, DiskInfoData &data)
{
    QVariant var = t_item->index().data(Qt::UserRole + 1);
    DiskInfoData parent_data = var.value<DiskInfoData>();
    data.level = parent_data.level + 1;
    qDebug() << data.level;
    QStandardItem *c_item = new QStandardItem(data.disklabel);
    t_item->setData(QVariant::fromValue((data)), Qt::UserRole + 1);
    t_item->appendRow(c_item);
    qDebug() << data.disksize << data.disklabel;
    // expand(m_pDataModel->indexFromItem(item));
//    setExpanded(m_model->indexFromItem(c_item), true);
}
QStandardItem *DmTreeview::addtopitem(DiskInfoData &data)
{
    data.level = 0;
    qDebug() << data.level;
    QStandardItem *t_item = new QStandardItem(data.disklabel);
    t_item->setData(QVariant::fromValue(data), Qt::UserRole + 1);
    m_model->appendRow(t_item);
    qDebug() << data.disksize << data.disklabel << "top";
    return  t_item;
}
void DmTreeview::initmodel()
{
    m_model = new QStandardItemModel(this);
    qDebug() << "111111111111";
    this->setModel(m_model);
    qDebug() << "33333333333333";
}
void DmTreeview::initdelegate()
{
    m_delegate = new DmTreeviewDelegate(this);
    this->setItemDelegate(m_delegate);


}


