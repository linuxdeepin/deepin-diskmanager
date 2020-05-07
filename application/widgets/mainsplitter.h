#ifndef MAINSPLITTER_H
#define MAINSPLITTER_H
#include <DSplitter>

DWIDGET_USE_NAMESPACE

class DeviceListWidget;
class InfoShowWidget;

class MainSplitter: public DSplitter
{
    Q_OBJECT
    Q_DISABLE_COPY(MainSplitter)
public:
    explicit MainSplitter(DWidget *parent = nullptr);

public:
    void initui();

private:
    InfoShowWidget *m_infoshow;
    DeviceListWidget *m_devicelist;

};

#endif // MAINSPLITTER_H
