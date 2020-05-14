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

    connect(m_btnparted, &DPushButton::clicked, this, [ = ] {
        tipPartDialog = new TipPartDialog(this);
        tipPartDialog->show();
    });

    connect(m_btnformat, &DPushButton::clicked, this, [ = ] {
        tipFormateDialog = new TipFormateDialog(this);
        tipFormateDialog->show();
    });
    connect(m_btnmount, &DPushButton::clicked, this, [ = ] {
        tipMountDialog = new TipMountDialog(this);
        tipMountDialog->show();
    });
    connect(m_btnunmount, &DPushButton::clicked, this, [ = ] {
        tipUmountDialog = new TipUmountDialog(this);
        tipUmountDialog->show();
    });

    connect(m_btnresize, &DPushButton::clicked, this, [ = ] {
        tipResizeDialog = new TipResizeDialog(this);
        tipResizeDialog->show();
    });
}

void TitleWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged, this, &TitleWidget::slotCurSelectChanged);
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

void TitleWidget::updateCurPath(const QString &path)
{

}

void TitleWidget::slotCurSelectChanged(const QString &devicepath, const QString &partitionpath, Sector start, Sector end)
{
    qDebug() << __FUNCTION__ << "-1--1-";
}

