// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "selectpvitemwidget.h"
#include "common.h"
#include "partedproxy/dmdbushandler.h"

#include <DFontSizeManager>

#include <QHBoxLayout>

SelectPVItemWidget::SelectPVItemWidget(PVInfoData pvInfoData, QWidget *parent)
    : RadiusWidget(parent)
    , m_pvInfoData(pvInfoData)
{
    qDebug() << "SelectPVItemWidget constructor called.";
    qDebug() << "Creating PV select item for:" << pvInfoData.m_diskPath << pvInfoData.m_partitionPath;
    initUi();
    initConnection();
    initData();
}

void SelectPVItemWidget::initUi()
{
    qDebug() << "SelectPVItemWidget::initUi called.";
    setFixedHeight(36);

    DPalette paletteName;
    paletteName.setColor(DPalette::TextTips, QColor("#2C4767"));

    DPalette paletteSize;
    paletteName.setColor(DPalette::TextTips, QColor("#526A7F"));

    QFont fontName;
    fontName.setWeight(QFont::Medium);
    fontName.setFamily("Source Han Sans");
    fontName.setPixelSize(14);

    QFont fontSize;
    fontSize.setWeight(QFont::Normal);
    fontSize.setFamily("Source Han Sans");
    fontSize.setPixelSize(12);

    m_checkBox = new DCheckBox(this);

    m_pathLabel = new DLabel(this);
    m_pathLabel->setText("/dev/sda");
    m_pathLabel->setFont(fontName);
    m_pathLabel->setPalette(paletteName);
    m_pathLabel->setAlignment(Qt::AlignVCenter);

    m_sizeLabel = new DLabel(this);
    m_sizeLabel->setText("1GiB");
    m_sizeLabel->setFont(fontSize);
    m_sizeLabel->setPalette(paletteSize);
    m_sizeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_checkBox, 0, Qt::AlignCenter);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(m_pathLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(m_sizeLabel);
    mainLayout->setSpacing(0);
    qDebug() << "Main layout widgets added and spacing set.";

    if ((m_pvInfoData.m_level == DMDbusHandler::DISK) && (m_pvInfoData.m_disktype != "unrecognized")) {
        qDebug() << "PV info data is DISK level and recognized, adding icon label.";
        m_iconLabel = new DLabel(this);
        QIcon icon = Common::getIcon("arrow_right");
        m_iconLabel->setPixmap(icon.pixmap(QSize(8, 11)));

        mainLayout->addSpacing(13);
        mainLayout->addWidget(m_iconLabel);
        mainLayout->setContentsMargins(10, 0, 12, 0);
    } else {
        qDebug() << "PV info data is not DISK level or unrecognized, setting content margins without icon.";
        mainLayout->setContentsMargins(10, 0, 20, 0);
    }

    setLayout(mainLayout);
    qDebug() << "Layout set.";
}

void SelectPVItemWidget::initConnection()
{
    qDebug() << "SelectPVItemWidget::initConnection called.";
    connect(m_checkBox, &DCheckBox::stateChanged, this, &SelectPVItemWidget::onCheckBoxStateChange);
}

void SelectPVItemWidget::initData()
{
    qDebug() << "SelectPVItemWidget::initData called.";
    switch (m_pvInfoData.m_selectStatus) {
    case Qt::CheckState::Unchecked: {
        qDebug() << "Setting check state to Unchecked.";
        setCheckBoxState(Qt::CheckState::Unchecked);
        break;
    }
    case Qt::CheckState::PartiallyChecked: {
        qDebug() << "Setting check state to PartiallyChecked.";
        setCheckBoxState(Qt::CheckState::PartiallyChecked);
        break;
    }
    case Qt::CheckState::Checked: {
        qDebug() << "Setting check state to Checked.";
        setCheckBoxState(Qt::CheckState::Checked);
        break;
    }
    default:
        qDebug() << "Unknown check state.";
        break;
    }

    if (m_pvInfoData.m_level == DMDbusHandler::DISK) {
        qDebug() << "PV info data level is DISK, setting disk path and size.";
        m_pathLabel->setText(m_pvInfoData.m_diskPath);
        m_sizeLabel->setText(m_pvInfoData.m_diskSize);
    } else {
        QString partitionPath = m_pvInfoData.m_partitionPath;
        if (partitionPath == "unallocated") {
            qDebug() << "Partition path is unallocated, setting path label to unallocated.";
            m_pathLabel->setText(partitionPath);
        } else {
            qDebug() << "Partition path is recognized, setting path label to modified partition path.";
            m_pathLabel->setText(partitionPath.remove(0, 5));
        }

        m_sizeLabel->setText(m_pvInfoData.m_partitionSize);
        qDebug() << "Setting partition size:" << m_pvInfoData.m_partitionSize;
    }

    if (m_pvInfoData.m_isReadOnly) {
        qDebug() << "PV is read-only, disabling checkbox.";
        m_checkBox->setDisabled(true);
    }
}

