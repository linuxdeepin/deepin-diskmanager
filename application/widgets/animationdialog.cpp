// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "animationdialog.h"

#include <DWindowCloseButton>

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDebug>

AnimationDialog::AnimationDialog(QWidget *parent) : DDialog(parent)
{
    qDebug() << "AnimationDialog initializing...";
    initUi();
    qDebug() << "AnimationDialog initialized";
}

void AnimationDialog::initUi()
{
    qDebug() << "AnimationDialog::initUi called.";
    setFixedSize(380, 160);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    qDebug() << "AnimationDialog window flags set.";

    DFrame *m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameRounded(false);
    m_mainFrame->setFrameStyle(DFrame::NoFrame);
    addContent(m_mainFrame);
    qDebug() << "Main frame initialized and added as content.";

    m_label = new DLabel(this);
    m_label->setAccessibleName("animationTitle");
    qDebug() << "Label initialized.";
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    m_spinner = new DSpinner(m_mainFrame);
    m_spinner->setFixedSize(28, 28);
    m_spinner->start();
    m_spinner->hide();
    qDebug() << "Spinner initialized and started, initially hidden.";

    mainLayout->addWidget(m_label, 0, Qt::AlignCenter | Qt::AlignTop);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_spinner, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    qDebug() << "Layout configured.";
}

void AnimationDialog::setShowSpinner(bool isShow, const QString &title)
{
    qDebug() << "Setting spinner visibility:" << isShow << "with title:" << title;

    if (isShow) {
        qDebug() << "Showing spinner.";
        m_label->setText(title);
        m_spinner->show();
    } else {
        qDebug() << "Hiding spinner.";
        m_spinner->hide();
    }
}

void AnimationDialog::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << "AnimationDialog::keyPressEvent called. Key:" << event->key();
    if (event->key() == Qt::Key::Key_Escape) {
        qDebug() << "Escape key pressed, ignoring event";
        event->ignore();
    } else {
        // qDebug() << "Other key pressed, calling base keyPressEvent.";
        DDialog::keyPressEvent(event);
    }
}






