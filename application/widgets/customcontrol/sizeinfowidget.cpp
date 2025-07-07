// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "sizeinfowidget.h"

#include <DFontSizeManager>

#include <QDebug>
#include <QApplication>

SizeInfoWidget::SizeInfoWidget(QWidget *parent)
    : QWidget(parent)
{
    qDebug() << "SizeInfoWidget constructor";
#if QT_VERSION_MAJOR > 5
    m_parentPb = DGuiApplicationHelper::instance()->applicationPalette();;
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::onHandleChangeTheme);
#else
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::onHandleChangeTheme);
#endif
}
SizeInfoWidget::SizeInfoWidget(double used, double unused, bool flag, QWidget *parent)
    : QWidget(parent)
    , m_used(used)
    , m_noused(unused)
    , m_flag(flag)
{
    qDebug() << "SizeInfoWidget constructor with data, used:" << used << "GiB, unused:" << unused << "GiB";
#if QT_VERSION_MAJOR > 5
    m_parentPb = DGuiApplicationHelper::instance()->applicationPalette();;
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::onHandleChangeTheme);
#else
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &SizeInfoWidget::onHandleChangeTheme);
#endif
}

void SizeInfoWidget::setData(PartitionInfo info, QVector<QColor> color, QVector<double> size, bool flag)
{
    qDebug() << "Setting partition data, path:" << info.m_path << "size:" << size;
    m_sizeInfo = size;
    m_colorInfo = color;
    m_flag = flag;

    m_noused = Utils::sectorToUnit(info.m_sectorsUnused, info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_used = Utils::sectorToUnit(info.m_sectorsUsed, info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_totalSpaceSize = Utils::formatSize(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize);
    m_usedSize = Utils::formatSize(info.m_sectorsUsed, info.m_sectorSize);

    if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
        qDebug() << "Partition is LUKS encrypted.";
        LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
        if (luksInfo.isDecrypt) {
            qDebug() << "LUKS partition is decrypted, updating used/unused size.";
            m_usedSize = Utils::LVMFormatSize(luksInfo.m_mapper.m_fsUsed);
            m_noused = Utils::LVMSizeToUnit(luksInfo.m_mapper.m_fsUnused, SIZE_UNIT::UNIT_GIB);
            m_used = Utils::LVMSizeToUnit(luksInfo.m_mapper.m_fsUsed, SIZE_UNIT::UNIT_GIB);
        }
    }

    m_totalSize = m_noused + m_used;
    m_partitionPath = info.m_path;
    if ("unallocated" != m_partitionPath) {
        qDebug() << "Partition path is not unallocated, removing '/dev/'.";
        m_partitionPath = m_partitionPath.remove(0, 5);
    }

    if (m_totalSize <= 0) {
        qDebug() << "Total size is zero or less, recalculating from sector info.";
        m_totalSize = Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    }

    if (size.at(0) < 0.00 || size.at(1) < 0.00) {
        qDebug() << "Size vector contains negative values, setting to 0.00.";
        m_sizeInfo = QVector<double> {0.00, 0.00};
    }

    update();
    qDebug() << "Partition data set, total:" << m_totalSize << "GiB, used:" << m_used << "GiB";
}

void SizeInfoWidget::setData(LVInfo info, QVector<QColor> color, QVector<double> size, bool flag)
{
    qDebug() << "Setting LV data, name:" << info.m_lvName << "size:" << size;
    m_sizeInfo = size;
    m_colorInfo = color;
    m_flag = flag;
    m_partitionPath = info.m_lvName;
    m_totalSpaceSize = info.m_lvSize;
    m_used = Utils::LVMSizeToUnit(info.m_fsUsed, SIZE_UNIT::UNIT_GIB);
    m_noused = Utils::LVMSizeToUnit(info.m_fsUnused, SIZE_UNIT::UNIT_GIB);
    m_usedSize = Utils::LVMFormatSize(info.m_fsUsed);

    if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS) {
        qDebug() << "LV is LUKS encrypted.";
        LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_lvPath);
        if (luksInfo.isDecrypt) {
            qDebug() << "LUKS LV is decrypted, updating used/unused size.";
            m_usedSize = Utils::LVMFormatSize(luksInfo.m_mapper.m_fsUsed);
            m_noused = Utils::LVMSizeToUnit(luksInfo.m_mapper.m_fsUnused, SIZE_UNIT::UNIT_GIB);
            m_used = Utils::LVMSizeToUnit(luksInfo.m_mapper.m_fsUsed, SIZE_UNIT::UNIT_GIB);
        } else {
            qDebug() << "LUKS LV is not decrypted, setting used size to '-'.";
            m_usedSize = "-";
        }
    }

    if (m_totalSpaceSize.contains("1024")) {
        qDebug() << "Total space size contains '1024', recalculating.";
        m_totalSpaceSize = Utils::LVMFormatSize(info.m_lvLECount * info.m_LESize + info.m_LESize);
    }

    if (info.m_lvName.isEmpty() && info.m_lvUuid.isEmpty()) {
        qDebug() << "LV name and UUID are empty, setting noused based on LECount.";
        m_noused = Utils::LVMSizeToUnit(info.m_lvLECount * info.m_LESize, SIZE_UNIT::UNIT_GIB);
    }

    m_totalSize = m_noused + m_used;
    if (m_totalSize <= 0) {
        qDebug() << "Total size is zero or less, recalculating from LECount.";
        m_totalSize = Utils::LVMSizeToUnit(info.m_lvLECount * info.m_LESize, SIZE_UNIT::UNIT_GIB);
    }

    if (size.at(0) < 0.00 || size.at(1) < 0.00) {
        qDebug() << "Size vector contains negative values, setting to 0.00.";
        m_sizeInfo = QVector<double> {0.00, 0.00};
    }
    update();
    qDebug() << "LV data set, total:" << m_totalSize << "GiB, used:" << m_used << "GiB";
}

