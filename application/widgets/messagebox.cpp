#include "messagebox.h"

#include <DLabel>

MessageBox::MessageBox(DDialog *parent) : DDialog(parent)
{

}

void MessageBox::setWarings(QString warningMsg, QString surebtntext, QString cancelbtntext)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    setTitle(tr("Warning"));

    addSpacing(10);
    addLabel(warningMsg);
    addSpacing(10);

    if (cancelbtntext != "") {
        addButton(cancelbtntext);
    }
    addButton(surebtntext);
    connect(this,&MessageBox::buttonClicked,this, [=]() {
        close();
    });
}

void MessageBox::addLabel(QString text)
{
    DLabel * title= new DLabel(this);
    title->setText(text);
    this->addContent(title,Qt::AlignHCenter);
}


