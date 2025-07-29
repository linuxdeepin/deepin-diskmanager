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
#if QT_VERSION_MAJOR <= 5
#include <QDesktopWidget>
#endif
#include <QRect>
#include <QDebug>
#include <QScreen>

MainWindow *MainWindow::instance()
{
    static MainWindow *m = nullptr;
    if (m == nullptr) {
        qDebug() << "Creating MainWindow singleton instance";
        m = new MainWindow;
    }
    return m;
}

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    qDebug() << "MainWindow constructor";
//    if (false == DeviceInfoParser::instance().getRootPassword()) {
//        exit(-1);
//    }

    m_handler = DMDbusHandler::instance(this);

    initUi();
    initConnection();

    // 窗口大小，默认适应1080P(1080，608)
    #define DEFAULT_SCALE 9 / 16
    const int niceWidth = 1080;
    const int minWidth = 850;
    const int minHeight = 608;

    //适应不同分辨率，保持9:16窗口比例, 默认适应1080P(1080，608)
    QSize normal(niceWidth, minHeight);
    // 获取主屏幕
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int scaleWidth = screenGeometry.width() * DEFAULT_SCALE;
    int scaleHeight = screenGeometry.height() * DEFAULT_SCALE;
    if (scaleWidth < normal.width() && scaleHeight < normal.height()) {
        qDebug() << "Screen dimensions are smaller than normal, adjusting window size.";
        normal.setWidth(scaleWidth);
        normal.setHeight(scaleHeight);
    }

    if (normal.width() < minWidth || normal.height() < minHeight) {
        qDebug() << "Adjusting window to minimum size.";
        setMinimumSize(minWidth, minHeight);
    } else {
        qDebug() << "Setting window to normal size.";
        setMinimumSize(normal.width(), normal.height());
    }

    resize(normal);

//    m_handler->getDeviceInfo(); //call after initUi
    QTimer::singleShot(200, this, SLOT(getDeviceInfo()));
    qDebug() << "MainWindow constructor finished.";
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destructor called.";
//    if (nullptr != m_bufferWin) {
//        m_bufferWin->deleteLater();
//    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow::closeEvent called.";
    m_central->HandleQuit();
//    m_handler->Quit();
    QProcess proc;
    QString args = QString("--system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.Quit");
    QStringList argList = args.split(" ");
    proc.startDetached("/usr/bin/dbus-send", argList);

    DMainWindow::closeEvent(event);
    qDebug() << "MainWindow::closeEvent completed.";
}

void MainWindow::initUi()
{
    qDebug() << "Initializing MainWindow UI";
    m_dialog = new AnimationDialog(this);
    m_dialog->setWindowModality(Qt::ApplicationModal);
    m_dialog->hide();

    m_central = new CenterWidget(this);
    setCentralWidget(m_central);

    m_btnRefresh = new DPushButton;
    QIcon icon = Common::getIcon("refresh");
    m_btnRefresh->setIcon(icon);
    setSizebyMode(m_btnRefresh);
    qDebug() << "setSizebyMode called for m_btnRefresh.";
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
    qDebug() << "MainWindow UI initialization complete.";
}

void MainWindow::setSizebyMode(DPushButton *button)
{
    qDebug() << "setSizebyMode called.";
#ifdef DTKWIDGET_CLASS_DSizeMode
    qDebug() << "DTKWIDGET_CLASS_DSizeMode is defined, setting size by DSizeModeHelper.";
    button->setFixedSize(QSize(DSizeModeHelper::element(24, 36), DSizeModeHelper::element(24, 36)));
    button->setIconSize(QSize(DSizeModeHelper::element(12, 18), DSizeModeHelper::element(12, 18)));
#else
    qDebug() << "DTKWIDGET_CLASS_DSizeMode is not defined, setting fixed size (36x36) and icon size (18x18).";
    button->setFixedSize(QSize(36, 36));
    button->setIconSize(QSize(18, 18));
#endif
    qDebug() << "setSizebyMode completed.";
}

void MainWindow::initConnection()
{
    qDebug() << "Setting up MainWindow signal connections";
    connect(m_handler, &DMDbusHandler::showSpinerWindow, this, &MainWindow::onShowSpinerWindow);
    connect(m_btnRefresh, &DPushButton::clicked, this, &MainWindow::onRefreshButtonClicked);
#ifdef DTKWIDGET_CLASS_DSizeMode
    qDebug() << "Connecting sizeModeChanged signal for DTKWIDGET_CLASS_DSizeMode.";
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::sizeModeChanged, this, [this]() {
        setSizebyMode(m_btnRefresh);
        qDebug() << "Size mode changed, setSizebyMode called for m_btnRefresh.";
    });
#endif
    qDebug() << "MainWindow signal connections setup complete.";
}

void MainWindow::onHandleQuitAction()
{
    //ToDo judge exit or no
    qDebug() << __FUNCTION__;

    QProcess proc;
    QString args = QString("--system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.Quit");
    QStringList argList = args.split(" ");
    proc.startDetached("/usr/bin/dbus-send", argList);

}

QString MainWindow::getRootLoginResult()
{
    return m_handler->getRootLoginResult();
}

void MainWindow::getDeviceInfo()
{
    qDebug() << "Requesting device information";
    m_handler->getDeviceInfo(); //call after initUi
}

void MainWindow::onRefreshButtonClicked()
{
    qDebug() << "Refresh button clicked";
    m_handler->refresh();
}

void MainWindow::onShowSpinerWindow(bool isShow, const QString &title)
{
    qDebug() << "Spinner window visibility changed:" << isShow << "title:" << title;
    if (isShow) {
        qDebug() << "Show spinner window";
        Dtk::Widget::moveToCenter(m_dialog);
        m_dialog->setShowSpinner(isShow, title);
        m_dialog->show();
    } else {
        qDebug() << "Hide spinner window";
        m_dialog->setShowSpinner(isShow, title);
        m_dialog->hide();

        raise();
        activateWindow();
    }
}
