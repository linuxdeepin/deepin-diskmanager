#include "infoshowwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <DFrame>

DWIDGET_USE_NAMESPACE

InfoShowWidget::InfoShowWidget(DWidget *parent): DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::lightGray));
    setPalette(plt);
    initUi();
}

void InfoShowWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);
//    QPainter painter(this);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

}

void InfoShowWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    DFrame *pframe = new DFrame;
    layout->addWidget(pframe);

    QVBoxLayout *framelayout = new QVBoxLayout(pframe);
    framelayout->setContentsMargins(10, 10, 10, 10);
    DFrame *pframetop = new DFrame;
    pframetop->setFixedHeight(200);
    framelayout->addWidget(pframetop);


    DFrame *pframemid = new DFrame;
    pframemid->setFixedHeight(200);
    pframemid->setFrameShape(QFrame::NoFrame);
    framelayout->addWidget(pframemid);


    DFrame *pframebottom = new DFrame;
    // pframebottom->setFixedHeight(300);
    framelayout->addWidget(pframebottom);
    framelayout->addStretch();

}
