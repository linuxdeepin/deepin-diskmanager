#include "titlewidget.h"
#include <QHBoxLayout>

TitleWidget::TitleWidget(DWidget *parent): DWidget(parent)
{
    initUi();
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
