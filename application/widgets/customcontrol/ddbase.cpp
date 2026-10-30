// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "ddbase.h"
#include "common.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QScreen>
#include <QShowEvent>

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

void DDBase::showEvent(QShowEvent *event)
{
    DDialog::showEvent(event);

    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    if (!screen && parentWidget())
        screen = QGuiApplication::screenAt(parentWidget()->window()->geometry().center());
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeo = screen->geometry();
        move(screenGeo.center() - rect().center());
    }
}
