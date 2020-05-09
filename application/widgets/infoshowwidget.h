#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H
#include <DWidget>
#include <DLabel>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>

DWIDGET_USE_NAMESPACE
class InfoShowWidget: public DWidget
{
    Q_OBJECT
public:
    explicit InfoShowWidget(DWidget *parent = nullptr);
    void topFrameSettings();



protected:
    void paintEvent(QPaintEvent *event);
public slots:
    void slotShowDiskInfo(QString diskname, QString diskformat, QString diskmemory);
private:
    void initUi();
    DFrame *pframe;
    DFrame *pframetop;
    DLabel *picLabel;
    DLabel *nameLabel;
    DLabel *typeLabel;
    DLabel *allnameLabel;
    DLabel *allmemoryLabel;



};

#endif // INFOSHOWWIDGET_H
