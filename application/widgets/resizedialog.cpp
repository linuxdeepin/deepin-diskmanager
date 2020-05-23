#include "resizedialog.h"
#include "partedproxy/dmdbushandler.h"
#include <DLabel>
#include <DFontSizeManager>
#include <QHBoxLayout>

ResizeDialog::ResizeDialog(QWidget *parent): DDBase(parent)
{
    initUi();
    initConnection();
    setOnButtonClickedClose(false);
}

void ResizeDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    setTitle(tr("Tyr to resize partition ") + info.path);
    DLabel *tipLabel = new DLabel(tr("It will resize the partitions on the disk"), this);
    tipLabel ->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    QHBoxLayout *hlayout = new QHBoxLayout;
    pedit = new DLineEdit(this);
    pcombo = new DComboBox(this);
    QStringList stritems;
    stritems << "MB" << "GB";
    pcombo->addItems(stritems);
    pcombo->setCurrentIndex(0);
    pcombo->setFixedWidth(70);
    hlayout->addWidget(pedit);
    hlayout->addWidget(pcombo);
    mainLayout->addWidget(tipLabel);
    mainLayout->addLayout(hlayout);

    addButton(tr("Cancel"), false, ButtonNormal);
    okcode = addButton(tr("Confirm"), true, ButtonRecommend);
}

void ResizeDialog::initConnection()
{
    connect(this, &ResizeDialog::buttonClicked, this, &ResizeDialog::slotbuttonClicked);
}

void ResizeDialog::slotbuttonClicked(int index, const QString &text)
{
    if (okcode == index) {
        auto phandler = DMDbusHandler::instance();
        Q_ASSERT(phandler != nullptr);
        bool canexpand = false;
        Sector expandspace = 0;
        PartitionInfo curinfo = phandler->getCurPartititonInfo();
        QVector<PartitionInfo> arrinfo = phandler->getCurDevicePartitionArr();
        for (int i = 0; i < arrinfo.size(); i++) {
            PartitionInfo tem = arrinfo.at(i);
            if (curinfo == tem && i + 1 < arrinfo.size()) {
                PartitionInfo next = arrinfo.at(i + 1);
                if (next.type == TYPE_UNALLOCATED) {
                    QString strspace = pedit->text();
                    if (0 == pcombo->currentIndex())
                        expandspace = strspace.toFloat() * (MEBIBYTE / curinfo.sector_size) - 1;
                    else
                        expandspace = strspace.toFloat() * (GIBIBYTE / curinfo.sector_size) - 1;

                    if (expandspace > curinfo.get_sector_length()) {
                        canexpand = false;

                    }
                    break;
                }
            }
        }
        if (!canexpand) {

            pedit->setAlertMessageAlignment(Qt::AlignTop);
            pedit->showAlertMessage(tr("have no unallocated space,can not resize"));
            pedit->setAlert(true);

        } else {

            PartitionInfo newinfo = curinfo;
            Sector old_size = curinfo.get_sector_length();
            QString strspace = pedit->text();
            if (0 == pcombo->currentIndex())
                newinfo.sector_end = curinfo.sector_start + strspace.toFloat() * (MEBIBYTE / curinfo.sector_size) - 1;
            else
                newinfo.sector_end = curinfo.sector_start + strspace.toFloat() * (GIBIBYTE / curinfo.sector_size) - 1;

            newinfo.alignment = ALIGN_MEBIBYTE;

            done(index);
        }
    } else {
        done(index);
    }

}


