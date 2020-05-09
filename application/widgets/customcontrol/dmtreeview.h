#ifndef DMTREEVIEW_H
#define DMTREEVIEW_H

#include <DTreeView>
#include <QStandardItemModel>
#include <QWidget>
#include "dmtreeviewdelegate.h"
////#include "dmtreemanagerwidget.h"
//#include "dmtreemanagerwidget.h"
DWIDGET_USE_NAMESPACE

class DmTreeview : public DTreeView
{
    Q_OBJECT
public:
    explicit DmTreeview(QWidget *parent = nullptr);
    void initUI();
    void initmodel();
    void initdelegate();
    void additem(QStandardItem *t_item, DiskInfoData &data);
    QStandardItem *addtopitem(DiskInfoData &data);
signals:

public slots:
private:
    QStandardItemModel *m_model;
    QAbstractItemDelegate *m_delegate;


};

#endif // DMTREEVIEW_H
