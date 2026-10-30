// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "vginfoshowwidget.h"
#include <QDebug>

#include <DFontSizeManager>
#include <DLabel>
#include <DGuiApplicationHelper>
#include <DScrollArea>
#include <DScrollBar>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QApplication>
#include <QScrollArea>

VGInfoShowWidget::VGInfoShowWidget(QWidget *parent) : QWidget(parent)
{
    qCDebug(appLog) << "VGInfoShowWidget initialized";
    initUi();
    initConnection();
}

void VGInfoShowWidget::initUi()
{
    qCDebug(appLog) << "Initializing VGInfoShowWidget UI.";
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    qCDebug(appLog) << "VGInfoShowWidget UI initialized.";
}

void VGInfoShowWidget::initConnection()
{
    qCDebug(appLog) << "Initializing VGInfoShowWidget connections.";
}

void VGInfoShowWidget::setData(const QList< QMap<QString, QVariant> > &lstInfo)
{
    qCDebug(appLog) << "setData called, setting VG info data, items count:" << lstInfo.count();
    QList<QObject *> lstCylinderWidget = children();
    for (int i = 0; i < lstCylinderWidget.count(); i ++) {
        QObject *obj = lstCylinderWidget.at(i);
        delete obj;
        obj = nullptr;
    }

    QFont fontName;
    fontName.setWeight(QFont::Medium);
    fontName.setFamily("Source Han Sans");
    fontName.setPixelSize(14);

    QFont fontSize;
    fontSize.setWeight(QFont::Normal);
    fontSize.setFamily("Source Han Sans");
    fontSize.setPixelSize(12);

    DPalette paletteText;
    QPalette paletteBackground;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    qCDebug(appLog) << "Current theme type:" << (themeType == DGuiApplicationHelper::LightType ? "Light" : "Dark");
    if (themeType == DGuiApplicationHelper::LightType) {
        qCDebug(appLog) << "Theme is LightType, setting palette colors for light theme.";
        QColor color("#F7F7F7");
        color.setAlphaF(0.6);
        paletteBackground.setColor(QPalette::Base, color);

        paletteText.setColor(DPalette::Text, QColor("#000000"));
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        qCDebug(appLog) << "Theme is DarkType, setting palette colors for dark theme.";
        QColor color("#080808");
        color.setAlphaF(0.6);
        paletteBackground.setColor(QPalette::Base, color);

        paletteText.setColor(DPalette::Text, QColor("#C0C6D4"));
    }

    QGridLayout *m_gridLayout = new QGridLayout;
    for (int i = 0; i< lstInfo.count(); i++) {
        QMap<QString, QVariant> mapInfo = lstInfo.at(i);
        QString path = mapInfo["name"].toString();
        QString size = mapInfo["size"].toString();
        QColor color = mapInfo["color"].value<QColor>();
        // qCDebug(appLog) << "Processing item" << i << ": path=" << path << ", size=" << size;

        DLabel *m_label = new DLabel(this);
        m_label->setFixedSize(16, 16);
        m_label->setFrameShape(QFrame::Box);
        m_label->setObjectName(QString("%1").arg(i));
        QString strColor = QString("rgba(%1, %2, %3, %4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alphaF());
        m_label->setStyleSheet(QString("background:%1;border:0px;border-radius:4px;").arg(strColor));

        DLabel *m_nameLabel = new DLabel(path, this);
        m_nameLabel->setFont(fontName);
        m_nameLabel->setPalette(paletteText);
        DLabel *m_sizeLabel = new DLabel(size, this);
        m_sizeLabel->setFont(fontSize);
        m_sizeLabel->setPalette(paletteText);

        QVBoxLayout *layoutLeft = new QVBoxLayout;
        layoutLeft->addSpacing(2);
        layoutLeft->addWidget(m_label);
        layoutLeft->addStretch();
        layoutLeft->setSpacing(0);
        layoutLeft->setContentsMargins(0, 0, 0, 0);

        QVBoxLayout *layoutRight = new QVBoxLayout;
        layoutRight->addWidget(m_nameLabel);
        layoutRight->addWidget(m_sizeLabel);
        layoutRight->setSpacing(0);
        layoutRight->setContentsMargins(0, 0, 0, 0);

        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addLayout(layoutLeft);
        hLayout->addLayout(layoutRight);
        hLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget = new QWidget;
        widget->setLayout(hLayout);

        m_gridLayout->addWidget(widget, i / 2, i % 2);
    }

    int height = lstInfo.count() / 2;
    if (lstInfo.count() % 2 != 0) {
        qCDebug(appLog) << "lstInfo count is odd, increasing height by 1.";
        height += 1;
    }
    qCDebug(appLog) << "Calculated layout height:" << height;

    DFrame *frame = new DFrame;
    frame->setPalette(paletteBackground);
    if (height > 4) {
        qCDebug(appLog) << "Height is greater than 4, creating scrollable layout for large content.";
        setFixedSize(340, 192);

        m_gridLayout->setSpacing(8);
        m_gridLayout->setContentsMargins(0, 0, 0, 0);

        QWidget *widget1 = new QWidget;
        widget1->setLayout(m_gridLayout);
        widget1->setFixedSize(318, height * 50 - 20);

        DScrollArea *scrollArea = new DScrollArea;
        scrollArea->setFrameShadow(QFrame::Plain);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏横向滚动条
//        m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖向滚动条
        scrollArea->setWidget(widget1);

        // 设置滚动区域背景颜色为透明
        QPalette areaPalette;
        areaPalette.setColor(QPalette::Base, QColor(255, 255, 255, 0));
        scrollArea->setPalette(areaPalette);

        QVBoxLayout *scrollAreaLayout = new QVBoxLayout;
        scrollAreaLayout->addWidget(scrollArea);
        scrollAreaLayout->setSpacing(0);
        scrollAreaLayout->setContentsMargins(10, 10, 10, 10);

        frame->setLayout(scrollAreaLayout);
    } else {
        qCDebug(appLog) << "Height is 4 or less, creating fixed layout for small content.";
        int width = 340;
        if (lstInfo.count() == 1) {
            qCDebug(appLog) << "lstInfo count is 1, setting width to 170.";
            width = 170;
        }

        if (height == 1) {
            qCDebug(appLog) << "Height is 1, setting fixed size (width, 60).";
            setFixedSize(width, 60);
        } else {
            qCDebug(appLog) << "Height is greater than 1, setting fixed size (width, height * 50).";
            setFixedSize(width, height * 50);
        }

        m_gridLayout->setSpacing(8);
        m_gridLayout->setContentsMargins(10, 10, 10, 10);

        frame->setLayout(m_gridLayout);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(frame);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    qCDebug(appLog) << "setData completed.";
}