void SizeInfoWidget::setData(DeviceInfo info, QVector<QColor> color, QVector<double> size, bool flag)
{
    qDebug() << "Setting device data, path:" << info.m_path << "size:" << size;
    m_sizeInfo = size;
    m_colorInfo = color;
    m_flag = flag;
    m_partitionPath = info.m_path;
    m_totalSpaceSize = Utils::formatSize(info.m_length, info.m_sectorSize);
    m_used = 0.00;
    m_noused = Utils::LVMSizeToUnit(info.m_length * info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    m_usedSize = Utils::LVMFormatSize(-1 * info.m_sectorSize);

    if (info.m_luksFlag == LUKSFlag::IS_CRYPT_LUKS && info.m_partition.size() == 0) {
        qDebug() << "Device is LUKS encrypted and has no partitions.";
        LUKS_INFO luksInfo = DMDbusHandler::instance()->probLUKSInfo().m_luksMap.value(info.m_path);
        if (luksInfo.isDecrypt) {
            qDebug() << "LUKS device is decrypted, updating used/unused size.";
            m_usedSize = Utils::LVMFormatSize(luksInfo.m_mapper.m_fsUsed);
            m_noused = Utils::LVMSizeToUnit(luksInfo.m_mapper.m_fsUnused, SIZE_UNIT::UNIT_GIB);
            m_used = Utils::LVMSizeToUnit(luksInfo.m_mapper.m_fsUsed, SIZE_UNIT::UNIT_GIB);
        }
    }

    m_totalSize = m_noused + m_used;
    if (m_totalSize <= 0) {
        qDebug() << "Total size is zero or less, recalculating from device length.";
        m_totalSize = Utils::LVMSizeToUnit(info.m_length * info.m_sectorSize, SIZE_UNIT::UNIT_GIB);
    }

    if (size.at(0) < 0.00 || size.at(1) < 0.00) {
        qDebug() << "Size vector contains negative values, setting to 0.00.";
        m_sizeInfo = QVector<double> {0.00, 0.00};
    }
    update();
    qDebug() << "Device data set, total:" << m_totalSize << "GiB, used:" << m_used << "GiB";
}

void SizeInfoWidget::paintEvent(QPaintEvent *event)
{
    // qDebug() << "SizeInfoWidget::paintEvent called.";
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect;
    QPainterPath path0, path1, path2, path3, path4, path5, path6, path7, path8, path9;
    QVector<QPainterPath> path {path0, path1, path2, path3, path4, path5, path6, path7, path8, path9};
    rect.setX(x());
    rect.setY(y());
    rect.setWidth(width());
    rect.setHeight(height());
    QRect paintRect = QRect(0, 60, rect.width() + 1, rect.height() - 170);
    QPainterPath paintpath0;
    int radius = 8;
    double sumSize = 0.00;
    // qDebug() << "Starting to draw rectangles based on color and size data.";
    //根据color和size数据遍历绘制矩形
    for (int i = 0; i < m_sizeInfo.size(); i++) {
        // qDebug() << "Processing segment:" << i << ", size:" << m_sizeInfo[i];
        if (i == 0) {
            // qDebug() << "First segment (i=0).";
            if (m_sizeInfo[0] == m_totalSize) {
                // 处理第一个值就是总大小，矩形首尾都绘制圆角
                path[0].moveTo(paintRect.bottomRight() - QPoint(0, radius));
                path[0].lineTo(paintRect.topRight() + QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                    QSize(radius * 2, radius * 2)), 0, 90);
                path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
                path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                    QSize(radius * 2, radius * 2)), 180, 90);
                path[0].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                    QSize(radius * 2, radius * 2)), 270, 90);

                painter.setBrush(QBrush(m_colorInfo[0]));
                painter.setPen(QPen(QColor(m_colorInfo[0]), 3));
                painter.fillPath(path[0], m_colorInfo[0]);
                // qDebug() << "Filled path for total size.";
            } else {
                // qDebug() << "First segment is not total size, drawing rounded rectangle for partial area.";
                path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
                path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                    QSize(radius * 2, radius * 2)),
                              180, 90);
                path[0].lineTo(paintRect.bottomLeft() + QPoint(static_cast<int>((m_sizeInfo[0] / m_totalSize) * paintRect.width() + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint(static_cast<int>((m_sizeInfo[0] / m_totalSize) * paintRect.width() + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
                if (m_sizeInfo.at(i) == 0.00) {
                    // qDebug() << "Segment size is 0.00, using colorInfo[1].";
                    painter.setBrush(QBrush(m_colorInfo[1]));
                    painter.setPen(QPen(QColor(m_colorInfo[1]), 3));
                    painter.fillPath(path[0], m_colorInfo[1]);
                } else {
                    // qDebug() << "Segment size is greater than 0.00, using colorInfo[0].";
                    painter.setBrush(QBrush(m_colorInfo[0]));
                    painter.setPen(QPen(QColor(m_colorInfo[0]), 3));
                    painter.fillPath(path[0], m_colorInfo[0]);
                }
            }

            sumSize += m_sizeInfo[0];
            // qDebug() << "sumSize after first segment:" << sumSize;
        } else if (i > 0 && i < m_sizeInfo.size() - 1) {
            // qDebug() << "Middle segment (i > 0 && i < size - 1).";
            path[i].moveTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width() + (m_sizeInfo[i] / m_totalSize) * paintRect.width()), 0));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width() + (m_sizeInfo[i] / m_totalSize) * paintRect.width()), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), paintRect.height()));
            path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), 0));
            painter.setBrush(QBrush(m_colorInfo[i]));
            painter.setPen(QPen(QColor(m_colorInfo[i]), 3));
            painter.fillPath(path[i], m_colorInfo[i]);
            // qDebug() << "Filled path for middle segment.";

            sumSize += m_sizeInfo[i];
            // qDebug() << "sumSize after middle segment:" << sumSize;
        } else if (i == m_sizeInfo.size() - 1) {
            // qDebug() << "Last segment (i == size - 1).";
            path[m_sizeInfo.size() - 1].moveTo(paintRect.bottomRight() - QPoint(0, radius));
            path[m_sizeInfo.size() - 1].lineTo(paintRect.topRight() + QPoint(0, radius));
            path[m_sizeInfo.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                  QSize(radius * 2, radius * 2)),
                                            0, 90);
            path[m_sizeInfo.size() - 1].lineTo(path[m_sizeInfo.size() - 2].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[m_sizeInfo.size() - 2] / m_totalSize) * paintRect.width()), 0));
            path[m_sizeInfo.size() - 1].lineTo(path[m_sizeInfo.size() - 2].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[m_sizeInfo.size() - 2] / m_totalSize) * paintRect.width()), paintRect.height() - 1));
            path[m_sizeInfo.size() - 1].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
            path[m_sizeInfo.size() - 1].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                                  QSize(radius * 2, radius * 2)),
                                            270, 90);
            painter.setBrush(QBrush(m_colorInfo[m_sizeInfo.size() - 1]));
            painter.setPen(QPen(QColor(m_colorInfo[m_sizeInfo.size() - 1]), 3));
            if ((m_totalSize - sumSize) > 0.00) {
                // qDebug() << "Remaining total size is positive, filling path for last segment.";
                painter.fillPath(path[m_sizeInfo.size() - 1], m_colorInfo[m_sizeInfo.size() - 1]);
            }
            // qDebug() << "Last segment processing complete.";
        }
    }
    // qDebug() << "Finished drawing rectangles. Starting to draw information labels.";
    //绘制首页下方标注
    if (m_flag) {
        // qDebug() << "Flag is true, drawing information labels.";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        // qDebug() << "Theme type for info labels:" << themeType;
        if (themeType == DGuiApplicationHelper::LightType) {
            // qDebug() << "Theme is LightType for info labels.";
            int height = 90 - static_cast<int>((QApplication::font().pointSizeF() / 0.75 - 14) * 1);
            QRect roundRect = QRect(rect.bottomLeft().x() + 2, rect.bottomLeft().y() - height, 15, 15);
            //            rect.setWidth(rect.width() - 1);
            //            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(roundRect, 3, 3);
            //            QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
#if QT_VERSION_MAJOR > 5
            m_parentPb = DGuiApplicationHelper::instance()->applicationPalette();;
            QBrush brush1 = m_parentPb.dark();
#else
            m_parentPb = DApplicationHelper::instance()->palette(this);
            QBrush brush1 = DApplicationHelper::instance()->palette(this).dark();
#endif
            QColor iconColor = m_parentPb.color(DPalette::Normal, DPalette::Dark);
            painter.setBrush(brush1);
            painter.setPen(iconColor);
            painter.fillRect(roundRect, brush1);
            //            QRect Round1rect1(rect.bottomLeft().x() + paintRect.width() / 2, paintRect.bottomLeft().y() + 20, 15, 15);

            QFont font;
            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textColor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter.setFont(font);
            painter.setPen(textColor);
            QTextOption option;
            option.setAlignment(Qt::AlignTop);

            QFontMetrics fmCapacity = painter.fontMetrics();
            QString textCapacity = QString(m_partitionPath + tr(" Capacity:") + " ");
#if QT_VERSION_MAJOR > 5
            int capacityWidth = fmCapacity.boundingRect(textCapacity).width();
#else
            int capacityWidth = fmCapacity.width(textCapacity);
#endif
            if (capacityWidth > rect.width() / 2) {
                capacityWidth = rect.width() / 2;
                textCapacity = painter.fontMetrics().elidedText(textCapacity, Qt::ElideMiddle, capacityWidth);
#if QT_VERSION_MAJOR > 5
                capacityWidth = fmCapacity.boundingRect(textCapacity).width();
#else
                capacityWidth = fmCapacity.width(textCapacity);
#endif
            }
            QRect rectText = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, capacityWidth, 70);
            painter.drawText(rectText, textCapacity, option);
            // qDebug() << "Drew capacity text.";
            // 获取总容量字符串显示的宽度
            int capacityNum = rectText.x() + capacityWidth;

            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QColor text1Color = m_parentPb.color(DPalette::Normal, DPalette::TextTitle);
            painter.setFont(font);
            painter.setPen(text1Color);
            QTextOption option1;
            option.setAlignment(Qt::AlignLeft);
#if QT_VERSION_MAJOR > 5
            QRect rectSizeNum = QRect(paintRect.bottomLeft().x() + capacityNum, paintRect.bottomLeft().y() + 20, painter.fontMetrics().boundingRect(m_totalSpaceSize).width() + 20, 30);
#else
            QRect rectSizeNum = QRect(paintRect.bottomLeft().x() + capacityNum, paintRect.bottomLeft().y() + 20, painter.fontMetrics().width(m_totalSpaceSize) + 20, 30);
#endif

            int height2 = 21 + static_cast<int>((QApplication::font().pointSizeF() / 0.75 - 14) * 1);
            int width = roundRect.x() + capacityNum + rectSizeNum.width() + 30;
            roundRect.moveTo(width, paintRect.bottomLeft().y() + height2);
            QPainterPath painterPath;
            painterPath.addRoundedRect(roundRect, 3, 3);
#if QT_VERSION_MAJOR > 5
            QBrush brush2 = DGuiApplicationHelper::instance()->applicationPalette().light();
#else
            QBrush brush2 = DApplicationHelper::instance()->palette(this).lightLively();
#endif
            QColor icon2Color = m_parentPb.color(DPalette::Normal, DPalette::LightLively);
            painter.setBrush(brush2);
            painter.setPen(icon2Color);
            painter.drawPath(painterPath);
            painter.fillPath(painterPath, brush2);
            // qDebug() << "Drew and filled painter path.";

            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            textColor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter.setFont(font);
            painter.setPen(textColor);
            option.setAlignment(Qt::AlignTop);
            width = roundRect.x() + roundRect.width() + 10;

            QFontMetrics fmUsed = painter.fontMetrics();
#if QT_VERSION_MAJOR > 5
            int usedWidth = fmCapacity.boundingRect(QString(tr("Used:") + " ")).width();
#else
            int usedWidth = fmCapacity.width(QString(tr("Used:") + " "));
#endif

//            rectText.moveTo(width, paintRect.bottomLeft().y() + 17);
            rectText = QRect(width, paintRect.bottomLeft().y() + 17, usedWidth, 70);
            painter.drawText(rectText, QString(tr("Used:") + " "), option);
            // qDebug() << "Drew Used: text for LightType.";
            // 获取已用空间字符串显示的宽度
            int usedNum = rectText.x() + usedWidth;

            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            text1Color = m_parentPb.color(DPalette::Normal, DPalette::TextTitle);
            painter.setFont(font);
            painter.setPen(text1Color);
            option.setAlignment(Qt::AlignLeft);
            painter.drawText(rectSizeNum, m_totalSpaceSize, option1);
            // qDebug() << "Drew total space size for LightType.";

            rectSizeNum.moveTo(paintRect.bottomLeft().x() + usedNum, paintRect.bottomLeft().y() + 20);
            if (m_usedSize.contains("-")) {
                // qDebug() << "Used size contains '-', setting to '-' for LightType.";
                m_usedSize = "-";
            }
            painter.drawText(rectSizeNum, m_usedSize, option1);
            // qDebug() << "Drew used size for LightType.";
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            // qDebug() << "Theme is DarkType for info labels.";
            int height = 90 - static_cast<int>((QApplication::font().pointSizeF() / 0.75 - 14) * 1);
            QRect roundRect = QRect(rect.bottomLeft().x() + 2, rect.bottomLeft().y() - height, 15, 15);
            //            rect.setWidth(rect.width() - 1);
            //            rect.setHeight(rect.height() - 1);
            painter.drawRoundedRect(roundRect, 3, 3);
            //            QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
#if QT_VERSION_MAJOR > 5
            m_parentPb = DGuiApplicationHelper::instance()->applicationPalette();;
            QBrush brush1 = m_parentPb.dark();
#else
            m_parentPb = DApplicationHelper::instance()->palette(this);
            QBrush brush1 = DApplicationHelper::instance()->palette(this).dark();
#endif
            QColor iconColor = m_parentPb.color(DPalette::Normal, DPalette::Dark);
            painter.setBrush(brush1);
            painter.setPen(iconColor);
            painter.fillRect(roundRect, brush1);
            //            QRect Round1rect1(rect.bottomLeft().x() + paintRect.width() / 2, paintRect.bottomLeft().y() + 20, 15, 15);

            QFont font;
            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            QColor textColor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter.setFont(font);
            painter.setPen(textColor);
            QTextOption option;
            option.setAlignment(Qt::AlignTop);

            QFontMetrics fmCapacity = painter.fontMetrics();
            QString textCapacity = QString(m_partitionPath + tr(" Capacity:") + " ");
#if QT_VERSION_MAJOR > 5
            int capacityWidth = fmCapacity.boundingRect(textCapacity).width();
#else
            int capacityWidth = fmCapacity.width(textCapacity);
#endif
            if (capacityWidth > rect.width() / 2) {
                capacityWidth = rect.width() / 2;
                textCapacity = painter.fontMetrics().elidedText(textCapacity, Qt::ElideMiddle, capacityWidth);
#if QT_VERSION_MAJOR > 5
                capacityWidth = fmCapacity.boundingRect(textCapacity).width();
#else
                capacityWidth = fmCapacity.width(textCapacity);
#endif
            }
            QRect rectText = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, capacityWidth, 70);
            painter.drawText(rectText, textCapacity, option);
            // qDebug() << "Drew capacity text for DarkType.";
            // 获取总容量字符串显示的宽度
            int capacityNum = rectText.x() + capacityWidth;

            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            QColor text1Color = m_parentPb.color(DPalette::Normal, DPalette::TextTitle);
            painter.setFont(font);
            painter.setPen(text1Color);
            QTextOption option1;
            option.setAlignment(Qt::AlignLeft);
