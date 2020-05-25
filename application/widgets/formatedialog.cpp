#include "formatedialog.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DFontSizeManager>
#include <QHBoxLayout>
#include <DLineEdit>


FormateDialog::FormateDialog(QWidget *parent) : DDBase(parent)
{
    initUi();
    initConnection();
}

void FormateDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    setTitle(tr("Try to format partition ") + info.path);
    DLabel *tipLabel = new DLabel(tr("To format the disk, all data stored on the disk will be deleted and cannot be undone"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    QHBoxLayout *hlayoutname = new QHBoxLayout;
    DLabel *pfilename = new DLabel(tr("File name:"), this);
    DLineEdit *pfilenameedit = new DLineEdit(this);
    hlayoutname->addWidget(pfilename);
    hlayoutname->addWidget(pfilenameedit);
    QHBoxLayout *hlayoutformat = new QHBoxLayout;
    DLabel *pformatname = new DLabel(tr("File format:"), this);
    pformatcombo = new DComboBox(this);
    pformatcombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    QStringList fslist = DMDbusHandler::instance()->getallsupportfs();
    pformatcombo->addItems(fslist);
    pformatcombo->setCurrentIndex(fslist.indexOf(Utils::FSTypeToString((FSType)info.fstype)));
    hlayoutformat->addWidget(pformatname);
    hlayoutformat->addWidget(pformatcombo);
    mainLayout->addWidget(tipLabel);
    mainLayout->addLayout(hlayoutname);
    mainLayout->addLayout(hlayoutformat);

    addButton(tr("Cancel"), true, ButtonNormal);
    okcode = addButton(tr("Confirm"), false, ButtonWarning);
}

void FormateDialog::initConnection()
{
    connect(this, &FormateDialog::buttonClicked, this, &FormateDialog::slotbuttonClicked);
}

void FormateDialog::slotbuttonClicked(int index, const QString &text)
{
    if (index == okcode) {
        DMDbusHandler::instance()->format(pformatcombo->currentText());
    }
}
