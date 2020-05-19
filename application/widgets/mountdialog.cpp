#include "mountdialog.h"
#include "widgets/widgetdeclare.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DFrame>
#include <DFontSizeManager>
#include <DStyleOption>
#include <QHideEvent>
#include <QDir>

MountDialog::MountDialog(QWidget *parent): DDialog(parent)
{
    initUi();
    initConnection();
}

void MountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setModal(true);
    setIcon(QIcon::fromTheme(app_name));
    DFrame *mainFrame = new DFrame(this);
    mainFrame->setFrameRounded(false);
    mainFrame->setFrameStyle(DFrame::NoFrame);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    DLabel *titleLabel = new DLabel(info.path + tr(" will be mounted"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    DLabel *tipLabel = new DLabel(tr("The disk will be mounted. Click mount"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    DLabel *mountLabel = new DLabel(tr("Mount point:"));
    DFontSizeManager::instance()->bind(mountLabel, DFontSizeManager::T6);
    pedit = new DFileChooserEdit(this);
    pedit->setFileMode(QFileDialog::Directory);
    pedit->fileDialog()->setOption(QFileDialog::ShowDirsOnly);
    pedit->fileDialog()->setAcceptMode(QFileDialog::AcceptOpen);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(mountLabel);
    mainLayout->addWidget(pedit);
    addContent(mainFrame);
    addButton(tr("Cancel"), true, ButtonNormal);
    okcode = addButton(tr("Ok"), false, ButtonRecommend);
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

