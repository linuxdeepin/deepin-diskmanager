// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "cylinderinfowidget.h"
#include "cylinderwidget.h"

#include <DPalette>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <QApplication>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QTime>

CylinderInfoWidget::CylinderInfoWidget(int cylNumber, QWidget *parent)
    : DFrame(parent)
    , m_cylNumber(cylNumber)
{
    qDebug()  << "[CylinderInfoWidget] Initializing with cylinder count:" << cylNumber;
    initUI();
    initConnections();
}

void CylinderInfoWidget::initUI()
{
    qDebug() << "[CylinderInfoWidget] Initializing UI components";
    m_gridLayout = new QGridLayout;

    int initCount = 360;
    if (initCount > m_cylNumber) {
        qDebug() << "[CylinderInfoWidget] Cylinder count is less than 360, set initCount to cylinder count:" << m_cylNumber;
        initCount = m_cylNumber;
    }

    m_initColor = "rgba(0,0,0,0.03)";
    m_excellentColor = "#6097FF";
    m_damagedColor = "#E23C3C";
    m_unknownColor = "#777990";
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
        m_initColor = "rgba(255,255,255,0.05)";
        m_excellentColor = "#2B6AE3";
        m_damagedColor = "#C41E1E";
        m_unknownColor = "#909090";
    } else if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        m_initColor = "rgba(0,0,0,0.03)";
        m_excellentColor = "#6097FF";
        m_damagedColor = "#E23C3C";
        m_unknownColor = "#777990";
    }

    for (int i = 0; i < initCount; i ++) {
        CylinderWidget *cylinderWidget = new CylinderWidget;
        cylinderWidget->setFixedSize(20, 20);
        cylinderWidget->setFrameShape(QFrame::Box);
        cylinderWidget->setObjectName(QString("%1").arg(i));
//        pLabel->setPalette(QPalette(QColor("#6097FF")));
//        QMap<QString, QVariant> map = lst.at(i);
        cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
//        pLabel->setUserData(map);
        connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
        connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);

        m_gridLayout->addWidget(cylinderWidget, i / 24, i % 24);
    }

    m_gridLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout *cylLayout = new QHBoxLayout;
    cylLayout->addLayout(m_gridLayout);
    cylLayout->addStretch();
    cylLayout->setContentsMargins(0, 0, 0, 0);

    m_widget = new QWidget;
    m_widget->setLayout(cylLayout);

    m_arrowRectangle = new DArrowRectangle(DArrowRectangle::ArrowTop, this);
    QColor color("#D2D2D2");
    color.setAlphaF(0.3);
    m_arrowRectangle->setBackgroundColor(color);
    m_arrowRectangle->setArrowWidth(54);
    m_arrowRectangle->setArrowHeight(18);
    m_arrowRectangle->setRadius(18);
    m_arrowRectangle->setWidth(207);
    m_arrowRectangle->setHeight(88);

    DPalette palette;
    palette.setColor(DPalette::WindowText, QColor("#000000"));

    m_LBALabel = new QLabel("LBA：772678305；");
    DFontSizeManager::instance()->bind(m_LBALabel, DFontSizeManager::T8, QFont::Normal);
    m_LBALabel->setPalette(palette);

    m_cylNumberLabel = new QLabel("柱面号：48097；");
    DFontSizeManager::instance()->bind(m_cylNumberLabel, DFontSizeManager::T8, QFont::Normal);
    m_cylNumberLabel->setPalette(palette);

    m_elapsedTimeLabel = new QLabel("柱面耗时：100ms；");
    DFontSizeManager::instance()->bind(m_elapsedTimeLabel, DFontSizeManager::T8, QFont::Normal);
    m_elapsedTimeLabel->setPalette(palette);

    m_curErrorInfoLabel = new QLabel("当前错误信息：I0 Device Timeout.");
    DFontSizeManager::instance()->bind(m_curErrorInfoLabel, DFontSizeManager::T8, QFont::Normal);
    m_curErrorInfoLabel->setPalette(palette);

    m_repairLabel =new QLabel("修复情况：已修复");
    DFontSizeManager::instance()->bind(m_repairLabel, DFontSizeManager::T8, QFont::Normal);
    m_repairLabel->setPalette(palette);

    QVBoxLayout *pLayout = new QVBoxLayout;
    pLayout->addWidget(m_LBALabel);
    pLayout->addWidget(m_cylNumberLabel);
    pLayout->addWidget(m_elapsedTimeLabel);
    pLayout->addWidget(m_curErrorInfoLabel);
    pLayout->addWidget(m_repairLabel);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(5, 5, 5, 5);

    QWidget *pInfoWidget = new QWidget;
    pInfoWidget->setLayout(pLayout);

    m_arrowRectangle->setContent(pInfoWidget);

    int rowCount = m_curCheckCount / 24;

    if ( m_curCheckCount % 24 != 0) {
        rowCount = m_curCheckCount / 24 + 1;
    }

    m_scrollBar = new DScrollBar(this);
    m_scrollBar->setRange(0, rowCount - 15);
    m_scrollBar->setPageStep(30);
    m_scrollBar->setSingleStep(5);
    m_scrollBar->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_widget);
    mainLayout->addStretch();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);

    m_settings = new QSettings("/tmp/CheckData.conf", QSettings::IniFormat, this);
    qDebug() << "[CylinderInfoWidget] CylinderInfoWidget initialized.";
}

