#include "messagebox.h"

#include <DLabel>

MessageBox::MessageBox(DDialog *parent) : DDialog(parent)
{

}

void MessageBox::setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, const QString &cancalBtnText)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    if (!title.isEmpty()) {
        setTitle(title);
        addSpacing(10);
    }

    if (!warningMsg.isEmpty()) {
        addLabel(warningMsg);
        addSpacing(10);
    }

    if (!cancalBtnText.isEmpty()) {
        addButton(cancalBtnText);
    }

    addButton(sureBtnText);
    connect(this, &MessageBox::buttonClicked, this, [=]() {
        close();
    });
}
void MessageBox::setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, ButtonType sureBtnType, const QString &cancalBtnText)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    if (!title.isEmpty()) {
        setTitle(title);
        addSpacing(10);
    }

    if (!warningMsg.isEmpty()) {
        addLabel(warningMsg);
        addSpacing(10);
    }

    if (!cancalBtnText.isEmpty()) {
        addButton(cancalBtnText);
    }

    addButton(sureBtnText, false, sureBtnType);
    connect(this, &MessageBox::buttonClicked, this, [=]() {
        close();
    });
}

void MessageBox::setProgressBar(const QString &title, const QString &cancalBtnText)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    if (!title.isEmpty()) {
        setTitle(title);
        addSpacing(10);
    }

    addProgressBar();
    addSpacing(10);

    addButton(cancalBtnText);
    connect(this, &MessageBox::buttonClicked, this, [=]() {
        close();
    });
}

void MessageBox::addLabel(QString text)
{
    DLabel *label= new DLabel(this);
    label->setText(text);
    addContent(label, Qt::AlignHCenter);
}

void MessageBox::addProgressBar()
{
    m_progressBar = new DProgressBar(this);
    m_progressBar->setValue(50);
    m_progressBar->setFixedSize(330, 10);
    addContent(m_progressBar, Qt::AlignHCenter);
}


