#ifndef FORMATEDIALOG_H
#define FORMATEDIALOG_H
#include "customcontrol/ddbase.h"
#include <DComboBox>
#include <DLineEdit>
DWIDGET_USE_NAMESPACE

class FormateDialog : public DDBase
{
    Q_OBJECT
public:
    explicit FormateDialog(QWidget *parent = nullptr);

private:
    void initUi();
    void initConnection();

    DComboBox *pformatcombo = nullptr;
    DLineEdit *pfilenameedit = nullptr;
signals:

public slots:
    void slotbuttonClicked(int index, const QString &text);
};

#endif // FORMATEDIALOG_H