void CylinderInfoWidget::initConnections()
{
    qDebug() << "[CylinderInfoWidget] Setting up signal-slot connections";
    connect(m_scrollBar, &QScrollBar::valueChanged, this, &CylinderInfoWidget::onScrollBarValueChanged);
}

void CylinderInfoWidget::againVerify(int cylNumber)
{
    qDebug() << "[CylinderInfoWidget] againVerify called with cylinder number:" << cylNumber;
    m_curCheckCount = 0;
    m_badSectorsCount = 0;
    m_scrollBar->hide();
    m_isChanged = false;
    m_startCylinder = m_settings->value("SettingData/BlockStart").toInt();
    m_endCylinder = m_settings->value("SettingData/BlockEnd").toInt();
    qDebug() << "[CylinderInfoWidget] Initialized for re-verification, start cylinder:" << m_startCylinder << ", end cylinder:" << m_endCylinder;

    QList<QObject *> lstCylinderWidget = m_widget->children();
    qDebug() << "[CylinderInfoWidget] Current cylinder widget count:" << lstCylinderWidget.count();

    if (cylNumber <= lstCylinderWidget.count() - 1) { // 当将要重新检测的个数小于等于360时
        qDebug() << "[CylinderInfoWidget] Number to verify is less than or equal to current widget count (<=360), resetting existing widgets.";
        for (int i = 1; i < lstCylinderWidget.count(); i ++) {
            CylinderWidget *cylinderWidget = static_cast<CylinderWidget *>(lstCylinderWidget.at(i));
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
            cylinderWidget->setUserData("");
            qDebug() << "[CylinderInfoWidget] Resetting widget at index:" << i;
        }
    } else { // 当将要重新检测的个数大于360
        qDebug() << "[CylinderInfoWidget] Number to verify is greater than 360, recreating up to 360 widgets.";
        for (int i = 0; i < 360; i ++) {
            if (i < lstCylinderWidget.count() - 1) {
                CylinderWidget *cylinderWidget = static_cast<CylinderWidget *>(lstCylinderWidget.at(i + 1));
                cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
                cylinderWidget->setUserData("");
                qDebug() << "[CylinderInfoWidget] Resetting existing widget at index:" << i;
            }else {
                CylinderWidget *cylinderWidget = new CylinderWidget;
                cylinderWidget->setFixedSize(20, 20);
                cylinderWidget->setFrameShape(QFrame::Box);
                cylinderWidget->setObjectName(QString("%1").arg(i));
                cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
                connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
                connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);
                // qDebug() << "[CylinderInfoWidget] Adding new widget at index:" << i;
                m_gridLayout->addWidget(cylinderWidget, i / 24, i % 24);
            }

        }
    }
    qDebug() << "[CylinderInfoWidget] againVerify completed.";
}

