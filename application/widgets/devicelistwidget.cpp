#include "devicelistwidget.h"
#include <DPalette>

DeviceListWidget::DeviceListWidget(QWidget *parent): DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::white));
    setPalette(plt);

    setMaximumWidth(360);
    setMinimumWidth(100);

}
