// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddlog.h"
#include "ddbase.h"
#include "common.h"

#include <QKeyEvent>

DDBase::DDBase(QWidget *parent)
    : DDialog(parent)
{
    qCDebug(appLog) << "Creating base dialog";
    setModal(true);
    setIcon(QIcon::fromTheme(appName));

    m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameRounded(false);
    m_mainFrame->setFrameStyle(DFrame::NoFrame);

    addContent(m_mainFrame);
    qCDebug(appLog) << "Base dialog initialized with main frame";
    // updateGeometry();
}

void DDBase::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Escape) {
        qCDebug(appLog) << "Ignoring Escape key press";
        event->ignore();
    } else {
        qCDebug(appLog) << "Forwarding key press to base class";
        DDialog::keyPressEvent(event);
    }
}
