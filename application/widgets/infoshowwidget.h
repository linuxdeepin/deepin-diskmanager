#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H

#include <DWidget>
#include <DLabel>
#include <DFrame>
#include "customcontrol/dmtreeviewdelegate.h"

class DmFrameWidget;
class SizeInfoWidget;
class InfoTopFrame;

DWIDGET_USE_NAMESPACE
class InfoShowWidget: public DFrame
{
    Q_OBJECT
public:
    explicit InfoShowWidget(DWidget *parent = nullptr);

private:
    void midFramSettings();
    void bottomFramSettings();
    void initUi();
    void initConnection();

private slots:
    void slotCurSelectChanged();

private:
    InfoTopFrame     *m_pInfoTopFrame = nullptr;

    DFrame *pframe = nullptr;
    DFrame *pframemid = nullptr;
    DmFrameWidget *pframebottom = nullptr;

    DLabel *totaluseLabel = nullptr;
    DLabel *usedLabel = nullptr;
    DLabel *trueusedLabel = nullptr;
    DTableView *tableview = nullptr;
    SizeInfoWidget *m_infowidget = nullptr;
    double m_allsize;
    double m_used;
    double m_noused;
    DiskInfoData data;
};

#endif // INFOSHOWWIDGET_H
