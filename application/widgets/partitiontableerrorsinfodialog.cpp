/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file partitiontableerrorsinfodialog.cpp
 *
 * @brief 分区表错误报告，展示分区表具体错误信息
 *
 * @date 2020-08-21 13:16
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "partitiontableerrorsinfodialog.h"
#include "partitiontableerrorsinfodelegate.h"

#include <DFrame>
#include <DGuiApplicationHelper>
#include <DPushButton>

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
    setTitle(tr("Errors in Partition Table")); // 分区表错误报告
    setMinimumSize(580, 386);

    DPalette palette1;
    palette1.setColor(DPalette::Text, QColor(0, 0, 0, 0.7));

    m_Label = new DLabel;
    m_Label->setText(tr("The partition table of disk  %1  has below errors:").arg(m_deviceInfo)); // 磁盘xxx存在下列分区表问题：
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
    m_tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    m_partitionTableErrorsInfoDelegatee = new PartitionTableErrorsInfoDelegate(this);
    m_tableView->setItemDelegate(m_partitionTableErrorsInfoDelegatee);
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        m_partitionTableErrorsInfoDelegatee->setTextColor(QColor("#C0C6D4"));
    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        m_partitionTableErrorsInfoDelegatee->setTextColor(QColor("#001A2E"));
    }

    m_standardItemModel->setColumnCount(1);
    m_standardItemModel->setHeaderData(0, Qt::Horizontal, tr("Error")); // 错误说明

    m_tableView->setModel(m_standardItemModel);
    m_tableView->horizontalHeader()->setStretchLastSection(true);// 设置最后一列自适应
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setFixedWidth(540);

    QList<QStandardItem*> itemList;

    itemList << new QStandardItem(tr("Partition table entries are not in disk order")); // 分区表项不是按磁盘顺序排列的

    m_standardItemModel->appendRow(itemList);

    DFrame *tableWidget = new DFrame;
    tableWidget->setFixedWidth(560);
    QHBoxLayout *tableLayout = new QHBoxLayout(tableWidget);
    tableLayout->addWidget(m_tableView);
    tableLayout->setSpacing(0);
    tableLayout->setContentsMargins(10, 10, 10, 10);

    pushButton = new DPushButton;
    pushButton->setText(tr("OK")); // 确定
    pushButton->setFixedSize(220, 36);

    DWidget *buttonWidget = new DWidget;
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->addStretch();
    buttonLayout->addWidget(pushButton);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    addSpacing(20);
    addContent(m_Label);
    addSpacing(10);
    addContent(tableWidget);
    addSpacing(17);
    addContent(buttonWidget);
}

void PartitionTableErrorsInfoDialog::initConnections()
{
    connect(pushButton, &DPushButton::clicked, this, &PartitionTableErrorsInfoDialog::close);
}


