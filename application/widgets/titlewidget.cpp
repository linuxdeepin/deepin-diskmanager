#include "titlewidget.h"
#include <QHBoxLayout>

#include "widgetdeclare.h"

TitleWidget::TitleWidget(DWidget *parent): DWidget(parent)
{
    initUi();
    initConnection();
}

void TitleWidget::initUi()
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_btnparted = createBtn(tr("partition"), "partition");
    m_btnformat = createBtn(tr("format"), "format");
    m_btnmount = createBtn(tr("mount"), "mount");
    m_btnunmount = createBtn(tr("unmount"), "unmount");
    m_btnresize = createBtn(tr("resize"), "resize");
    layout->addWidget(m_btnparted);
    layout->addWidget(m_btnformat);
    layout->addWidget(m_btnmount);
    layout->addWidget(m_btnunmount);
    layout->addWidget(m_btnresize);
    tipPartDialog = new TipPartDialog(this);
    tipFormateDialog = new TipFormateDialog(this);
    tipMountDialog = new TipMountDialog(this);
    tipUnmountDialog = new TipUmountDialog(this);
    tipResizeDialog = new TipResizeDialog(this);
    partitionWidget = new PartitionWidget(this);
}

void TitleWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged, this, &TitleWidget::slotCurSelectChanged);
    connect(m_btnparted, &DPushButton::clicked, this, &TitleWidget::showPartInfoWidget);
    connect(m_btnformat, &DPushButton::clicked, this, &TitleWidget::showFormateInfoWidget);
    connect(m_btnmount, &DPushButton::clicked, this, &TitleWidget::showMountInfoWidget);
    connect(m_btnunmount, &DPushButton::clicked, this, &TitleWidget::showUnmountInfoWidget);
    connect(m_btnresize, &DPushButton::clicked, this, &TitleWidget::showResizeInfoWidget);
}

DPushButton *TitleWidget::createBtn(const QString &btnName,  const QString &objName, bool bCheckable)
{
    auto btn = new DPushButton(this);

    //  wzx 设置图标icon    2020-05-19
    QIcon icon = getIcon(objName);
    btn->setIcon(icon);

    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(18, 18));
    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);

    if (bCheckable) {
        btn->setChecked(false);
    }
    // btn->setDisabled(true);
    return btn;
}

void TitleWidget::showPartInfoWidget()
{
    if (!tipPartDialog->isVisible()) {
        tipPartDialog->getFlagShow(0);
        controlButton = 1;
    }

    connect(tipPartDialog->getButton(1), &QAbstractButton::clicked, this, [ = ] {
        if (controlButton == 1)
        {
            partitionWidget->show();
            partitionWidget->getPartitionInfo(getPartitionInfo, device_size);

        }
    });

}

void TitleWidget::showFormateInfoWidget()
{
    if (!tipPartDialog->isVisible()) {
        tipPartDialog->getFlagShow(1);
        controlButton = 2;
    }
    connect(tipPartDialog->getButton(1), &QAbstractButton::clicked, this, [ = ] {
        if (controlButton == 2)
        {
            qDebug() << "XXX will be formatted";

        }
    });

}

void TitleWidget::showMountInfoWidget()
{
    tipMountDialog->show();
}

void TitleWidget::showUnmountInfoWidget()
{
    tipUnmountDialog->show();
}

void TitleWidget::showResizeInfoWidget()
{
    tipResizeDialog->show();
}



void TitleWidget::slotCurSelectChanged()
{
    qDebug() << __FUNCTION__ << "-1--1-";
    auto it = DMDbusHandler::instance()->probDeviceInfo().find(DMDbusHandler::instance()->getCurPartititonInfo().device_path);
    if (it != DMDbusHandler::instance()->probDeviceInfo().end()) {
        //  QString s_disksize = QString::number(((it.value().length * it.value().sector_size) / 1024.0 / 1024.0 / 1024.0), 'f', 2) + "GB";
        QString s_disksize = QString::number(Utils::sector_to_unit(it.value().length, it.value().sector_size, SIZE_UNIT::UNIT_GIB), 'f', 2) + "GB";
        //  qDebug() << Utils::sector_to_unit(it.value().sectors, it.value().sector_size, SIZE_UNIT::UNIT_GIB) << it.value().m_path << s_disksize << it.value().sectors << it.value().sector_size;
        getPartitionInfo = DMDbusHandler::instance()->getCurPartititonInfo();
        device_size = s_disksize;
    }
}




