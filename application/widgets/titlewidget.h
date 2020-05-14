#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DWidget>
#include <DPushButton>

#include "customcontrol/tipformatedialog.h"
#include "customcontrol/tipresizedialog.h"
#include "customcontrol/tipmountdialog.h"
#include "customcontrol/tipumountdialog.h"
#include "customcontrol/tippartdialog.h"
DWIDGET_USE_NAMESPACE


class TitleWidget: public DWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(DWidget *parent = nullptr);
    void initUi();
    void initConnection();
    DPushButton *createBtn(const QString &btnName, bool bCheckable = false);

public slots:
    void updateCurPath(const QString &path);
    void slotCurSelectChanged(const QString &devicepath, const QString &partitionpath);

private:
    DPushButton *m_btnparted;
    DPushButton *m_btnformat;
    DPushButton *m_btnmount;
    DPushButton *m_btnunmount;
    DPushButton *m_btnresize;
    TipMountDialog *tipMountDialog;
    TipFormateDialog *tipFormateDialog;
    TipResizeDialog *tipResizeDialog;
    TipUmountDialog *tipUmountDialog;
    TipPartDialog *tipPartDialog;
};



#endif // TITLEWIDGET_H