void CylinderInfoWidget::reset(int cylNumber)
{
    qDebug() << "[CylinderInfoWidget] reset called with cylinder number:" << cylNumber;
    m_curCheckCount = 0;
    m_badSectorsCount = 0;
    m_scrollBar->hide();
    m_isChanged = false;
    qDebug() << "[CylinderInfoWidget] Resetting internal state: curCheckCount, badSectorsCount, scrollBar hidden, isChanged to false.";

    QList<QObject *> lstCylinderWidget = m_widget->children();
    qDebug() << "[CylinderInfoWidget] Current cylinder widget count for reset:" << lstCylinderWidget.count();
    for (int i = 1; i < lstCylinderWidget.count(); i ++) {
        QObject *obj = lstCylinderWidget.at(i);
        qDebug() << "[CylinderInfoWidget] Deleting cylinder widget object at index:" << i;
        delete obj;
        obj = nullptr;
    }
    qDebug() << "[CylinderInfoWidget] All existing cylinder widgets deleted.";

    if (cylNumber <= 360) {
        qDebug() << "[CylinderInfoWidget] Cylinder number <= 360, recreating widgets up to:" << cylNumber;
        for (int i = 0; i < cylNumber; i++) {
            CylinderWidget *cylinderWidget = new CylinderWidget;
            cylinderWidget->setFixedSize(20, 20);
            cylinderWidget->setFrameShape(QFrame::Box);
            cylinderWidget->setObjectName(QString("%1").arg(i));
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
            connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
            connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);
            qDebug() << "[CylinderInfoWidget] Adding new cylinder widget for index:" << i;
            m_gridLayout->addWidget(cylinderWidget, i / 24, i % 24);
        }
    } else {
        qDebug() << "[CylinderInfoWidget] Cylinder number > 360, recreating 360 widgets.";
        for (int i = 0; i < 360; i++) {
            CylinderWidget *cylinderWidget = new CylinderWidget;
            cylinderWidget->setFixedSize(20, 20);
            cylinderWidget->setFrameShape(QFrame::Box);
            cylinderWidget->setObjectName(QString("%1").arg(i));
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
            connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
            connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);
            qDebug() << "[CylinderInfoWidget] Adding new cylinder widget for index:" << i;
            m_gridLayout->addWidget(cylinderWidget, i / 24, i % 24);
        }
    }
    qDebug() << "[CylinderInfoWidget] reset completed.";
}

void CylinderInfoWidget::setChecked(bool isCheck)
{
    qDebug() << "[CylinderInfoWidget] setChecked called with:" << isCheck;
    m_isCheck = isCheck;
}

void CylinderInfoWidget::setCylinderNumber(int cylNumber)
{
    qDebug() << "[CylinderInfoWidget] setCylinderNumber called with:" << cylNumber;
    m_cylNumber = cylNumber;
    m_curCheckCount = 0;
    m_badSectorsCount = 0;
    m_scrollBar->hide();
    m_isChanged = false;
    m_startCylinder = m_settings->value("SettingData/BlockStart").toInt();
    m_endCylinder = m_settings->value("SettingData/BlockEnd").toInt();
    qDebug() << "[CylinderInfoWidget] Initialized m_startCylinder:" << m_startCylinder << "m_endCylinder:" << m_endCylinder;

    QList<QObject *> lstCylinderWidget = m_widget->children();
    qDebug() << "[CylinderInfoWidget] Current cylinder widget count:" << lstCylinderWidget.count();

    if (m_cylNumber == lstCylinderWidget.count() - 1) { // 当前要检测的柱面数等于初始化个数
        qDebug() << "[CylinderInfoWidget] Current cylinder number equals initialized count, returning.";
        return;
    }

    if (m_cylNumber > lstCylinderWidget.count() - 1 && (lstCylinderWidget.count() - 1 == 360)) { // 初始化个数为360并且当前要检测的柱面数大于初始化个数
        qDebug() << "[CylinderInfoWidget] Initialized count is 360 and current cylinder number is greater, returning.";
        return;
    }

    if (m_cylNumber < lstCylinderWidget.count() - 1) { // 当前要检测的柱面数小于初始化个数
        qDebug() << "[CylinderInfoWidget] Current cylinder number is less than initialized count, deleting excess widgets.";
        for (int i = 1; i < lstCylinderWidget.count(); i ++) {
            if(i > m_cylNumber) {
                QObject *obj = lstCylinderWidget.at(i);
                qDebug() << "[CylinderInfoWidget] Deleting cylinder widget at index:" << i;
                delete obj;
                obj = nullptr;
            }
        }
    } else if((m_cylNumber > lstCylinderWidget.count() - 1) && (m_cylNumber < 360)){ // 当前要检测的柱面数大于初始化个数且当前要检测的柱面数不到360个时
        qDebug() << "[CylinderInfoWidget] Current cylinder number is greater than initialized count but less than 360, recreating widgets.";
        for (int i = 1; i < lstCylinderWidget.count(); i ++) {
            QObject *obj = lstCylinderWidget.at(i);
            delete obj;
            obj = nullptr;
        }

        for (int i = 0; i < m_cylNumber; i ++) {
            CylinderWidget *cylinderWidget = new CylinderWidget;
            cylinderWidget->setFixedSize(20, 20);
            cylinderWidget->setFrameShape(QFrame::Box);
            cylinderWidget->setObjectName(QString("%1").arg(i));
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
            connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
            connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);

            m_gridLayout->addWidget(cylinderWidget, i / 24, i % 24);
        }
    } else { // 当前要检测的柱面数大于初始化个数且当前要检测的柱面数大于360个时，只显示360个柱面块
        qDebug() << "[CylinderInfoWidget] Current cylinder number is greater than 360, displaying only 360 widgets.";
        for (int i = 1; i < lstCylinderWidget.count(); i ++) {
            QObject *obj = lstCylinderWidget.at(i);
            qDebug() << "[CylinderInfoWidget] Deleting existing widget at index:" << i;
            delete obj;
            obj = nullptr;
        }

        for (int i = 0; i < 360; i ++) {
            CylinderWidget *cylinderWidget = new CylinderWidget;
            cylinderWidget->setFixedSize(20, 20);
            cylinderWidget->setFrameShape(QFrame::Box);
            cylinderWidget->setObjectName(QString("%1").arg(i));
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
            connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
            connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);

            m_gridLayout->addWidget(cylinderWidget, i / 24, i % 24);
        }
    }
}

