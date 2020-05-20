#ifndef UNMOUNTDIALOG_H
#define UNMOUNTDIALOG_H
#include "customcontrol/ddbase.h"

DWIDGET_USE_NAMESPACE
class UnmountDialog : public DDBase
{
    Q_OBJECT
public:
    explicit UnmountDialog(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();

public slots:
    void slotbuttonClicked(int index, const QString &text);


};

#endif // UNMOUNTDIALOG_H
