// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "ddlog.h"
#include "animationdialog.h"

#include <DWindowCloseButton>

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDebug>

AnimationDialog::AnimationDialog(QWidget *parent) : DDialog(parent)
{
    qCDebug(appLog) << "AnimationDialog initializing...";
    initUi();
    qCDebug(appLog) << "AnimationDialog initialized";
}

void AnimationDialog::initUi()
{
    qCDebug(appLog) << "AnimationDialog::initUi called.";
    setFixedSize(380, 160);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    qCDebug(appLog) << "AnimationDialog window flags set.";

    DFrame *m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameRounded(false);
    m_mainFrame->setFrameStyle(DFrame::NoFrame);
    addContent(m_mainFrame);
    qCDebug(appLog) << "Main frame initialized and added as content.";

    m_label = new DLabel(this);
    m_label->setAccessibleName("animationTitle");
    qCDebug(appLog) << "Label initialized.";
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    m_spinner = new DSpinner(m_mainFrame);
    m_spinner->setFixedSize(28, 28);
    m_spinner->start();
    m_spinner->hide();
    qCDebug(appLog) << "Spinner initialized and started, initially hidden.";

    mainLayout->addWidget(m_label, 0, Qt::AlignCenter | Qt::AlignTop);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_spinner, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    qCDebug(appLog) << "Layout configured.";
}

void AnimationDialog::setShowSpinner(bool isShow, const QString &title)
{
    qCDebug(appLog) << "Setting spinner visibility:" << isShow << "with title:" << title;

    if (isShow) {
        qCDebug(appLog) << "Showing spinner.";
        m_label->setText(title);
        m_spinner->show();
    } else {
        qCDebug(appLog) << "Hiding spinner.";
        m_spinner->hide();
    }
}

void AnimationDialog::keyPressEvent(QKeyEvent *event)
{
    // qCDebug(appLog) << "AnimationDialog::keyPressEvent called. Key:" << event->key();
    if (event->key() == Qt::Key::Key_Escape) {
        qCDebug(appLog) << "Escape key pressed, ignoring event";
        event->ignore();
    } else {
        // qCDebug(appLog) << "Other key pressed, calling base keyPressEvent.";
        DDialog::keyPressEvent(event);
    }
}






