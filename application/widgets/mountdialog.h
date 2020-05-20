#ifndef MOUNTDIALOG_H
#define MOUNTDIALOG_H
#include "customcontrol/ddbase.h"
#include <dfilechooseredit.h>

DWIDGET_USE_NAMESPACE

class MountDialog: public DDBase
{
public:
    explicit MountDialog(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();


public slots:
    void slotEditContentChanged(const QString &content);
    void slotbuttonClicked(int index, const QString &text);

private:
    DFileChooserEdit *pedit;
};

#endif // MOUNTDIALOG_H