#if QT_VERSION_MAJOR > 5
            QRect rectSizeNum = QRect(paintRect.bottomLeft().x() + capacityNum, paintRect.bottomLeft().y() + 20, painter.fontMetrics().boundingRect(m_totalSpaceSize).width() + 20, 30);
#else
            QRect rectSizeNum = QRect(paintRect.bottomLeft().x() + capacityNum, paintRect.bottomLeft().y() + 20, painter.fontMetrics().width(m_totalSpaceSize) + 20, 30);
#endif

            int height2 = 21 + static_cast<int>((QApplication::font().pointSizeF() / 0.75 - 14) * 1);
            int width = roundRect.x() + capacityNum + rectSizeNum.width() + 30;
            roundRect.moveTo(width, paintRect.bottomLeft().y() + height2);
            QPainterPath painterPath;
            painterPath.addRoundedRect(roundRect, 3, 3);
#if QT_VERSION_MAJOR > 5
            QBrush brush2 = DGuiApplicationHelper::instance()->applicationPalette().light();
#else
            QBrush brush2 = DApplicationHelper::instance()->palette(this).lightLively();
#endif
            QColor icon2Color = m_parentPb.color(DPalette::Normal, DPalette::LightLively);
            painter.setBrush(brush2);
            painter.setPen(icon2Color);
            painter.drawPath(painterPath);
            painter.fillPath(painterPath, brush2);
            // qDebug() << "Drew and filled painter path for DarkType.";

            font = DFontSizeManager::instance()->get(DFontSizeManager::T6);
            textColor = m_parentPb.color(DPalette::Normal, DPalette::Text);
            painter.setFont(font);
            painter.setPen(textColor);
            option.setAlignment(Qt::AlignTop);
            width = roundRect.x() + roundRect.width() + 10;

            QFontMetrics fmUsed = painter.fontMetrics();
