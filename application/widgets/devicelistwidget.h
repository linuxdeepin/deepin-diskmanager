#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include <DWidget>
#include "customcontrol/dmtreeview.h"
#include "customcontrol/dmdiskinfobox.h"
#include "deviceinfo.h"
#include "../partedproxy/dmdbushandler.h"
#include "utils.h"
#include "commondef.h"
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
    DMDbusHandler *m_handler;
signals:
    void sigPartitionSelectChanged();
<<<<<<< Updated upstream

public slots:
    void slotUpdateDeviceInfo();
=======
public slots:
    void slotUpdateLwInfo(const DeviceInfoMap &infomap);
>>>>>>> Stashed changes
};

#endif // DEVICELISTWIDGET_H
