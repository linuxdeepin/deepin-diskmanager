#ifndef DMTREEMANAGERWIDGET_H
#define DMTREEMANAGERWIDGET_H

#include <QWidget>
#include <DWidget>
#include <QStandardItem>
#include "dmtreeview.h"
DWIDGET_USE_NAMESPACE
class DmTreeManagerWidget : public DWidget
{
    Q_OBJECT
public:
    explicit DmTreeManagerWidget(QWidget *parent = nullptr);
    void addItem();
signals:

public slots:
private:
    DmTreeview *m_treeview;
};

#endif // DMTREEMANAGERWIDGET_H
