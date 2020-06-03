#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include "partedproxy/dmdbushandler.h"
#include "customcontrol/partitionwidget.h"
#include "partitiondialog.h"
#include "utils.h"
#include <DWidget>
#include <DPushButton>
//#include "customcontrol/tippartdialog.h"

DWIDGET_USE_NAMESPACE


class TitleWidget: public DWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(DWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();
    DPushButton *createBtn(const QString &btnName, const QString &objName, bool bCheckable = false);
    void updateBtnStatus();


signals:

public slots:
    void slotCurSelectChanged();
    void showPartInfoWidget();
    void showFormateInfoWidget();
    void showMountInfoWidget();
    void showUnmountInfoWidget();
    void showResizeInfoWidget();
private:
    DPushButton *m_btnparted;
    DPushButton *m_btnformat;
    DPushButton *m_btnmount;
    DPushButton *m_btnunmount;
    DPushButton *m_btnresize;
};



#endif // TITLEWIDGET_H
