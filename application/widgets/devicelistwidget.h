#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include <DWidget>
#include "customcontrol/dmtreeview.h"
#include "customcontrol/dmdiskinfobox.h"
DWIDGET_USE_NAMESPACE

class DeviceListWidget: public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();
    void add();
    DmTreeview *m_treeview;

signals:
    void sigPartitionSelectChanged();

public slots:
    void slotUpdateDeviceInfo();
};

#endif // DEVICELISTWIDGET_H
