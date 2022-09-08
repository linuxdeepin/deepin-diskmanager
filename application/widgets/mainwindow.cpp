// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "mainwindow.h"
#include "centerwidget.h"
#include "titlewidget.h"
#include "partedproxy/dmdbushandler.h"
#include "common.h"

#include <DTitlebar>
#include <DWidgetUtil>
#include <DWindowCloseButton>

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QDebug>
#include <QScreen>

MainWindow *MainWindow::instance()
{
    static MainWindow *m = nullptr;
    if (m == nullptr) {
        m = new MainWindow;
    }
    return m;
}

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
//    if (false == DeviceInfoParser::instance().getRootPassword()) {
//        exit(-1);
//    }

    m_handler = DMDbusHandler::instance(this);

    initUi();
    initConnection();

    QSize normal(1000, 650);

    QList<QScreen *> lst = QGuiApplication::screens();
    if (lst.size() > 0) {
        QSize rect = lst.at(0)->size();
        qDebug() << rect;
        if ( rect.width() * 3 / 5 - 150 < normal.width() && rect.height() * 3 / 5 < normal.height() ) {
            normal.setWidth(rect.width() * 3 / 5 - 150);
            normal.setHeight(rect.height() * 3 / 5);
        }
    }

    if (normal.width() < 850 && normal.height() < 600) {
        setMinimumSize(850, 600);
    } else {
        setMinimumSize(normal.width(), normal.height());
    }

    resize(normal);
//    QRect rect = QApplication::desktop()->screenGeometry(0);
//    setMinimumSize(rect.width() * 3 / 5 - 150, rect.height() * 3 / 5);
//    m_handler->getDeviceInfo(); //call after initUi
    QTimer::singleShot(200, this, SLOT(getDeviceInfo()));
}

MainWindow::~MainWindow()
{
//    if (nullptr != m_bufferWin) {
//        m_bufferWin->deleteLater();
//    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_central->HandleQuit();
//    m_handler->Quit();
    QProcess proc;
    proc.startDetached("/usr/bin/dbus-send --system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.Quit");

    DMainWindow::closeEvent(event);
}

void MainWindow::initUi()
{
    m_dialog = new AnimationDialog(this);
    m_dialog->setWindowModality(Qt::ApplicationModal);
    m_dialog->hide();

    m_central = new CenterWidget(this);
    setCentralWidget(m_central);

    m_btnRefresh = new DPushButton;
    QIcon icon = Common::getIcon("refresh");
    m_btnRefresh->setIcon(icon);
    m_btnRefresh->setFixedSize(QSize(36, 36));
    m_btnRefresh->setIconSize(QSize(19, 16));
    m_btnRefresh->setToolTip(tr("Refresh"));
    m_btnRefresh->setCheckable(false);
    m_btnRefresh->setObjectName("refresh");
    m_btnRefresh->setAccessibleName("refresh");

    QHBoxLayout *refreshLayout = new QHBoxLayout;
    refreshLayout->addSpacing(10);
    refreshLayout->addWidget(m_btnRefresh);
    refreshLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *widget = new QWidget;
    widget->setLayout(refreshLayout);

    titlebar()->setIcon(QIcon::fromTheme(appName));
    titlebar()->setTitle("");
    // titlebar()->setMenu(m_central->titleMenu());
    titlebar()->addWidget(widget, Qt::AlignLeft);
    titlebar()->addWidget(m_central->getTitleWidget(), Qt::AlignCenter);
    titlebar()->menu();
}

void MainWindow::initConnection()
{
    connect(m_handler, &DMDbusHandler::showSpinerWindow, this, &MainWindow::onShowSpinerWindow);
    connect(m_btnRefresh, &DPushButton::clicked, this, &MainWindow::onRefreshButtonClicked);
}

void MainWindow::onHandleQuitAction()
{
    //ToDo judge exit or no
    qDebug() << __FUNCTION__;

    QProcess proc;
    proc.startDetached("/usr/bin/dbus-send --system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.Quit");

}

QString MainWindow::getRootLoginResult()
{
    return m_handler->getRootLoginResult();
}

void MainWindow::getDeviceInfo()
{
    m_handler->getDeviceInfo(); //call after initUi
}

void MainWindow::onRefreshButtonClicked()
{
    m_handler->refresh();
}

void MainWindow::onShowSpinerWindow(bool isShow, const QString &title)
{
    if (isShow) {
        Dtk::Widget::moveToCenter(m_dialog);
        m_dialog->setShowSpinner(isShow, title);
        m_dialog->show();
    } else {
        m_dialog->setShowSpinner(isShow, title);
        m_dialog->hide();

        raise();
        activateWindow();
    }
}
