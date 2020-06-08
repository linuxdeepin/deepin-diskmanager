#include "infoshowwidget.h"
#include "customcontrol/sizeinfowidget.h"
#include "partedproxy/dmdbushandler.h"
#include "customcontrol/infotopframe.h"
#include "customcontrol/dmframewidget.h"
#include "widgetdeclare.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

DWIDGET_USE_NAMESPACE

InfoShowWidget::InfoShowWidget(DWidget *parent)
    : DFrame(parent)
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
    m_pInfoTopFrame->setMinimumWidth(450);
    pframemid = new DFrame;
    pframemid->setMinimumWidth(450);
    pframemid->setFixedHeight(200);
    pframemid->setFrameShape(QFrame::NoFrame);
    framelayout->addWidget(pframemid);
    midFramSettings();
    pframebottom = new DmFrameWidget(data);
    pframebottom->setFixedHeight(150);
    pframebottom->setMinimumWidth(450);
    framelayout->addWidget(pframebottom);
    framelayout->addStretch();
    bottomFramSettings();
}

void InfoShowWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged, this, &InfoShowWidget::slotCurSelectChanged);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &InfoShowWidget::slothandleChangeTheme);
}

void InfoShowWidget::midFramSettings()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(pframemid);
    m_infowidget = new SizeInfoWidget;
    mainLayout->addWidget(m_infowidget);
}

void InfoShowWidget::bottomFramSettings()
{
    QVBoxLayout *infolayout = new QVBoxLayout();
    QVBoxLayout *info1layout = new QVBoxLayout();
    DLabel *m_label = new DLabel(tr("Mount point:"));
    m_label->setAlignment(Qt::AlignLeft);
    m_label->setWordWrap(true);
    m_label->adjustSize();
    m_label->setFixedHeight(30);
    DFontSizeManager::instance()->bind(m_label, DFontSizeManager::T6);
    DLabel *m_label1 = new DLabel(tr("Free:"));
    m_label1->setWordWrap(true);
    m_label1->adjustSize();
    m_label1->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_label1, DFontSizeManager::T6);
    DLabel *m_label2 = new DLabel(tr("Used:"));
    m_label2->setWordWrap(true);
    m_label2->adjustSize();
    m_label2->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_label2, DFontSizeManager::T6);
    DLabel *m_label3 = new DLabel(tr("Type:"));
    m_label3->setWordWrap(true);
    m_label3->adjustSize();
    DFontSizeManager::instance()->bind(m_label3, DFontSizeManager::T6);
    DLabel *m_label4 = new DLabel(tr("Capacity:"));
    m_label4->setWordWrap(true);
    m_label4->adjustSize();
    DFontSizeManager::instance()->bind(m_label4, DFontSizeManager::T6);
    DLabel *m_label5 = new DLabel(tr("Volume label:"));
    m_label5->setWordWrap(true);
    m_label5->adjustSize();
    DFontSizeManager::instance()->bind(m_label5, DFontSizeManager::T6);
    DPalette pa = DApplicationHelper::instance()->palette(m_label);
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
    this, [ = ] {
        DPalette pa;
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
        {
            pa = palette();
            pa.setColor(DPalette::Text, QColor("#414d68"));
            m_label->setPalette(pa);
            m_label1->setPalette(pa);
            m_label2->setPalette(pa);
            m_label3->setPalette(pa);
            m_label4->setPalette(pa);
            m_label5->setPalette(pa);
        } else if (themeType == DGuiApplicationHelper::DarkType)
        {
            pa = palette();
            pa.setColor(DPalette::Text, QColor("#c0c6d4"));
            m_label->setPalette(pa);
            m_label1->setPalette(pa);
            m_label2->setPalette(pa);
            m_label3->setPalette(pa);
            m_label4->setPalette(pa);
            m_label5->setPalette(pa);
        }
    });

    pa.setBrush(DPalette::Text, pa.color(DPalette::Text));
    DApplicationHelper::instance()->setPalette(m_label, pa);
    DApplicationHelper::instance()->setPalette(m_label1, pa);
    DApplicationHelper::instance()->setPalette(m_label2, pa);
    DApplicationHelper::instance()->setPalette(m_label3, pa);
    DApplicationHelper::instance()->setPalette(m_label4, pa);
    DApplicationHelper::instance()->setPalette(m_label5, pa);
    infolayout->addSpacing(1);
    infolayout->addWidget(m_label);
    infolayout->addSpacing(18);
    infolayout->addWidget(m_label1);
    infolayout->addSpacing(22);
    infolayout->addWidget(m_label2);
    infolayout->addStretch();
    info1layout->addSpacing(1);
    info1layout->addWidget(m_label3);
    info1layout->addSpacing(22);
    info1layout->addWidget(m_label4);
    info1layout->addSpacing(25);
    info1layout->addWidget(m_label5);
    info1layout->addStretch();
    QHBoxLayout *mainlayout = new QHBoxLayout(pframebottom);
    mainlayout->addLayout(infolayout);
    mainlayout->addLayout(info1layout);
}

void InfoShowWidget::slotCurSelectChanged()
{
    qDebug() << __FUNCTION__ << "-0--0-";
    pframebottom->setFrameData();
    m_noused = Utils::sector_to_unit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_unused, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sector_to_unit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_used, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    DPalette pa;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        fillcolor = QColor("#0091ff");
        fillcolor1 = pa.color(DPalette::Normal, DPalette::ToolTipText);
        fillcolor1.setAlphaF(0.1);
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        fillcolor = QColor("#0059D2");
        fillcolor1 = pa.color(DPalette::Normal, DPalette::BrightText);
        fillcolor1.setAlphaF(0.2);
    }
    QVector<QColor> color {fillcolor, fillcolor1};
    QVector<double> size {m_used, m_noused};
    m_infowidget->setdata(DMDbusHandler::instance()->getCurPartititonInfo(), color, size, 1);
    m_pInfoTopFrame->setShowDiskInfo();
}

void InfoShowWidget::slothandleChangeTheme()
{
    DPalette pa;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        fillcolor = QColor("#0091ff");
        fillcolor1 = pa.color(DPalette::Normal, DPalette::ToolTipText);
        fillcolor1.setAlphaF(0.1);

    } else if (themeType == DGuiApplicationHelper::DarkType) {
        fillcolor = QColor("#0059D2");
        fillcolor1 = pa.color(DPalette::Normal, DPalette::BrightText);
        fillcolor1.setAlphaF(0.2);
    }
    m_noused = Utils::sector_to_unit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_unused, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sector_to_unit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_used, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);

    QVector<QColor> color {fillcolor, fillcolor1};
    QVector<double> size {m_used, m_noused};
    m_infowidget->setdata(DMDbusHandler::instance()->getCurPartititonInfo(), color, size, 1);
}
