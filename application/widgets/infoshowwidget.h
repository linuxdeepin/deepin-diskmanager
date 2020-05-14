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
public slots:
    void slotShowDiskInfo(QString diskname, QString diskformat, QString diskmemory);
    void slotCurSelectChanged(const QString &devicepath, const QString &partitionpath, Sector start, Sector end);
private:
    void initUi();
    void initConnection();
    DFrame *pframe;
    DFrame *pframetop;
    DFrame *pframemid;
    DFrame *pframebottom;
    DLabel *picLabel;
    DLabel *nameLabel;
    DLabel *typeLabel;
    DLabel *allnameLabel;
    DLabel *allmemoryLabel;
    DLabel *totaluseLabel;
    DLabel *usedLabel;
    DLabel *trueusedLabel;
    DTableView *tableview;
    SizeInfoWidget *m_infowidget;
    double m_allsize;
    double m_used;
    double m_trueused;

};

#endif // INFOSHOWWIDGET_H
