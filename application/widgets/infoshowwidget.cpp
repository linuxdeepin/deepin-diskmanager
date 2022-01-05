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
#include "customcontrol/partitioninfowidget.h"

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
    m_frame->setLineWidth(0);

    QVBoxLayout *frameLayout = new QVBoxLayout(m_frame);
    frameLayout->setContentsMargins(10, 10, 10, 10);

    //  右侧 上部数据信息
    m_infoTopFrame = new InfoTopFrame(this);
    frameLayout->addWidget(m_infoTopFrame);
    m_infoTopFrame->setMinimumWidth(450);
    m_infoTopFrame->setLineWidth(0);

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
    connect(m_partitionInfoWidget, &PartitionInfoWidget::enterWidget, this, &InfoShowWidget::onEnterWidget);
    connect(m_partitionInfoWidget, &PartitionInfoWidget::leaveWidget, this, &InfoShowWidget::onLeaveWidget);
}

void InfoShowWidget::midFramSettings()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(m_frameMid);
    m_infoWidget = new SizeInfoWidget;
    m_partitionInfoWidget = new PartitionInfoWidget;
    mainLayout->addWidget(m_infoWidget);
    mainLayout->addWidget(m_partitionInfoWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_partitionInfoWidget->hide();

    m_arrowRectangle = new DArrowRectangle(DArrowRectangle::ArrowBottom, this);
    QColor color("#F8F8F8");
    color.setAlphaF(0.3);
    m_arrowRectangle->setBackgroundColor(color);
    m_arrowRectangle->setArrowWidth(20);
    m_arrowRectangle->setArrowHeight(10);
    m_arrowRectangle->setRadius(18);
    m_arrowRectangle->setWidth(262);
    m_arrowRectangle->setHeight(20);

    m_pathLabel = new DLabel("/dev/sda");
    DFontSizeManager::instance()->bind(m_pathLabel, DFontSizeManager::T8, QFont::Normal);
    m_pathLabel->setAccessibleName("partitionPath");
    DPalette palette;
    palette.setColor(DPalette::WindowText, QColor("#001A2E"));
    m_pathLabel->setPalette(palette);
    m_pathLabel->setAlignment(Qt::AlignCenter);
    m_pathLabel->setFixedHeight(18);
    m_arrowRectangle->setContent(m_pathLabel);
}

void InfoShowWidget::bottomFramSettings()
{
    QVBoxLayout *leftInfoLayout = new QVBoxLayout();
    QVBoxLayout *rightInfolayout = new QVBoxLayout();

    m_mountpointLabel = new DLabel(tr("Mount point:"));
    m_mountpointLabel->setAlignment(Qt::AlignLeft);
//    m_mountpointLabel->setWordWrap(true);
//    m_mountpointLabel->adjustSize();
    m_mountpointLabel->setFixedHeight(30);
    DFontSizeManager::instance()->bind(m_mountpointLabel, DFontSizeManager::T6, QFont::Medium);
    m_mountpointLabel->setAccessibleName("Mount point");

    m_freeLabel = new DLabel(tr("Free:"));
//    m_freeLabel->setWordWrap(true);
//    m_freeLabel->adjustSize();
    m_freeLabel->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_freeLabel, DFontSizeManager::T6, QFont::Medium);
    m_freeLabel->setAccessibleName("Free");

    m_usedLabel = new DLabel(tr("Used:"));
//    m_usedLabel->setWordWrap(true);
//    m_usedLabel->adjustSize();
    m_usedLabel->setAlignment(Qt::AlignLeft);
    DFontSizeManager::instance()->bind(m_usedLabel, DFontSizeManager::T6, QFont::Medium);
    m_usedLabel->setAccessibleName("Used");

    m_typeLabel = new DLabel(tr("Type:"));
//    m_typeLabel->setWordWrap(true);
//    m_typeLabel->adjustSize();
    DFontSizeManager::instance()->bind(m_typeLabel, DFontSizeManager::T6, QFont::Medium);
    m_typeLabel->setAccessibleName("Type");

    m_capacityLabel = new DLabel(tr("Capacity:"));
