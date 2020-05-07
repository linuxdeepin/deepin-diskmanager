#include "infoshowwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <DFrame>

InfoShowWidget::InfoShowWidget(DWidget *parent): DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::lightGray));
    setPalette(plt);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    DFrame *pframe = new DFrame;
    layout->addWidget(pframe);;


}

void InfoShowWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);
//    QPainter painter(this);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

}
