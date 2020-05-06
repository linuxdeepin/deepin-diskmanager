#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DApplicationSettings>
#include <DLog>
#include <DMainWindow>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include "widgets/mainwindow.h"
#include <signal.h>
DWIDGET_USE_NAMESPACE
//void dealsig(int sig)
//{
//    qDebug() << "kdsfjjjjjj------";
//}
int main(int argc, char *argv[])
{
    // signal(SIGINT, SIG_IGN);
    // signal(SIGKILL, SIG_IGN);
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.loadTranslator();
    a.setApplicationName("deepin-diskmanager");
    a.setOrganizationName("deepin");
    a.setWindowIcon(QIcon::fromTheme("deepin-reader"));
    a.setApplicationDisplayName(QObject::tr("ISO disk administrator"));
    a.setApplicationVersion(DApplication::buildVersion("20191227"));
    const QString acknowledgementLink = "https://www.deepin.org/acknowledgments/deepin_reader";
    a.setApplicationAcknowledgementPage(acknowledgementLink);

    QPixmap px(QIcon::fromTheme("deepin-diskmanager").pixmap(256 * qApp->devicePixelRatio(), 256 * qApp->devicePixelRatio()));
    px.setDevicePixelRatio(qApp->devicePixelRatio());
    a.setProductIcon(QIcon(px));
    a.setApplicationDescription(QObject::tr("ISO disk manager is a disk management tool provided by tongxin to the installation manufacture"));

    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w;
    if (a.setSingleInstance("deepin-diskmanager")) {
        QObject::connect(&a, &DApplication::newInstanceStarted, &w, [&] {qDebug() << "======"; w.activateWindow();});
    } else {
        exit(0);
    }

    w.setMinimumSize(500, 500);
    w.show();

    Dtk::Widget::moveToCenter(&w);

    return a.exec();
}
