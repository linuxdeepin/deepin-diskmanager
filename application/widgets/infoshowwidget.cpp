#include "infoshowwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

#include "customcontrol/sizeinfowidget.h"
#include "partedproxy/dmdbushandler.h"
#include "customcontrol/infotopframe.h"
#include "customcontrol/dmframewidget.h"

#include "widgetdeclare.h"

DWIDGET_USE_NAMESPACE

InfoShowWidget::InfoShowWidget(DWidget *parent): DFrame(parent)
{
    initUi();
    initConnection();
}

void InfoShowWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    pframe = new DFrame();
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(pframe);

    QVBoxLayout *framelayout = new QVBoxLayout(pframe);
    framelayout->setContentsMargins(10, 10, 10, 10);

    //  右侧 上部数据信息
    m_pInfoTopFrame = new InfoTopFrame(this);
    framelayout->addWidget(m_pInfoTopFrame);

    pframemid = new DFrame;
    pframemid->setFixedHeight(200);
    pframemid->setFrameShape(QFrame::NoFrame);
    framelayout->addWidget(pframemid);
    midFramSettings();
    pframebottom = new DmFrameWidget(data);
    pframebottom->setFixedHeight(150);
    framelayout->addWidget(pframebottom);
    framelayout->addStretch();
    bottomFramSettings();
}

void InfoShowWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged, this, &InfoShowWidget::slotCurSelectChanged);
}

void InfoShowWidget::midFramSettings()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(pframemid);
    m_infowidget = new SizeInfoWidget;
    mainLayout->addWidget(m_infowidget);
//    totaluseLabel = new DLabel(pframemid);
//    QFont  font;
//    font.setPointSize(20);
//    QPixmap pixmap;
//    pixmap.load(":/icons/deepin/builtin/exception-logo.svg");
//    totaluseLabel->setPixmap(pixmap);
//    totaluseLabel->setGeometry(0, 100, 10, 10);
//    totaluseLabel->setFixedSize(20, 20);
//    mainLayout->addSpacing(40);
//    mainLayout->addWidget(totaluseLabel, 0, Qt::AlignLeft);
//    mainLayout->addStretch();




}

void InfoShowWidget::bottomFramSettings()
{
    QVBoxLayout *infolayout = new QVBoxLayout();
    QVBoxLayout *info1layout = new QVBoxLayout();
    DLabel *m_label = new  DLabel("mount point:");
    m_label->setAlignment(Qt::AlignLeft);
    m_label->setWordWrap(true);
    m_label->adjustSize();
    DFontSizeManager::instance()->bind(m_label, DFontSizeManager::T5);
    DLabel *m_label1 = new  DLabel("The available capacity:");
    m_label1->setWordWrap(true);
    m_label1->adjustSize();
    m_label1->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_label1, DFontSizeManager::T5);
    DLabel *m_label2 = new  DLabel("Used space:");
    m_label2->setWordWrap(true);
    m_label2->adjustSize();
    m_label2->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_label2, DFontSizeManager::T5);
    DLabel *m_label3 = new  DLabel("type:");
    m_label3->setWordWrap(true);
    m_label3->adjustSize();
    DFontSizeManager::instance()->bind(m_label3, DFontSizeManager::T5);
    DLabel *m_label4 = new  DLabel("The total capacity:");
    m_label4->setWordWrap(true);
    m_label4->adjustSize();
    DFontSizeManager::instance()->bind(m_label4, DFontSizeManager::T5);
    DLabel *m_label5 = new  DLabel("Partition volume label:");
    m_label5->setWordWrap(true);
    m_label5->adjustSize();
    DFontSizeManager::instance()->bind(m_label5, DFontSizeManager::T5);
    infolayout->addWidget(m_label);
    infolayout->addSpacing(20);
    infolayout->addWidget(m_label1);
    infolayout->addSpacing(20);
    infolayout->addWidget(m_label2);
    infolayout->addStretch();
    info1layout->addWidget(m_label3);
    info1layout->addSpacing(20);
    info1layout->addWidget(m_label4);
    info1layout->addSpacing(20);
    info1layout->addWidget(m_label5);
    info1layout->addStretch();
    QHBoxLayout *mainlayout = new QHBoxLayout(pframebottom);
    mainlayout->addLayout(infolayout);
    mainlayout->addLayout(info1layout);


}

void InfoShowWidget::slotCurSelectChanged()
{
    qDebug() << __FUNCTION__ << "-0--0-";
    pframebottom->setFrameData(DMDbusHandler::instance()->getCurPartititonInfo());
    m_noused = Utils::sector_to_unit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_unused, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sector_to_unit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_used, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    QColor fillcolor = this->palette().color(DPalette::Normal, DPalette::Highlight);
    QColor fillcolor1 = this->palette().color(DPalette::Normal, DPalette::ToolTipText);
    fillcolor1.setAlphaF(0.1);
    QVector<QColor>color{fillcolor, fillcolor1};
    QVector<double>size{m_used, m_noused};
    m_infowidget->setdata(DMDbusHandler::instance()->getCurPartititonInfo(), color, size, 1);

    m_pInfoTopFrame->setShowDiskInfo();
}
