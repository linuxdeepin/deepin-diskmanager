#include "unmountdialog.h"
#include "widgetdeclare.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DFrame>
#include <DFontSizeManager>
UnmountDialog::UnmountDialog(QWidget *parent) : DDialog(parent)
{
    initUi();
    initConnection();
}

void UnmountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setModal(true);
    setIcon(QIcon::fromTheme(app_name));
    DFrame *mainFrame = new DFrame(this);
    mainFrame->setFrameRounded(false);
    mainFrame->setFrameStyle(DFrame::NoFrame);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    DLabel *titleLabel = new DLabel(info.path + tr(" will be ummounted"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T6);
    DLabel *tipLabel = new DLabel(tr("The disk will be ummounted. Click ummounted"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(tipLabel);
    addContent(mainFrame);

    addButton(tr("Cancel"), false, ButtonNormal);
    unmountcode = addButton(tr("Unmount"), false, ButtonWarning);
}

void UnmountDialog::initConnection()
{
    connect(this, &UnmountDialog::buttonClicked, this, &UnmountDialog::slotbuttonClicked);
}

void UnmountDialog::slotbuttonClicked(int index, const QString &text)
{
    if (unmountcode == index) {
        DMDbusHandler::instance()->unmount();
    }
}
