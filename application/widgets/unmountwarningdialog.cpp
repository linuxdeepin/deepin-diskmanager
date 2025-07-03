// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "unmountwarningdialog.h"
#include <QDebug>

#include <QHBoxLayout>
#include <QVBoxLayout>

UnmountWarningDialog::UnmountWarningDialog(QWidget *parent) : DDBase(parent)
{
    qDebug() << "UnmountWarningDialog initialized";
    initUi();
    initConnection();
}

void UnmountWarningDialog::initUi()
{
    qDebug() << "Initializing UnmountWarningDialog UI.";
    DPalette palette1;
    QColor color("#000000");
    color.setAlphaF(0.9);
    palette1.setColor(DPalette::ToolTipText, color);

    DPalette palette2;
    palette2.setColor(DPalette::TextTips, QColor("#2C4767"));

    QFont font;
    font.setWeight(QFont::Medium);
    font.setFamily("Source Han Sans");
    font.setPixelSize(14);

    DLabel *titleLabel = new DLabel(tr("Unmounting system disk may result in system crash"), this);
    titleLabel->setFont(font);
    titleLabel->setPalette(palette1);
    titleLabel->setAlignment(Qt::AlignCenter);

    m_checkBox = new DCheckBox(this);
    DLabel *label = new DLabel(tr("I will take the risks that may arise"), this);
    label->setFont(font);
    label->setPalette(palette2);
    label->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch();
    layout->addWidget(m_checkBox);
    layout->addSpacing(10);
    layout->addWidget(label);
    layout->addStretch();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(layout);

    int index = addButton(tr("Cancel"), true, ButtonNormal);
    m_okCode = addButton(tr("Unmount"), false, ButtonWarning);
    getButton(m_okCode)->setDisabled(true);
    qDebug() << "Unmount button initially disabled";

    getButton(index)->setAccessibleName("cancelBtn");
    getButton(m_okCode)->setAccessibleName("unmountBtn");
    qDebug() << "UnmountWarningDialog UI initialization completed.";
}

void UnmountWarningDialog::initConnection()
{
    qDebug() << "Initializing UnmountWarningDialog connections.";
    connect(m_checkBox, &DCheckBox::stateChanged, this, &UnmountWarningDialog::onCheckBoxStateChange);
    qDebug() << "UnmountWarningDialog connections initialized.";
}

void UnmountWarningDialog::onCheckBoxStateChange(int state)
{
    qDebug() << "onCheckBoxStateChange called with state:" << state;
    switch (state) {
    case Qt::CheckState::Unchecked: {
        qDebug() << "CheckBox state is Unchecked, disabling unmount button.";
        getButton(m_okCode)->setDisabled(true);
        qDebug() << "Unmount button disabled";
        break;
    }
    case Qt::CheckState::Checked: {
        qDebug() << "CheckBox state is Checked, enabling unmount button.";
        getButton(m_okCode)->setDisabled(false);
        qDebug() << "Unmount button enabled";
        break;
    }
    default:
        qDebug() << "Unknown CheckBox state.";
        break;
    }
    qDebug() << "onCheckBoxStateChange completed.";
}



