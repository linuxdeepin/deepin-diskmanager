/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mainwindow.cpp
 *
 * @brief 主窗口类
 *
 * @date 2020-09-04 10:23
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "mainwindow.h"
#include "centerwidget.h"
#include "titlewidget.h"
#include "partedproxy/dmdbushandler.h"
#include "common.h"

#include <DTitlebar>
#include <DWidgetUtil>

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
//    m_bufferWin = new BufferWin;
    m_spinner = new DSpinner(this);
    m_spinner->setWindowFlags(Qt::SplashScreen);
    m_spinner->setWindowModality(Qt::ApplicationModal);
    m_spinner->setAttribute(Qt::WA_TranslucentBackground, true);
    m_spinner->setGeometry(100, 100, 100, 100);

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

void MainWindow::onShowSpinerWindow(bool isShow)
{
    if (isShow) {
        // m_bufferWin->Start();
        m_spinner->start();

        Dtk::Widget::moveToCenter(m_spinner);
        m_spinner->show();
    } else {
        // m_bufferWin->Stop();
        m_spinner->stop();
        m_spinner->hide();

        raise();
        activateWindow();
    }
}
