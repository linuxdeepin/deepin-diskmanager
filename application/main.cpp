#include "cusapplication.h"
#include "widgets/widgetdeclare.h"
#include "widgets/mainwindow.h"
#include <DMainWindow>
#include <DWidgetUtil>
#include <DApplicationSettings>
#include <DLog>
#include <DMainWindow>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
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
    CusApplication::loadDXcbPlugin();
    CusApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.loadTranslator();
    a.setApplicationName(app_name);
    a.setOrganizationName("deepin");
    a.setWindowIcon(QIcon::fromTheme(app_name));
    a.setApplicationDisplayName(QObject::tr("Disk Utility"));
    a.setApplicationVersion(DApplication::buildVersion("20191227"));
    const QString acknowledgementLink = "https://www.deepin.org/acknowledgments/deepin_reader";
    a.setApplicationAcknowledgementPage(acknowledgementLink);

    QPixmap px(QIcon::fromTheme(app_name).pixmap(256 * qApp->devicePixelRatio(), 256 * qApp->devicePixelRatio()));
    px.setDevicePixelRatio(qApp->devicePixelRatio());
    a.setProductIcon(QIcon(px));
    a.setApplicationDescription(QObject::tr("Disk Utility is a disk management tool for creating, reorganizing and formatting partitions."));

    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    MainWindow w;
    if (a.setSingleInstance(app_name)) {
        QObject::connect(&a, &DApplication::newInstanceStarted, &w, [&] {qDebug() << "======"; w.activateWindow(); });
    } else {
        exit(0);
    }
    QObject::connect(&a, &CusApplication::sighandleQuitAction, &w, &MainWindow::slotHandleQuitAction);
    w.show();

    Dtk::Widget::moveToCenter(&w);

    return a.exec();
}
