#include "mainwindow.h"
#include "centerwidget.h"
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) : DMainWindow(parent)
{
    m_central = new CenterWidget(this);
    setCentralWidget(m_central);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_central->HandleQuit();
    DMainWindow::closeEvent(event);
}

