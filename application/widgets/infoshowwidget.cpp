/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file infoshowwidget.cpp
 *
 * @brief 分区信息展示窗口
 *
 * @date 2020-09-04 16:10
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "infoshowwidget.h"
#include "customcontrol/sizeinfowidget.h"
#include "partedproxy/dmdbushandler.h"
#include "customcontrol/infotopframe.h"
#include "customcontrol/dmframewidget.h"

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
    setWindowFlags(Qt::FramelessWindowHint); //无边框
    setAttribute(Qt::WA_TranslucentBackground); //背景透明

    QVBoxLayout *layout = new QVBoxLayout(this);
    m_frame = new DFrame();
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(m_frame);

    QVBoxLayout *frameLayout = new QVBoxLayout(m_frame);
    frameLayout->setContentsMargins(10, 10, 10, 10);

    //  右侧 上部数据信息
    m_infoTopFrame = new InfoTopFrame(this);
    frameLayout->addWidget(m_infoTopFrame);
    m_infoTopFrame->setMinimumWidth(450);

    // 右侧 中部数据信息
    m_frameMid = new DFrame;
    m_frameMid->setMinimumWidth(450);
    m_frameMid->setFixedHeight(200);
    m_frameMid->setFrameShape(QFrame::NoFrame);
    frameLayout->addWidget(m_frameMid);
    midFramSettings();

    // 右侧 底部数据信息
    m_frameBottom = new DmFrameWidget(m_diskInfoData);
    m_frameBottom->setFixedHeight(150);
    m_frameBottom->setMinimumWidth(450);
    frameLayout->addWidget(m_frameBottom);
    frameLayout->addStretch();
    bottomFramSettings();
}

void InfoShowWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::curSelectChanged, this, &InfoShowWidget::onCurSelectChanged);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &InfoShowWidget::onHandleChangeTheme);
}

