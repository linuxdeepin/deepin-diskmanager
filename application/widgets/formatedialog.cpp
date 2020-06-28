#include "formatedialog.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <DFontSizeManager>
#include <QVBoxLayout>
#include <QHBoxLayout>

FormateDialog::FormateDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void FormateDialog::initUi()
{
    this->setFixedSize(450, 300);
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    setTitle(tr("Format %1").arg(info.path));
    DLabel *tipLabel = new DLabel(tr("Formatting will erase all data on the disk, which cannot be undone"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setFixedHeight(50);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    QHBoxLayout *hlayoutname = new QHBoxLayout;
    DLabel *pfilename = new DLabel(tr("Name:"), this);
    pfilename->setMinimumWidth(76);
    pfilenameedit   = new DLineEdit(this);
    if (pfilenameedit->text().isEmpty())
        pfilenameedit->lineEdit()->setPlaceholderText(tr("Name"));
    hlayoutname->addWidget(pfilename);
    hlayoutname->addWidget(pfilenameedit);
    QHBoxLayout *hlayoutformat = new QHBoxLayout;
    DLabel *pformatname = new DLabel(tr("File system:"), this);
    pformatcombo = new DComboBox(this);
    pformatcombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QStringList fslist = DMDbusHandler::instance()->getallsupportfs();
    fslist.removeOne("linux-swap");
    pformatcombo->addItems(fslist);
    pformatcombo->setCurrentIndex(fslist.indexOf(Utils::FSTypeToString(static_cast<FSType>(info.fstype))));
    hlayoutformat->addWidget(pformatname);
    hlayoutformat->addWidget(pformatcombo);
    mainLayout->addWidget(tipLabel);
    mainLayout->addSpacing(60);
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
    Q_UNUSED(text);
    if (index == okcode) {
        DMDbusHandler::instance()->format(pformatcombo->currentText(), pfilenameedit->text());
    }
}
