#include "partitiontableerrorsinfodialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>

PartitionTableErrorsInfoDialog::PartitionTableErrorsInfoDialog(const QString &deviceInfo, QWidget *parent)
    : DDialog(parent)
{
    m_deviceInfo = deviceInfo;

    initUI();
    initConnections();
}

void PartitionTableErrorsInfoDialog::initUI()
{
    setTitle("Errors in Partition Table"); // 分区表错误报告
    setMinimumSize(580, 386);

    DPalette palette1;
    palette1.setColor(DPalette::Text, QColor(0, 0, 0, 0.7));

    m_Label = new DLabel;
    m_Label->setText(QString("The partition table of disk  %1  has below errors:").arg(m_deviceInfo)); // 磁盘xxx存在下列分区表问题：
    m_Label->setFont(QFont("SourceHanSansSC", 12, 50));
    m_Label->setPalette(palette1);

    m_tableView = new DTableView;
    m_standardItemModel = new QStandardItemModel;

    m_tableView->setShowGrid(false);
    m_tableView->setFrameShape(QAbstractItemView::NoFrame);
    m_tableView->verticalHeader()->setVisible(false); //隐藏列表头
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::NoSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setAlternatingRowColors(true);
//    m_tableView->setPalette(palette5);
    m_tableView->setFont(QFont("SourceHanSansSC", 10, 50));
    m_tableView->horizontalHeader()->setFont(QFont("SourceHanSansSC", 12, 57));

//    m_diskHealthDetectionDelegate = new DiskHealthDetectionDelegate(this);
//    m_tableView->setItemDelegate(m_diskHealthDetectionDelegate);
//    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
//        m_diskHealthDetectionDelegate->setTextColor(QColor("#C0C6D4"));
//    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
//        m_diskHealthDetectionDelegate->setTextColor(QColor("#001A2E"));
//    }

    m_standardItemModel->setColumnCount(2);
    m_standardItemModel->setHeaderData(0, Qt::Horizontal, tr("Partition")); // 分区
    m_standardItemModel->setHeaderData(1, Qt::Horizontal, tr("Status")); // 状态

    addSpacing(20);
    addContent(m_Label);
}

void PartitionTableErrorsInfoDialog::initConnections()
{

}


