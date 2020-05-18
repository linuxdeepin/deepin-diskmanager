#include "dmframewidget.h"
#include <QDebug>


DmFrameWidget::DmFrameWidget(DiskInfoData data, QWidget *parent): DFrame(parent), m_infodata(data)
{

}

//void DmFrameWidget::setFrameData(DiskInfoData data)
//{
//    m_infodata = data;
//    update();
//}

void DmFrameWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QWidget::paintEvent(event);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    DPalette pa = DApplicationHelper::instance()->palette(this);
    QBrush brush = DApplicationHelper::instance()->palette(this).frameBorder();
    painter.setBrush(brush);
    painter.setPen(Qt::gray);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 15, 15);
    QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
    DPalette pa1 = DApplicationHelper::instance()->palette(this);
    QBrush brush1 = DApplicationHelper::instance()->palette(this).base();
    painter.setBrush(brush1);
    painter.fillRect(paintRect, brush1);
    painter.drawLine(paintRect.width() / 2, rect.topLeft().y(), paintRect.width() / 2, rect.bottomLeft().y());
    QRect textRect = QRect(rect.width() / 2 - 80, rect.topLeft().y() + 10, 80, 40);
    QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
    painter.setPen(Qt::black);
    painter.drawText(textRect, m_infodata.mountpoints);
    textRect.moveTo(rect.width() / 2 - 80, rect.topLeft().y() + 60);
    painter.drawText(textRect, m_infodata.unused);
    textRect.moveTo(rect.width() / 2 - 80, rect.topLeft().y() + 110);
    painter.drawText(textRect, m_infodata.used);
    textRect.moveTo(rect.width() - 80, rect.topLeft().y() + 10);
    painter.drawText(textRect, m_infodata.fstype);
    textRect.moveTo(rect.width() - 80, rect.topLeft().y() + 60);
    painter.drawText(textRect, m_infodata.partitionsize);
    textRect.moveTo(rect.width() - 80, rect.topLeft().y() + 110);
    painter.drawText(textRect, m_infodata.syslabel);
    painter.restore();

}