void CylinderInfoWidget::setCurCheckBadBlocksInfo(const QString &LBANumber, const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo)
{
    qDebug()  << "[CylinderInfoWidget] Updating cylinder info - LBA:" << LBANumber
            << "Cylinder:" << cylinderNumber << "Status:" << cylinderStatus << "Time:" << cylinderTimeConsuming << "ms";
    ++m_curCheckCount;

    if (cylinderStatus == "bad") {
        qWarning() << "[CylinderInfoWidget] Bad sector detected - LBA:" << LBANumber
                  << "Cylinder:" << cylinderNumber;
        ++m_badSectorsCount;
        qDebug() << "[CylinderInfoWidget] Bad sectors count incremented to:" << m_badSectorsCount;
    }

    if (m_checkData.count() >= 360) {
        qDebug() << "[CylinderInfoWidget] Check data count >= 360, removing first element.";
        m_checkData.removeFirst();
        m_cylNumberData.removeFirst();
    }
    m_cylNumberData << cylinderNumber;
    m_checkData << QString("%1,%2,%3,%4,%5,0").arg(LBANumber).arg(cylinderNumber).arg(cylinderTimeConsuming).arg(cylinderStatus).arg(cylinderErrorInfo);
    qDebug() << "[CylinderInfoWidget] Added current check data:" << cylinderNumber;

    if (m_isChanged) {
        qDebug() << "[CylinderInfoWidget] m_isChanged is true, skipping update.";
        return;
    }

//    int startCylinder = m_settings->value("SettingData/BlockStart").toInt();
    if (m_cylNumber <= 360) {
        qDebug() << "[CylinderInfoWidget] Cylinder number <= 360, updating cylinder info directly.";
        updateCylinderInfo((cylinderNumber.toInt() - m_startCylinder) % 360, LBANumber, cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo, "0");
    } else {
        qDebug() << "[CylinderInfoWidget] Cylinder number > 360, checking curCheckCount.";
        if (m_curCheckCount <= 360) {
            qDebug() << "[CylinderInfoWidget] curCheckCount <= 360, updating cylinder info directly.";
            updateCylinderInfo((cylinderNumber.toInt() - m_startCylinder) % 360, LBANumber, cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo, "0");
        }else {
            qDebug() << "[CylinderInfoWidget] curCheckCount > 360, handling scroll bar and widget updates.";
            int rowCount = m_curCheckCount / 24;

            if ( m_curCheckCount % 24 != 0) {
                rowCount = m_curCheckCount / 24 + 1;
                qDebug() << "[CylinderInfoWidget] rowCount adjusted for remainder:" << rowCount;
            }
            m_scrollBar->setRange(0, rowCount - 15);
            qDebug() << "[CylinderInfoWidget] Scroll bar range set to (0, " << rowCount - 15 << ").";

            if (!m_isChanged) {
                qDebug() << "[CylinderInfoWidget] m_isChanged is false, proceeding with scroll bar and widget updates.";
                if (rowCount > 15) {
                    m_scrollBar->setValue(rowCount - 15);
                    m_scrollBar->setGeometry(600, 0, 18, 390);
                    m_scrollBar->show();
                    qDebug() << "[CylinderInfoWidget] Scroll bar shown and value set:" << rowCount - 15;
                }

                QList<QObject *> lstCylinderWidget = m_widget->children();
                int start = (rowCount - 15) * 24 + m_startCylinder;
                qDebug() << "[CylinderInfoWidget] Recalculated start index for widgets:" << start;
                for (int i = start; i < start + 360; i ++) {
                    if (-1 != m_cylNumberData.indexOf(QString("%1").arg(i))) {
                        QString value = m_checkData.at(m_cylNumberData.indexOf(QString("%1").arg(i)));
                        QStringList lst = value.split(",");
                        updateCylinderInfo((i - start) % 360, lst.at(0), lst.at(1), lst.at(2), lst.at(3), lst.at(4), lst.at(5));
                        qDebug() << "[CylinderInfoWidget] Updating existing widget with data for cylinder:" << i;
                    } else {
                        if (i > m_endCylinder) {
                            CylinderWidget *cylinderWidget = m_widget->findChild<CylinderWidget *>(QString("%1").arg((i - start) % 360));
                            delete cylinderWidget;
                            cylinderWidget = nullptr;
                            qDebug() << "[CylinderInfoWidget] Deleting cylinder widget for out of range cylinder:" << i;
                        } else {
                            updateCylinderInfo((i - start) % 360, "", "", "", "", "", "");
                            qDebug() << "[CylinderInfoWidget] Resetting cylinder widget for cylinder:" << i;
                        }
                    }
                }
            }
        }
    }
    qDebug() << "[CylinderInfoWidget] setCurCheckBadBlocksInfo completed.";
//    if (m_cylNumber == m_curCheckCount) {
//        emit checkCoomplete(m_badSectorsCount);
//    }
}

