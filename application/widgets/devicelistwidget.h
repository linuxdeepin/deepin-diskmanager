#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include <DWidget>
#include "customcontrol/dmtreeview.h"
#include "partedproxy/dmdbushandler.h"

DWIDGET_USE_NAMESPACE

class DeviceListWidget: public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);
    ~DeviceListWidget();
private:
    void initUi();
    void initConnection();
    void add();
    DmTreeview *m_treeview = nullptr;

signals:
    void sigPartitionSelectChanged();

public slots:
    void slotUpdateDeviceInfo();

private:
    DmDiskinfoBox *m_box = nullptr;
    DmDiskinfoBox *m_childbox = nullptr;
};

#endif // DEVICELISTWIDGET_H
