#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H
#include "customcontrol/sizeinfowidget.h"
#include "partedproxy/dmdbushandler.h"
#include <DWidget>
#include <DLabel>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
#include <QPixmap>
#include <QStandardItemModel>
#include <QStandardItem>
#include <DTableView>
#include"customcontrol/dmframewidget.h"
#include <DLabel>
#include <DFontSizeManager>
#include "customcontrol/dmtreeviewdelegate.h"
#include <QVector>
#include <QRgb>

class InfoTopFrame;

DWIDGET_USE_NAMESPACE
class InfoShowWidget: public DWidget
{
    Q_OBJECT
public:
    explicit InfoShowWidget(DWidget *parent = nullptr);
    void topFrameSettings();
    void midFramSettings();
    void bottomFramSettings();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void slotCurSelectChanged();

private:
    void initUi();
    void initConnection();

private:
    InfoTopFrame     *m_pInfoTopFrame = nullptr;

    DFrame *pframe;

    DFrame *pframemid;
    DmFrameWidget *pframebottom = nullptr;

    DLabel *totaluseLabel;
    DLabel *usedLabel;
    DLabel *trueusedLabel;
    DTableView *tableview;
    SizeInfoWidget *m_infowidget;
    double m_allsize;
    double m_used;
    double m_noused;
    DiskInfoData data;

};

#endif // INFOSHOWWIDGET_H
