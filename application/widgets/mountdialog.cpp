#include "mountdialog.h"
#include "widgets/widgetdeclare.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DFrame>
#include <DFontSizeManager>
#include <QDir>

MountDialog::MountDialog(QWidget *parent): DDBase(parent)
{
    initUi();
    initConnection();

}

void MountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setTitle(tr("Mount ") + info.path);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    DLabel *tipLabel = new DLabel(tr("Choose a mount point please"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    DLabel *mountLabel = new DLabel(tr("Mount point:"));
    DFontSizeManager::instance()->bind(mountLabel, DFontSizeManager::T6);
    pedit = new DFileChooserEdit(this);
    pedit->setFileMode(QFileDialog::Directory);
    pedit->fileDialog()->setOption(QFileDialog::ShowDirsOnly);
    pedit->fileDialog()->setAcceptMode(QFileDialog::AcceptOpen);

    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(mountLabel);
    mainLayout->addWidget(pedit);
    addButton(tr("Cancel"), true, ButtonNormal);
    okcode = addButton(tr("Mount"), false, ButtonRecommend);
    getButton(okcode)->setDisabled(true);
}

void MountDialog::initConnection()
{
    connect(pedit->lineEdit(), &QLineEdit::textChanged, this, &MountDialog::slotEditContentChanged);
    connect(this, &MountDialog::buttonClicked, this, &MountDialog::slotbuttonClicked);
}

void MountDialog::slotEditContentChanged(const QString &content)
{
    if (content.isEmpty()) {
        getButton(okcode)->setDisabled(true);
    } else {
        QDir dir(content);
        if (dir.exists()) {
            getButton(okcode)->setDisabled(false);
            pedit->setAlert(false);
        } else {
            getButton(okcode)->setDisabled(true);
            pedit->setAlert(true);
        }
    }
}

void MountDialog::slotbuttonClicked(int index, const QString &text)
{
    if (index == okcode) {
        DMDbusHandler::instance()->mount(pedit->lineEdit()->text());
    }
}

