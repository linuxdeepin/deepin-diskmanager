#include "mainwindow.h"
#include "centerwidget.h"
#include "titlewidget.h"
#include <QTimer>
#include <DTitlebar>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : DMainWindow(parent)
{
    initUi();
    QRect rect = QApplication::desktop()->geometry();
    setMinimumSize(rect.width() * 0.6, rect.height() * 0.6);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_central->HandleQuit();
    DMainWindow::closeEvent(event);
}

void MainWindow::initUi()
{
    m_central = new CenterWidget(this);
    setCentralWidget(m_central);
    titlebar()->setIcon(QIcon::fromTheme("preferences-system"));
    titlebar()->setTitle("");
    // titlebar()->setMenu(m_central->titleMenu());
    titlebar()->addWidget(m_central->titlewidget(), Qt::AlignCenter);
}

void MainWindow::slotHandleQuitAction()
{
    //ToDo judge exit or no
    qDebug() << __FUNCTION__;
}

