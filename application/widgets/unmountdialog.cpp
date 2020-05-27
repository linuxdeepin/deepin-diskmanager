#include "unmountdialog.h"
#include "widgetdeclare.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DFrame>
#include <DFontSizeManager>
UnmountDialog::UnmountDialog(QWidget *parent) : DDBase(parent)
{
    initUi();
    initConnection();
}

void UnmountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setTitle(tr("Unmount") + info.path);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    DLabel *tipLabel = new DLabel(tr("Make sure there are no programs running on the disk"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    mainLayout->addWidget(tipLabel);

    addButton(tr("Cancel"), false, ButtonNormal);
    okcode = addButton(tr("Unmount"), false, ButtonWarning);
}

void UnmountDialog::initConnection()
{
    connect(this, &UnmountDialog::buttonClicked, this, &UnmountDialog::slotbuttonClicked);
}

void UnmountDialog::slotbuttonClicked(int index, const QString &text)
{
    if (okcode == index) {
        DMDbusHandler::instance()->unmount();
    }
}