void CylinderInfoWidget::updateCylinderInfo(int number, const QString &LBANumber, const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo, const QString &repair)
{
    qDebug() << "[CylinderInfoWidget] Updating cylinder widget - Number:" << number
             << "LBA:" << LBANumber << "Status:" << cylinderStatus << "Repair:" << repair;
    CylinderWidget *cylinderWidget = m_widget->findChild<CylinderWidget *>(QString("%1").arg(number));

    if(cylinderWidget == nullptr) {
        qWarning() << "[CylinderInfoWidget] Cylinder widget not found for number:" << number << ", returning.";
        return;
    }

    if (LBANumber.isEmpty()) {
        qDebug() << "[CylinderInfoWidget] LBA number is empty, resetting widget style.";
        cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
        cylinderWidget->setUserData("");

    } else {
        qDebug() << "[CylinderInfoWidget] LBA number is not empty, updating widget based on status.";
        if (cylinderStatus == "good") {
            qDebug() << "[CylinderInfoWidget] Cylinder status is good, setting excellent color.";
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_excellentColor));
        } else if (cylinderStatus == "bad") {
            qDebug() << "[CylinderInfoWidget] Cylinder status is bad, setting damaged color.";
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_damagedColor));
        } else {
            qDebug() << "[CylinderInfoWidget] Cylinder status is unknown, setting unknown color.";
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_unknownColor));
        }

        QMap<QString, QVariant> mapInfo;
        mapInfo["id"] = LBANumber;
        mapInfo["number"] = cylinderNumber;
        mapInfo["time"] = cylinderTimeConsuming;
        mapInfo["errorinfo"] = cylinderErrorInfo;
        mapInfo["status"] = cylinderStatus;
        mapInfo["repair"] = repair;

        cylinderWidget->setUserData(mapInfo);
        qDebug() << "[CylinderInfoWidget] Widget user data updated.";
    }
    qDebug() << "[CylinderInfoWidget] updateCylinderInfo completed.";
}

