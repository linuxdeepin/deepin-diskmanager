#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include <DWidget>
#include "customcontrol/dmtreeview.h"
#include "customcontrol/dmtreemanagerwidget.h"
DWIDGET_USE_NAMESPACE

class DeviceListWidget: public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();
    DmTreeview *m_treeview;
signals:
    void sigPartitionSelectChanged();
};

#endif // DEVICELISTWIDGET_H