void InfoShowWidget::midFramSettings()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(m_frameMid);
    m_infoWidget = new SizeInfoWidget;
    mainLayout->addWidget(m_infoWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void InfoShowWidget::bottomFramSettings()
{
    QVBoxLayout *leftInfoLayout = new QVBoxLayout();
    QVBoxLayout *rightInfolayout = new QVBoxLayout();

    DLabel *mountpointLabel = new DLabel(tr("Mount point:"));
    mountpointLabel->setAlignment(Qt::AlignLeft);
    mountpointLabel->setWordWrap(true);
    mountpointLabel->adjustSize();
    mountpointLabel->setFixedHeight(30);
    DFontSizeManager::instance()->bind(mountpointLabel, DFontSizeManager::T6);

    DLabel *freeLabel = new DLabel(tr("Free:"));
    freeLabel->setWordWrap(true);
    freeLabel->adjustSize();
    freeLabel->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(freeLabel, DFontSizeManager::T6);

    DLabel *usedLabel = new DLabel(tr("Used:"));
    usedLabel->setWordWrap(true);
    usedLabel->adjustSize();
    usedLabel->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(usedLabel, DFontSizeManager::T6);

    DLabel *typeLabel = new DLabel(tr("Type:"));
    typeLabel->setWordWrap(true);
    typeLabel->adjustSize();
    DFontSizeManager::instance()->bind(typeLabel, DFontSizeManager::T6);

    DLabel *capacityLabel = new DLabel(tr("Capacity:"));
    capacityLabel->setWordWrap(true);
    capacityLabel->adjustSize();
    DFontSizeManager::instance()->bind(capacityLabel, DFontSizeManager::T6);

    DLabel *volumeLabel = new DLabel(tr("Volume label:"));
    volumeLabel->setWordWrap(true);
    volumeLabel->adjustSize();
    DFontSizeManager::instance()->bind(volumeLabel, DFontSizeManager::T6);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
    this, [ = ] {
        DPalette pa;
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType) {
            pa = palette();
            pa.setColor(DPalette::Text, QColor("#414d68"));

            mountpointLabel->setPalette(pa);
            freeLabel->setPalette(pa);
            usedLabel->setPalette(pa);
            typeLabel->setPalette(pa);
            capacityLabel->setPalette(pa);
            volumeLabel->setPalette(pa);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            pa = palette();
            pa.setColor(DPalette::Text, QColor("#c0c6d4"));

            mountpointLabel->setPalette(pa);
            freeLabel->setPalette(pa);
            usedLabel->setPalette(pa);
            typeLabel->setPalette(pa);
            capacityLabel->setPalette(pa);
            volumeLabel->setPalette(pa);
        }
    });

    DPalette palette = DApplicationHelper::instance()->palette(mountpointLabel);
    palette.setBrush(DPalette::Text, palette.color(DPalette::Text));
    DApplicationHelper::instance()->setPalette(mountpointLabel, palette);
    DApplicationHelper::instance()->setPalette(freeLabel, palette);
    DApplicationHelper::instance()->setPalette(usedLabel, palette);
    DApplicationHelper::instance()->setPalette(typeLabel, palette);
    DApplicationHelper::instance()->setPalette(capacityLabel, palette);
    DApplicationHelper::instance()->setPalette(volumeLabel, palette);

    leftInfoLayout->addSpacing(1);
    leftInfoLayout->addWidget(mountpointLabel);
    leftInfoLayout->addSpacing(18);
    leftInfoLayout->addWidget(freeLabel);
    leftInfoLayout->addSpacing(22);
    leftInfoLayout->addWidget(usedLabel);
    leftInfoLayout->addStretch();

    rightInfolayout->addSpacing(1);
    rightInfolayout->addWidget(typeLabel);
    rightInfolayout->addSpacing(22);
    rightInfolayout->addWidget(capacityLabel);
    rightInfolayout->addSpacing(25);
    rightInfolayout->addWidget(volumeLabel);
    rightInfolayout->addStretch();

    QHBoxLayout *mainlayout = new QHBoxLayout(m_frameBottom);
    mainlayout->addLayout(leftInfoLayout);
    mainlayout->addLayout(rightInfolayout);
}

void InfoShowWidget::onCurSelectChanged()
{
    qDebug() << __FUNCTION__ << "-0--0-";
    m_frameBottom->setFrameData();
    m_noused = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_unused, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_used, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);

    DPalette palette;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        fillcolor = QColor("#0091ff");
        fillcolor1 = palette.color(DPalette::Normal, DPalette::ToolTipText);
        fillcolor1.setAlphaF(0.1);
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        fillcolor = QColor("#0059D2");
        fillcolor1 = palette.color(DPalette::Normal, DPalette::BrightText);
        fillcolor1.setAlphaF(0.2);
    }

    QVector<QColor> color {fillcolor, fillcolor1};
    QVector<double> size {m_used, m_noused};
    m_infoWidget->setData(DMDbusHandler::instance()->getCurPartititonInfo(), color, size, 1);
    m_infoTopFrame->setShowDiskInfo();
}

void InfoShowWidget::onHandleChangeTheme()
{
    DPalette palette;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        fillcolor = QColor("#0091ff");
        fillcolor1 = palette.color(DPalette::Normal, DPalette::ToolTipText);
        fillcolor1.setAlphaF(0.1);

    } else if (themeType == DGuiApplicationHelper::DarkType) {
        fillcolor = QColor("#0059D2");
        fillcolor1 = palette.color(DPalette::Normal, DPalette::BrightText);
        fillcolor1.setAlphaF(0.2);
    }

    m_noused = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_unused, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().sectors_used, DMDbusHandler::instance()->getCurPartititonInfo().sector_size, SIZE_UNIT::UNIT_GIB);

    QVector<QColor> color {fillcolor, fillcolor1};
    QVector<double> size {m_used, m_noused};
    m_infoWidget->setData(DMDbusHandler::instance()->getCurPartititonInfo(), color, size, 1);
}