void CylinderInfoWidget::setCurRepairBadBlocksInfo(const QString &cylinderNumber)
{
    qDebug() << "[CylinderInfoWidget] setCurRepairBadBlocksInfo called for cylinder:" << cylinderNumber;
    QList<QObject *> lstCylinderWidget = m_widget->children();
    qDebug() << "[CylinderInfoWidget] Current cylinder widget count:" << lstCylinderWidget.count();

    if (lstCylinderWidget.count() < 2) {
        qDebug() << "[CylinderInfoWidget] Less than 2 cylinder widgets, returning.";
        return;
    }

    CylinderWidget *firstWidget = static_cast<CylinderWidget *>(lstCylinderWidget.at(1));
    QMap<QString, QVariant> firstValue = firstWidget->getUserData().toMap();
    qDebug() << "[CylinderInfoWidget] First widget number:" << firstValue["number"].toInt();

    CylinderWidget *lastWidget = static_cast<CylinderWidget *>(lstCylinderWidget.at(lstCylinderWidget.count() - 1));
    QMap<QString, QVariant> lastValue = lastWidget->getUserData().toMap();
    qDebug() << "[CylinderInfoWidget] Last widget number:" << lastValue["number"].toInt();
    if (firstValue["number"].toInt() > cylinderNumber.toInt() || lastValue["number"].toInt() < cylinderNumber.toInt()) {
        qDebug() << "[CylinderInfoWidget] Cylinder number out of current widget range, returning.";
        return;
    }

    for (int i = 1; i < lstCylinderWidget.count(); i++) {
        CylinderWidget *cylinderWidget = static_cast<CylinderWidget *>(lstCylinderWidget.at(i));
        QMap<QString, QVariant> mapInfo = cylinderWidget->getUserData().toMap();
        qDebug() << "[CylinderInfoWidget] Checking widget at index:" << i << ", number:" << mapInfo["number"].toInt();

        if (mapInfo["number"].toInt() == cylinderNumber.toInt()) {
            qDebug() << "[CylinderInfoWidget] Matching cylinder found, updating style and user data.";
            cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_excellentColor));
            QString curCheckData = m_settings->value(QString("CheckData/%1").arg(cylinderNumber)).toString();
            QStringList lstCheckData = curCheckData.split(",");
            mapInfo["status"] = lstCheckData.at(3);
            mapInfo["repair"] = lstCheckData.at(5);
            cylinderWidget->setUserData(mapInfo);
            qDebug() << "[CylinderInfoWidget] Widget updated, status:" << mapInfo["status"].toString() << ", repair:" << mapInfo["repair"].toString();
            break;
        }
    }
    qDebug() << "[CylinderInfoWidget] setCurRepairBadBlocksInfo completed.";
}

void CylinderInfoWidget::enterSlot()
{
    qDebug() << "[CylinderInfoWidget] Mouse entered cylinder widget";
    CylinderWidget *label = qobject_cast<CylinderWidget *>(sender());

    QMap<QString, QVariant> mapInfo = label->getUserData().toMap();

    if (mapInfo.isEmpty()) {
        qDebug() << "[CylinderInfoWidget] mapInfo is empty, returning from enterSlot.";
        return;
    }

    qDebug() << "[CylinderInfoWidget] Setting LBA label text.";
    m_LBALabel->setText(QString(tr("LBA: %1")).arg(mapInfo["id"].toString())); // LBA
    m_cylNumberLabel->setText(QString(tr("Cyl.: %1")).arg(mapInfo["number"].toString())); // 柱面号
    m_curErrorInfoLabel->setText(QString(tr("Error: %1")).arg(mapInfo["errorinfo"].toString())); // 当前错误信息
    m_elapsedTimeLabel->setText(QString(tr("Cyl. elapsed time: %1")).arg(mapInfo["time"].toString()) + tr("ms")); // 柱面耗时  毫秒
    m_repairLabel->setText(tr("Status: Repaired")); // 修复情况：已修复

    if (mapInfo["repair"].toInt() == 1) {
        qDebug() << "[CylinderInfoWidget] Repair status is 1, showing error and repair, hiding elapsed time.";
        m_curErrorInfoLabel->show();
        m_repairLabel->show();
        m_elapsedTimeLabel->hide();
    } else {
        if (mapInfo["status"].toString() == "good") {
            qDebug() << "[CylinderInfoWidget] Status is good, hiding error and repair, showing elapsed time.";
            m_curErrorInfoLabel->hide();
            m_repairLabel->hide();
            m_elapsedTimeLabel->show();
        } else {
            qDebug() << "[CylinderInfoWidget] Status is not good, showing error and elapsed time, hiding repair.";
            m_curErrorInfoLabel->show();
            m_elapsedTimeLabel->show();
            m_repairLabel->hide();
        }
    }

    qDebug() << "[CylinderInfoWidget] Showing arrow rectangle.";
    m_arrowRectangle->show(label->mapToGlobal(label->pos()).x() - label->pos().x() + (label->width() / 2),
                           label->mapToGlobal(label->pos()).y() - label->pos().y() + label->height() - 1);
}

