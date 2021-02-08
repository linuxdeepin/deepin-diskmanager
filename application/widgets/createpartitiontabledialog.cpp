#include "createpartitiontabledialog.h"

#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QDir>
#include <QVBoxLayout>

CreatePartitionTableDialog::CreatePartitionTableDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void CreatePartitionTableDialog::initUi()
{
    m_ComboBox = new DComboBox;
    m_ComboBox->addItem("GPT");
    m_ComboBox->addItem("MSDOS");

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->addWidget(m_ComboBox);

    addButton(tr("Cancel"), true, ButtonNormal);

    DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
    if (info.disktype == "unrecognized") {
        // 当前磁盘无分区表，是否新建分区表?
        setTitle(tr("No partition table in this disk. Create a new one?"));
        m_okCode = addButton(tr("Create"), false, ButtonRecommend);
    } else {
        // 当前磁盘已有分区表，是否替换此分区表?
        setTitle(tr("The disk has a partition table already. Replace it?"));
        m_okCode = addButton(tr("Replace"), false, ButtonRecommend);
    }

}

void CreatePartitionTableDialog::initConnection()
{
    connect(this, &CreatePartitionTableDialog::buttonClicked, this, &CreatePartitionTableDialog::onButtonClicked);
}

void CreatePartitionTableDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == m_okCode) {
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        DMDbusHandler::instance()->createPartitionTable(info.m_path, QString("%1").arg(info.length), QString("%1").arg(info.sector_size), m_ComboBox->currentText());
        close();
    } else {
        close();
    }
}

