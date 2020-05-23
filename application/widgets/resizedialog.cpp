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
        PartitionInfo next;
        QVector<PartitionInfo> arrinfo = phandler->getCurDevicePartitionArr();
        int index = 0;
        for (index = 0; index < arrinfo.size(); index++) {
            PartitionInfo tem = arrinfo.at(index);
            if (curinfo == tem && index + 1 < arrinfo.size()) {
                next = arrinfo.at(index + 1);
                if (next.type == TYPE_UNALLOCATED) {
                    QString strspace = pedit->text();
                    if (0 == pcombo->currentIndex())
                        expandspace = strspace.toFloat() * (MEBIBYTE / curinfo.sector_size);
                    else
                        expandspace = strspace.toFloat() * (GIBIBYTE / curinfo.sector_size);

                    qDebug() << Utils::format_size(expandspace, curinfo.sector_size) << Utils::format_size(next.get_sector_length(), curinfo.sector_size);
                    canexpand = true;
                    index += 1;
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
            qDebug() << Utils::format_size(newinfo.sector_end - newinfo.sector_start, curinfo.sector_size) << next.sector_end << expandspace << next.get_sector_length();
            newinfo.sector_end = expandspace > next.get_sector_length() ? next.sector_end : curinfo.sector_end + expandspace;
            newinfo.sector_end = next.sector_end;
//            if (newinfo.sector_end - next.sector_end < MEBIBYTE / newinfo.sector_size) {

//            }
//            if ( newinfo->type == TYPE_LOGICAL )
//            {

//            }
//            newinfo.sector_end -= MEBIBYTE / newinfo.sector_size;
            qDebug() << Utils::format_size(newinfo.sector_end - newinfo.sector_start, curinfo.sector_size) << newinfo.sector_start << newinfo.sector_end;
            newinfo.alignment = ALIGN_STRICT;
            phandler->resize(newinfo);
            done(index);
        }
    } else {
        done(index);
    }

}


