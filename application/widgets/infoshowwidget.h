#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H

#include "customcontrol/dmtreeviewdelegate.h"
#include <DWidget>
#include <DLabel>
#include <DFrame>

class DmFrameWidget;
class SizeInfoWidget;
class InfoTopFrame;

DWIDGET_USE_NAMESPACE
class InfoShowWidget : public DFrame
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
    void slothandleChangeTheme();

private:
    InfoTopFrame *m_pInfoTopFrame = nullptr;

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
    DPalette m_parentPb;
    QColor fillcolor;
    QColor fillcolor1;
};

#endif // INFOSHOWWIDGET_H
