#include "resizedialog.h"
#include "partedproxy/dmdbushandler.h"
#include <DFontSizeManager>
#include <QHBoxLayout>
#include <QRegExpValidator>
#include <QRegExp>

ResizeDialog::ResizeDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
    setOnButtonClickedClose(false);
}

void ResizeDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    setTitle(tr("Resize %1").arg(info.path));
    DLabel *tipLabel = new DLabel(tr("It will resize the partitions on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    QHBoxLayout *hlayout = new QHBoxLayout;
    pedit = new DLineEdit(this);
    QRegExp regexp("^[0-9]*\\.[0-9]{1,2}");
    QRegExpValidator *pvalidaor = new QRegExpValidator(regexp, this);
    pedit->lineEdit()->setValidator(pvalidaor);
    pcombo = new DComboBox(this);
    plabel = new DLabel(this);
    plabel->setText(tr("Reserved size:"));
    plabel->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    DFontSizeManager::instance()->bind(plabel, DFontSizeManager::T6);
    QStringList stritems;
    stritems << "MiB"
             << "GiB";
    pcombo->addItems(stritems);
    pcombo->setCurrentIndex(0);
    pcombo->setFixedWidth(70);
    hlayout->addWidget(plabel);
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
    connect(pcombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboSelectedChanged(int)));
    connect(pedit, &DLineEdit::textChanged, this, &ResizeDialog::slotEditTextChanged);
}

void ResizeDialog::slotbuttonClicked(int index, const QString &)
{
    if (okcode == index) {
        auto phandler = DMDbusHandler::instance();
        Q_ASSERT(phandler != nullptr);
        bool canexpand = false;
        Sector expandspace = 0;
        PartitionInfo curinfo = phandler->getCurPartititonInfo();
        PartitionInfo next;
        PartitionVec arrinfo = phandler->getCurDevicePartitionArr();
        int index = 0;
        for (index = 0; index < arrinfo.size(); index++) {
            PartitionInfo tem = arrinfo.at(index);
            if (curinfo == tem && index + 1 < arrinfo.size()) {
                next = arrinfo.at(index + 1);
                if (next.type == TYPE_UNALLOCATED) {
                    QString strspace = pedit->text();
                    double total = 0;
                    //目前使用GB为单位精度损失太大，如果扩容全部空闲分区会出现空隙，按当前单位转换后如果差值小于0.01默认全选
                    if (0 == pcombo->currentIndex()) {
                        total = Utils::sector_to_unit(next.get_sector_length(), curinfo.sector_size, UNIT_MIB);
                        expandspace = strspace.toFloat() * (MEBIBYTE / curinfo.sector_size);
                        if (strspace.toFloat() - total >= 0.01) {
                            break;
                        }
                        if (total - strspace.toFloat() < 0.01)
                            expandspace = next.get_sector_length();

                    } else {
                        total = Utils::sector_to_unit(next.get_sector_length(), curinfo.sector_size, UNIT_GIB);
                        expandspace = strspace.toFloat() * (GIBIBYTE / curinfo.sector_size);
                        if (strspace.toFloat() - total >= 0.01) {
                            break;
                        }
                        if (total - strspace.toFloat() < 0.01)
                            expandspace = next.get_sector_length();
                    }

                    qDebug() << curinfo.sector_size * 1.0 << GIBIBYTE / curinfo.sector_size << GIBIBYTE / (curinfo.sector_size * 1.0);
                    qDebug() << Utils::format_size(expandspace, curinfo.sector_size) << Utils::format_size(next.get_sector_length(), curinfo.sector_size);
                    canexpand = true;
                    index += 1;
                    break;
                }
            }
        }
        if (!canexpand) {
            pedit->setAlertMessageAlignment(Qt::AlignTop);
            pedit->showAlertMessage(tr("Space limit exceeded"), mainFrame);
            pedit->setAlert(true);
            return ;

        } else {
            PartitionInfo newinfo = curinfo;
            qDebug() << Utils::format_size(newinfo.sector_end - newinfo.sector_start, curinfo.sector_size) << next.sector_start << next.sector_end << expandspace << next.get_sector_length();
            //  newinfo.sector_end = expandspace > next.get_sector_length() ? next.sector_end : curinfo.sector_end + expandspace;

            newinfo.sector_end = expandspace + next.sector_start > next.sector_end ? next.sector_end : next.sector_start + expandspace;

            Sector diff = 0;
            diff = (newinfo.sector_end + 1) % (MEBIBYTE / newinfo.sector_size);
            if (diff)
                newinfo.sector_end -= diff;
            else {
                // If this is a GPT partition table and the partition ends less than 34 sectors
                // from the end of the device, then reserve at least a mebibyte for the backup
                // partition table.
                DeviceInfo device = phandler->getCurDeviceInfo();
                if (device.disktype == "gpt" && device.length - newinfo.sector_end < 34)
                    newinfo.sector_end -= MEBIBYTE / newinfo.sector_size;
            }

            qDebug() << Utils::format_size(newinfo.sector_end - newinfo.sector_start, curinfo.sector_size) << newinfo.sector_start << newinfo.sector_end;
            newinfo.alignment = ALIGN_MEBIBYTE; //ALIGN_MEBIBYTE;
            phandler->resize(newinfo);
            done(index);
        }
    } else {
        done(index);
    }
}

void ResizeDialog::slotComboSelectedChanged(int index)
{
    QString strspace = pedit->text();
    QString expandspace;
    if (!strspace.isEmpty()) {
        if (index == 0) {
            //MiB
            expandspace = QString().setNum(strspace.toFloat() * 1024.0, 'f', 2);

        } else if (index == 1) {
            //GiB
            expandspace = QString().setNum(strspace.toFloat() / 1024.0, 'f', 2);
        }
        pedit->setText(expandspace);
    }
}

void ResizeDialog::slotEditTextChanged(const QString &)
{
    if (pedit->isAlert())
        pedit->setAlert(false);
}