//    m_capacityLabel->setWordWrap(true);
//    m_capacityLabel->adjustSize();
    DFontSizeManager::instance()->bind(m_capacityLabel, DFontSizeManager::T6, QFont::Medium);
    m_capacityLabel->setAccessibleName("Capacity");

    m_volumeLabel = new DLabel(tr("Volume label:"));
//    m_volumeLabel->setWordWrap(true);
//    m_volumeLabel->adjustSize();
    DFontSizeManager::instance()->bind(m_volumeLabel, DFontSizeManager::T6, QFont::Medium);
    m_volumeLabel->setAccessibleName("Volume label");

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
    this, [ = ] {
        DPalette pa;
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType) {
            pa = palette();
            pa.setColor(DPalette::Text, QColor("#414d68"));

            m_mountpointLabel->setPalette(pa);
            m_freeLabel->setPalette(pa);
            m_usedLabel->setPalette(pa);
            m_typeLabel->setPalette(pa);
            m_capacityLabel->setPalette(pa);
            m_volumeLabel->setPalette(pa);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            pa = palette();
            pa.setColor(DPalette::Text, QColor("#c0c6d4"));

            m_mountpointLabel->setPalette(pa);
            m_freeLabel->setPalette(pa);
            m_usedLabel->setPalette(pa);
            m_typeLabel->setPalette(pa);
            m_capacityLabel->setPalette(pa);
            m_volumeLabel->setPalette(pa);
        }
    });

    DPalette palette = DApplicationHelper::instance()->palette(m_mountpointLabel);
    palette.setBrush(DPalette::Text, palette.color(DPalette::Text));
    DApplicationHelper::instance()->setPalette(m_mountpointLabel, palette);
    DApplicationHelper::instance()->setPalette(m_freeLabel, palette);
    DApplicationHelper::instance()->setPalette(m_usedLabel, palette);
    DApplicationHelper::instance()->setPalette(m_typeLabel, palette);
    DApplicationHelper::instance()->setPalette(m_capacityLabel, palette);
    DApplicationHelper::instance()->setPalette(m_volumeLabel, palette);

    leftInfoLayout->addSpacing(1);
    leftInfoLayout->addWidget(m_mountpointLabel, 0, Qt::AlignLeft);
    leftInfoLayout->addSpacing(18);
    leftInfoLayout->addWidget(m_freeLabel, 0, Qt::AlignLeft);
    leftInfoLayout->addSpacing(22);
    leftInfoLayout->addWidget(m_usedLabel, 0, Qt::AlignLeft);
    leftInfoLayout->addStretch();

    rightInfolayout->addSpacing(1);
    rightInfolayout->addWidget(m_typeLabel, 0, Qt::AlignLeft);
    rightInfolayout->addSpacing(25);
    rightInfolayout->addWidget(m_capacityLabel, 0, Qt::AlignLeft);
    rightInfolayout->addSpacing(23);
    rightInfolayout->addWidget(m_volumeLabel, 0, Qt::AlignLeft);
    rightInfolayout->addStretch();

    QHBoxLayout *mainlayout = new QHBoxLayout(m_frameBottom);
    mainlayout->addLayout(leftInfoLayout);
    mainlayout->addSpacing(10);
    mainlayout->addLayout(rightInfolayout);
}

