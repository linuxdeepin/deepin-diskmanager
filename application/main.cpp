#include "environments.h"
#include "cusapplication.h"
#include "common.h"
#include "widgets/mainwindow.h"
#include "partedproxy/dmdbushandler.h"
#include <DMainWindow>
#include <DWidgetUtil>
#include <DApplicationSettings>
#include <DLog>
#include <DMainWindow>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QProcess>
#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

DWIDGET_USE_NAMESPACE

//void dealsig(int sig)
//{
//    qDebug() << "kdsfjjjjjj------";
//}
int main(int argc, char *argv[])
{
    // signal(SIGINT, SIG_IGN);
    // signal(SIGKILL, SIG_IGN);
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    if (XDG_SESSION_TYPE == QLatin1String("x11")) {
        CusApplication::loadDXcbPlugin();
    }

    CusApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.loadTranslator();
    a.setApplicationName(appName);
    a.setOrganizationName("deepin");
    a.setWindowIcon(QIcon::fromTheme(appName));
    a.setApplicationDisplayName(QObject::tr("Disk Utility"));
    a.setApplicationVersion(/*DApplication::buildVersion("20191227")*/VERSION);
    const QString acknowledgementLink = "https://www.deepin.org/acknowledgments/deepin_reader";
    a.setApplicationAcknowledgementPage(acknowledgementLink);

    QPixmap px(QIcon::fromTheme(appName).pixmap(static_cast<int>(256 * qApp->devicePixelRatio()), static_cast<int>(256 * qApp->devicePixelRatio())));
    px.setDevicePixelRatio(qApp->devicePixelRatio());
    a.setProductIcon(QIcon(px));
    a.setApplicationDescription(QObject::tr("Disk Utility is a disk management tool for creating, reorganizing and formatting partitions."));
    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w;
    if (a.setSingleInstance(appName)) {
        QObject::connect(&a, &DApplication::newInstanceStarted, &w, [&] {qDebug() << "======"; w.activateWindow(); });
    } else {
        exit(0);
    }
    QObject::connect(&a, &CusApplication::handleQuitActionChanged, &w, &MainWindow::onHandleQuitAction);

//    DMDbusHandler::instance()->startService(static_cast<qint64>(getpid()));
//    QObject::connect(DMDbusHandler::instance(), &DMDbusHandler::rootLogin, &w, [&] {
//        qDebug() << "Root Authentication Result:" << w.getRootLoginResult();
//        if (w.getRootLoginResult() == "1") {
//            w.show();
//            Dtk::Widget::moveToCenter(&w);
//        } else {
//            exit(0);
//        }
//    });

    w.show();
    Dtk::Widget::moveToCenter(&w);

    return a.exec();
}
