#include "titlewidget.h"
#include "mountdialog.h"
#include "unmountdialog.h"
#include "formatedialog.h"
#include "resizedialog.h"
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
//    tipFormateDialog = new TipFormateDialog(this);
//    tipUnmountDialog = new TipUmountDialog(this);
//    tipResizeDialog = new TipResizeDialog(this);
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
    tipPartDialog->getButton(1)->disconnect();
    tipPartDialog->getFlagShow(0);
    controlButton = 1;
    connect(tipPartDialog->getButton(1), &QAbstractButton::clicked, this, [ = ] {
        if (controlButton == 1)
        {
            tipPartDialog->close();
            partitionWidget->show();
            partitionWidget->getPartitionInfo(getPartitionInfo, device_size);

        }
    });

}

void TitleWidget::showFormateInfoWidget()
{

    FormateDialog dlg;
    dlg.exec();

}

void TitleWidget::showMountInfoWidget()
{
    MountDialog dlg;
    dlg.exec();
}

void TitleWidget::showUnmountInfoWidget()
{
    UnmountDialog dlg;
    dlg.exec();
}

void TitleWidget::showResizeInfoWidget()
{
//    tipPartDialog->getButton(1)->disconnect();
//    tipPartDialog->getFlagShow(4);
//    controlButton = 5;
//    connect(tipPartDialog->getButton(1), &QAbstractButton::clicked, this, [ = ] {
//        if (controlButton == 5)
//        {
//            tipPartDialog->close();
//            qDebug() << tr("XXX system space is in operation");
//        }
//    });
    ResizeDialog dlg;
    dlg.exec();

}

void TitleWidget::updateBtnStatus()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    //已挂载
    if (info.mountpoints.size() > 0 && info.busy) {
        m_btnparted->setDisabled(true);
        m_btnformat->setDisabled(true);
        m_btnmount->setDisabled(true);
        m_btnunmount->setDisabled(false);
        m_btnresize->setDisabled(true);
    } else {
        //需判断扩展分区上是否无分区，否则认为不可操作，此处省略操作
        if (FS_EXTENDED == info.fstype) {
            m_btnparted->setDisabled(true);
            m_btnformat->setDisabled(true);
            m_btnmount->setDisabled(true);
            m_btnunmount->setDisabled(true);
            m_btnresize->setDisabled(true);
        } else {
            m_btnunmount->setDisabled(true);
            if (info.fstype == FS_UNALLOCATED) {
                m_btnparted->setDisabled(false);
                m_btnformat->setDisabled(true);
                m_btnmount->setDisabled(true);
                m_btnresize->setDisabled(true);
            } else if (info.fstype == FS_UNKNOWN) {
                m_btnparted->setDisabled(true);
                m_btnformat->setDisabled(false);
                m_btnmount->setDisabled(true);
                m_btnresize->setDisabled(true);
            } else {
                m_btnparted->setDisabled(true);
                m_btnformat->setDisabled(false);
                m_btnmount->setDisabled(false);
                m_btnresize->setDisabled(false);
            }
        }
    }
}

void TitleWidget::slotCurSelectChanged()
{
    updateBtnStatus();
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




