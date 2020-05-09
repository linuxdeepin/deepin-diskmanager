#ifndef DEVICELISTWIDGET_H
#define DEVICELISTWIDGET_H
#include <DWidget>

DWIDGET_USE_NAMESPACE

class DeviceListWidget: public DWidget
{
    Q_OBJECT
public:
    explicit DeviceListWidget(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();

signals:
    void sigPartitionSelectChanged();
};

#endif // DEVICELISTWIDGET_H