void InfoShowWidget::onCurSelectChanged()
{
    qDebug() << __FUNCTION__ << "-0--0-";
    if (1 == DMDbusHandler::instance()->getCurLevel()) {
        m_partitionInfoWidget->hide();
        m_infoWidget->show();
        m_frameBottom->setFrameData();
        m_mountpointLabel->setText(tr("Mount point:"));
        m_typeLabel->setText(tr("Type:"));
        m_volumeLabel->setText(tr("Volume label:"));
        m_noused = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorsUnused, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize, SIZE_UNIT::UNIT_GIB);
        m_used = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorsUsed, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize, SIZE_UNIT::UNIT_GIB);

        QString mountpoints;
        for (QString point : DMDbusHandler::instance()->getCurPartititonInfo().m_mountPoints) {
            mountpoints.append(point + " ");
        }
        m_mountpointLabel->setObjectName(QString("@==@%1").arg(mountpoints));
        QString free = Utils::formatSize(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorsUnused, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize);
        if (free.contains("-")){
            free = "-";
        }
        m_freeLabel->setObjectName(QString("@==@%1").arg(free));
        QString used = Utils::formatSize(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorsUsed, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize);
        if (used.contains("-")){
            used = "-";
        }
        m_usedLabel->setObjectName(QString("@==@%1").arg(used));
        m_typeLabel->setObjectName(QString("@==@%1").arg(Utils::fileSystemTypeToString(static_cast<FSType>(DMDbusHandler::instance()->getCurPartititonInfo().m_fileSystemType))));
        m_capacityLabel->setObjectName(QString("@==@%1").arg(Utils::formatSize(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorEnd - DMDbusHandler::instance()->getCurPartititonInfo().m_sectorStart, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize)));
        m_volumeLabel->setObjectName(QString("@==@%1").arg(DMDbusHandler::instance()->getCurPartititonInfo().m_fileSystemLabel));

        DPalette palette;
    //    typeLabel->setText(typeLabel->objectName());
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
    } else {
        m_partitionInfoWidget->show();
        m_infoWidget->hide();
        DeviceInfo info = DMDbusHandler::instance()->getCurDeviceInfo();
        Sector usedSector = 0;
        Sector unusedSector = 0;
        for (int i = 0; i < info.m_partition.size(); i++) {
            PartitionInfo partitionInfo = info.m_partition.at(i);
            if (partitionInfo.m_path != "unallocated") {
                usedSector += partitionInfo.m_sectorsUsed;
                unusedSector += partitionInfo.m_sectorsUnused;
            } else {
                unusedSector += partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1;
            }
        }

        QString diskSize = Utils::formatSize(info.m_length, info.m_sectorSize);
        QString used = Utils::formatSize(usedSector, info.m_sectorSize);
        QString unused = Utils::formatSize(unusedSector, info.m_sectorSize);

        if (used.contains("-")) {
            used = "-";
        }

        if (unused.contains("-")) {
            unused = "-";
        }

        m_mountpointLabel->setText(tr("Path:"));
        m_typeLabel->setText(tr("Disk type:"));
        m_volumeLabel->setText(tr("Interface:"));
        m_mountpointLabel->setObjectName(QString("@==@%1").arg(info.m_path));
        m_capacityLabel->setObjectName(QString("@==@%1").arg(diskSize));
        m_freeLabel->setObjectName(QString("@==@%1").arg(unused));
        m_usedLabel->setObjectName(QString("@==@%1").arg(used));
        m_typeLabel->setObjectName(QString("@==@%1").arg(info.m_mediaType));
        m_volumeLabel->setObjectName(QString("@==@%1").arg(info.m_interface));

        m_frameBottom->setDiskFrameData(info.m_path, info.m_mediaType, used, unused, diskSize, info.m_interface);
        m_partitionInfoWidget->setData(info);
        m_infoTopFrame->setShowDiskInfo();
    }
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

    m_noused = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorsUnused, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sectorToUnit(DMDbusHandler::instance()->getCurPartititonInfo().m_sectorsUsed, DMDbusHandler::instance()->getCurPartititonInfo().m_sectorSize, SIZE_UNIT::UNIT_GIB);

    QVector<QColor> color {fillcolor, fillcolor1};
    QVector<double> size {m_used, m_noused};
    m_infoWidget->setData(DMDbusHandler::instance()->getCurPartititonInfo(), color, size, 1);
}

void InfoShowWidget::onEnterWidget(QRectF rect, QString path)
{
    if ("unallocated" != path) {
        path = path.remove(0, 5);
    }

    m_pathLabel->setText(path);
    QFontMetrics fmLabel = m_pathLabel->fontMetrics();
    int labelWidth = fmLabel.width(path);
    m_pathLabel->setFixedWidth(labelWidth + 20);

    m_arrowRectangle->show(m_partitionInfoWidget->mapToGlobal(m_partitionInfoWidget->pos()).x() + (rect.x() + rect.width() / 2),
                           m_partitionInfoWidget->mapToGlobal(m_partitionInfoWidget->pos()).y() + rect.y());
}

void InfoShowWidget::onLeaveWidget()
{
    m_arrowRectangle->hide();
}
