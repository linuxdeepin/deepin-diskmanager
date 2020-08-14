#ifndef MOUNTDIALOG_H
#define MOUNTDIALOG_H
#include "customcontrol/ddbase.h"
#include <dfilechooseredit.h>
#include <DComboBox>

DWIDGET_USE_NAMESPACE

class MountDialog : public DDBase
{
    Q_OBJECT
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
    DComboBox *m_ComboBox;
};

#endif // MOUNTDIALOG_H
