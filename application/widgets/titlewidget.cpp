#include "titlewidget.h"
#include <QHBoxLayout>

TitleWidget::TitleWidget(DWidget *parent): DWidget(parent)
{
    initUi();
    initConnection();
}

void TitleWidget::initUi()
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    m_btnparted = createBtn(tr("partition"));
    m_btnformat = createBtn(tr("format"));
    m_btnmount = createBtn(tr("mount"));
    m_btnunmount = createBtn(tr("unmount"));
    m_btnresize = createBtn(tr("resize"));
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
    connect(m_btnresize, &DPushButton::clicked, this, &TitleWidget::showResizeInfoWIdget);
}

DPushButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    auto btn = new DPushButton(this);
    int tW = 36;
    btn->setFixedSize(QSize(tW, tW));
    btn->setIconSize(QSize(tW, tW));
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
    if (!tipPartDialog->isVisible())
        tipPartDialog->show();
    QList<QAbstractButton *> list3 = tipPartDialog->getButtons();
    for (int i = 0; i < list3.size(); i++) {
        connect(list3.at(i), &QAbstractButton::clicked, this, [ = ] {
            if (list3.at(i)->text() == "Ok")
            {
                partitionWidget->show();
            }
        });
    }
}

void TitleWidget::showFormateInfoWidget()
{
    tipFormateDialog->show();
}

void TitleWidget::showMountInfoWidget()
{
    tipMountDialog->show();
}

void TitleWidget::showUnmountInfoWidget()
{
    tipUnmountDialog->show();
}

void TitleWidget::showResizeInfoWIdget()
{
    tipResizeDialog->show();
}

void TitleWidget::slotCurSelectChanged()
{
    qDebug() << __FUNCTION__ << "-1--1-";
    //for (auto it = DMDbusHandler::instance()->probDeviceInfo().begin(); it != DMDbusHandler::instance()->probDeviceInfo().end(); it++) {
    auto it = DMDbusHandler::instance()->probDeviceInfo().find(DMDbusHandler::instance()->getCurPartititonInfo().device_path);
    if (it != DMDbusHandler::instance()->probDeviceInfo().end()) {
        double_t sectorall = (it.value().length * it.value().sector_size) / 1024.0 / 1024.0 / 1024.0;
        QString s_disksize = QString::number(sectorall, 'f', 2) + "GB";
        qDebug() << it.value().m_path << s_disksize;
        partitionWidget->getPartitionInfo(DMDbusHandler::instance()->getCurPartititonInfo(), s_disksize);
    }
    // }
}




