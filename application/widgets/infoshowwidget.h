#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H
#include <DWidget>
#include <DLabel>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
#include "customcontrol/sizeinfowidget.h"
#include <QPixmap>

DWIDGET_USE_NAMESPACE
class InfoShowWidget: public DWidget
{
    Q_OBJECT
public:
    explicit InfoShowWidget(DWidget *parent = nullptr);
    void topFrameSettings();
    void midFramSettings();


protected:
    void paintEvent(QPaintEvent *event);
public slots:
    void slotShowDiskInfo(QString diskname, QString diskformat, QString diskmemory);
    void slotCurSelectChanged(const QString &devicepath, const QString &partitionpath);
private:
    void initUi();
    void initConnection();
    DFrame *pframe;
    DFrame *pframetop;
    DFrame *pframemid;
    DLabel *picLabel;
    DLabel *nameLabel;
    DLabel *typeLabel;
    DLabel *allnameLabel;
    DLabel *allmemoryLabel;
    DLabel *totaluseLabel;
    DLabel *usedLabel;
    DLabel *trueusedLabel;
    SizeInfoWidget *m_infowidget;


};

#endif // INFOSHOWWIDGET_H
