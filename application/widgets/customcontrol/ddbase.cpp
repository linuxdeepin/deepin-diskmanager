#include "ddbase.h"
#include "common.h"

DDBase::DDBase(QWidget *parent)
    : DDialog(parent)
{
    setModal(true);
    setIcon(QIcon::fromTheme(appName));
    mainFrame = new DFrame(this);
    mainFrame->setFrameRounded(false);
    mainFrame->setFrameStyle(DFrame::NoFrame);
    addContent(mainFrame);
    // updateGeometry();
}