void CylinderInfoWidget::leaveSlot()
{
    qDebug() << "[CylinderInfoWidget] Mouse left cylinder widget";
    CylinderWidget *label = qobject_cast<CylinderWidget *>(sender());
    QMap<QString, QVariant> mapInfo = label->getUserData().toMap();
    if (mapInfo.isEmpty()) {
        qDebug() << "[CylinderInfoWidget] mapInfo is empty, returning from leaveSlot.";
        return;
    }

    m_arrowRectangle->hide();
}

void CylinderInfoWidget::onScrollBarValueChanged(int value)
{
    qDebug() << "[CylinderInfoWidget] Scroll bar value changed to:" << value;
    if (value < m_scrollBar->maximum()) {
        qDebug() << "[CylinderInfoWidget] Scroll bar value is less than maximum, proceeding with update.";
        m_isChanged = true;
    } else {
        qDebug() << "[CylinderInfoWidget] Scroll bar value is greater than or equal to maximum, not proceeding with update.";
        m_isChanged = false;
    }

    if (m_isCheck && !m_isChanged) {
        qDebug() << "[CylinderInfoWidget] Check is in progress and scroll bar value has not changed, returning.";
        return;
    }

    int startCylinder = m_settings->value("SettingData/BlockStart").toInt();

    QList<QObject *> lstCylinderWidget = m_widget->children();
    int start = value * 24 + startCylinder;
    for (int i = start; i < start + 360; i ++) {
        QString value = m_settings->value(QString("CheckData/%1").arg(i)).toString();
        if (!value.isEmpty()) {
            if (((i - start) % 360 + 1) > (lstCylinderWidget.count() - 1)) {
                CylinderWidget *cylinderWidget = new CylinderWidget;
                cylinderWidget->setFixedSize(20, 20);
                cylinderWidget->setFrameShape(QFrame::Box);
                cylinderWidget->setObjectName(QString("%1").arg((i - start) % 360));
                cylinderWidget->setStyleSheet(QString("background:%1;border:0px").arg(m_initColor));
                connect(cylinderWidget, &CylinderWidget::enter, this, &CylinderInfoWidget::enterSlot);
                connect(cylinderWidget, &CylinderWidget::leave, this, &CylinderInfoWidget::leaveSlot);

                m_gridLayout->addWidget(cylinderWidget, ((i - start) % 360) / 24, ((i - start) % 360) % 24);
            }

            QStringList lst = value.split(",");
            updateCylinderInfo((i - start) % 360, lst.at(0), lst.at(1), lst.at(2), lst.at(3), lst.at(4), lst.at(5));
        } else {
            if (i > m_settings->value("SettingData/BlockEnd").toInt()) {
                CylinderWidget *cylinderWidget = m_widget->findChild<CylinderWidget *>(QString("%1").arg((i - start) % 360));
                delete cylinderWidget;
                cylinderWidget = nullptr;
            } else {
                updateCylinderInfo((i - start) % 360, "", "", "", "", "", "");
            }
        }
    }
    qDebug() << "[CylinderInfoWidget] Scroll bar value update finished.";
}

void CylinderInfoWidget::wheelEvent(QWheelEvent *event)
{
    if (!m_scrollBar->isHidden()) {
#if QT_VERSION_MAJOR > 5
        if (event->angleDelta().y() > 0) {
#else
        if(event->delta() > 0){
#endif
            int value = m_scrollBar->value() - 15;
            m_scrollBar->setValue(value);
            m_isChanged = true;
        }else{
            int value = m_scrollBar->value() + 15;
            m_scrollBar->setValue(value);

            if (value >= m_scrollBar->maximum()) {
                m_isChanged = false;
            }
        }
    }
}


