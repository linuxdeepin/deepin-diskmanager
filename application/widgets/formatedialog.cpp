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
    this->setFixedSize(450, 300);
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    setTitle(tr("Format ") + info.path);
    DLabel *tipLabel = new DLabel(tr("Formatting will erase all data on the disk, which cannot be \n undone. \n Please set its name and file system,and then format"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    QHBoxLayout *hlayoutname = new QHBoxLayout;
    DLabel *pfilename = new DLabel(tr("Name:"), this);
    pfilename->setMinimumWidth(76);
    DLineEdit *pfilenameedit = new DLineEdit(this);
    if (pfilenameedit->text().isEmpty())
        pfilenameedit->lineEdit()->setPlaceholderText(tr("Name"));
    hlayoutname->addWidget(pfilename);
    hlayoutname->addWidget(pfilenameedit);
    QHBoxLayout *hlayoutformat = new QHBoxLayout;
    DLabel *pformatname = new DLabel(tr("File system:"), this);
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
    okcode = addButton(tr("Format"), false, ButtonWarning);
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