void SelectPVItemWidget::onCheckBoxStateChange(int state)
{
    qDebug() << "SelectPVItemWidget::onCheckBoxStateChange called with state:" << state;
    if (m_pvInfoData.m_level == DMDbusHandler::DISK) {
        qDebug() << "PV info data level is DISK.";
        if (state != Qt::CheckState::PartiallyChecked) {
            qDebug() << "State is not PartiallyChecked.";
            if (m_lstPVInfoData.count() != 0) {
                qDebug() << "List of PV info data is not empty, updating select status.";
                QList<PVInfoData> lstData;
                lstData.clear();
                for (int i = 0; i < m_lstPVInfoData.count(); i++) {
                    PVInfoData pvData = m_lstPVInfoData.at(i);
                    pvData.m_selectStatus = state;

                    lstData.append(pvData);
                    qDebug() << "Updated PV data for item:" << pvData.m_diskPath << ", new state:" << pvData.m_selectStatus;
                }

                m_lstPVInfoData.clear();
                m_lstPVInfoData = lstData;
                qDebug() << "m_lstPVInfoData updated.";
            }
        }

        m_pvInfoData.m_selectStatus = state;
        qDebug() << "PV info data select status updated to:" << m_pvInfoData.m_selectStatus;
        emit checkBoxStateChange(state);
    } else {
        qDebug() << "PV info data level is not DISK.";
        m_pvInfoData.m_selectStatus = state;
        qDebug() << "PV info data select status updated to:" << m_pvInfoData.m_selectStatus;
        emit checkBoxStateChange(state);
    }
}

void SelectPVItemWidget::setCheckBoxState(Qt::CheckState state, bool isPartiallyChecked)
{
    qDebug() << "Setting checkbox state:" << state << "isPartially:" << isPartiallyChecked;
    if(isPartiallyChecked){
        qDebug() << "isPartiallyChecked is true.";
        m_pvInfoData.m_selectStatus = state;
        m_checkBox->setCheckState(state);
    } else {
        qDebug() << "isPartiallyChecked is false, blocking signals.";
        blockSignals(true);
        m_pvInfoData.m_selectStatus = state;
        m_checkBox->setCheckState(state);
        blockSignals(false);
        qDebug() << "Signals unblocked.";
    }
}

void SelectPVItemWidget::setData(const QList<PVInfoData> &lstData)
{
    qDebug() << "SelectPVItemWidget::setData called, setting data with count:" << lstData.count();
    m_lstPVInfoData = lstData;
}

QList<PVInfoData> SelectPVItemWidget::getData()
{
    qDebug() << "SelectPVItemWidget::getData called.";
    return m_lstPVInfoData;
}

PVInfoData SelectPVItemWidget::getCurInfo()
{
    qDebug() << "SelectPVItemWidget::getCurInfo called.";
    return m_pvInfoData;
}

void SelectPVItemWidget::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "SelectPVItemWidget::mousePressEvent called.";
    if (m_pvInfoData.m_level == DMDbusHandler::DISK) {
        qDebug() << "PV info data level is DISK, emitting selectItem.";
        emit selectItem();
    }

    return QWidget::mousePressEvent(event);
}
