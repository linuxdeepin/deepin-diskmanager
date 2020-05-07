#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include <DWidget>

DWIDGET_USE_NAMESPACE

class DeviceListWidget: public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);
};

#endif // DEVICELISTWIDGET_H
