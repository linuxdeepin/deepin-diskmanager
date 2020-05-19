#ifndef UNMOUNTDIALOG_H
#define UNMOUNTDIALOG_H

#include <DDialog>

DWIDGET_USE_NAMESPACE
class UnmountDialog : public DDialog
{
    Q_OBJECT
public:
    explicit UnmountDialog(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();

public slots:
    void slotbuttonClicked(int index, const QString &text);

private:
    int unmountcode;
};

#endif // UNMOUNTDIALOG_H