#if QT_VERSION_MAJOR > 5
            int usedWidth = fmCapacity.boundingRect(QString(tr("Used:") + " ")).width();
#else
            int usedWidth = fmCapacity.width(QString(tr("Used:") + " "));
#endif
//            rectText.moveTo(width, paintRect.bottomLeft().y() + 17);
            rectText = QRect(width, paintRect.bottomLeft().y() + 17, usedWidth, 70);
            painter.drawText(rectText, QString(tr("Used:") + " "), option);
            // qDebug() << "Drew Used: text for DarkType.";
            // 获取已用空间字符串显示的宽度
            int usedNum = rectText.x() + usedWidth;

            font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
            text1Color = m_parentPb.color(DPalette::Normal, DPalette::TextTitle);
            painter.setFont(font);
            painter.setPen(text1Color);
            option.setAlignment(Qt::AlignLeft);
            painter.drawText(rectSizeNum, m_totalSpaceSize, option1);
            // qDebug() << "Drew total space size for DarkType.";

            rectSizeNum.moveTo(paintRect.bottomLeft().x() + usedNum, paintRect.bottomLeft().y() + 20);
            if (m_usedSize.contains("-")) {
                // qDebug() << "Used size contains '-', setting to '-' for DarkType.";
                m_usedSize = "-";
            }
            painter.drawText(rectSizeNum, m_usedSize, option1);
            // qDebug() << "Drew used size for DarkType.";
        }
    }
    painter.restore();
    // qDebug() << "Paint event completed.";
}
void SizeInfoWidget::onHandleChangeTheme()
{
    qDebug() << "SizeInfoWidget::onHandleChangeTheme called, theme type changed.";
    m_parentPb = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
}
