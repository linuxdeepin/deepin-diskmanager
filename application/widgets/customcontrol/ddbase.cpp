// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddbase.h"
#include "common.h"

#include <QKeyEvent>

DDBase::DDBase(QWidget *parent)
    : DDialog(parent)
{
    qDebug() << "Creating base dialog";
    setModal(true);
    setIcon(QIcon::fromTheme(appName));

    m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameRounded(false);
    m_mainFrame->setFrameStyle(DFrame::NoFrame);

    addContent(m_mainFrame);
    qDebug() << "Base dialog initialized with main frame";
    // updateGeometry();
}

void DDBase::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Escape) {
        qDebug() << "Ignoring Escape key press";
        event->ignore();
    } else {
        qDebug() << "Forwarding key press to base class";
        DDialog::keyPressEvent(event);
    }
}
