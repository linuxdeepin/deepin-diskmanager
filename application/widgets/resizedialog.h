#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H
#include "customcontrol/ddbase.h"
#include <DLineEdit>
#include <DComboBox>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ResizeDialog : public DDBase
{
    Q_OBJECT
public:
    ResizeDialog(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();

    DLineEdit *pedit;
    DComboBox *pcombo;
    DLabel *plabel;
public slots:
    void slotbuttonClicked(int index, const QString &text);
};

#endif // RESIZEDIALOG_H
