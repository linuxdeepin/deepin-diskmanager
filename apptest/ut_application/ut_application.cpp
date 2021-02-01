#include <iostream>
#include "gtest/gtest.h"
#include <qtest.h>
#include <DTitlebar>
#include <DSuggestButton>
#include <DWarningButton>
#include <QTimer>
#include <QObject>

#include "../application/partedproxy/dmdbushandler.h"
#include "mainwindow.h"
#include "centerwidget.h"
#include "mainsplitter.h"
#include "devicelistwidget.h"
#include "partitionwidget.h"
#include "mountdialog.h"
#include "dmtreeview.h"
#include "titlewidget.h"
#include "unmountdialog.h"
#include "formatedialog.h"
#include "resizedialog.h"
#include "stub.h"
#include "stubAll.h"
#include "messagebox.h"

class ut_application : public ::testing::Test
        , public QObject
{
protected:
    ut_application()
    {
    }

    virtual ~ut_application()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_application, init)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, getDeviceInfo), getDeviceInfo);

    Stub stub3;
    stub3.set(ADDR(DMDbusHandler, onSetCurSelect), setCurSelect);

    Stub stub2;
    stub2.set(ADDR(DMDbusHandler, startService), start);

    qDebug() << MainWindow::instance()->isHidden();
    qDebug() << MainWindow::instance()->children();
    qDebug() << DMDbusHandler::instance()->getDeviceNameList();
    qDebug() << DMDbusHandler::instance()->getCurDeviceInfo().m_path
             << DMDbusHandler::instance()->getCurDeviceInfo().partition.count()
             << DMDbusHandler::instance()->getCurPartititonInfo().m_path;

    ASSERT_FALSE(DMDbusHandler::instance()->getCurDeviceInfo().m_path.isEmpty());
    ASSERT_FALSE(DMDbusHandler::instance()->getCurPartititonInfo().m_path.isEmpty());
}

TEST_F(ut_application, hide)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, hidePartition), hide);

    typedef int (*fptr)(DeviceListWidget*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub2;
    stub2.set(foo, MessageboxExec);

    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();

    deviceListWidget->m_curDiskInfoData = deviceListWidget->m_treeView->getCurItem()->data().value<DiskInfoData>();
    deviceListWidget->onHidePartitionClicked();

    DMDbusHandler::instance()->onHidePartition("1");
    ASSERT_TRUE(deviceListWidget->isHideSuccess);

    DMDbusHandler::instance()->onHidePartition("0");
    ASSERT_FALSE(deviceListWidget->isHideSuccess);
}

TEST_F(ut_application, hide_mount)
{
    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();

    DiskInfoData oldData = deviceListWidget->m_curDiskInfoData;
    DiskInfoData diskInfoData = deviceListWidget->m_curDiskInfoData;
    qDebug() << diskInfoData.m_mountpoints;
    diskInfoData.m_mountpoints = "/";
    deviceListWidget->m_curDiskInfoData = diskInfoData;

    typedef int (*fptr)(DeviceListWidget*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub5;
    stub5.set(foo, MessageboxExec);

    deviceListWidget->onHidePartitionClicked();
    ASSERT_FALSE(deviceListWidget->isHideSuccess);

    diskInfoData.m_mountpoints = "/mnt";
    deviceListWidget->m_curDiskInfoData = diskInfoData;

    deviceListWidget->onHidePartitionClicked();
    ASSERT_FALSE(deviceListWidget->isHideSuccess);

    deviceListWidget->m_curDiskInfoData = oldData;
}

TEST_F(ut_application, show)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, unhidePartition), show);

    typedef int (*fptr)(DeviceListWidget*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub2;
    stub2.set(foo, MessageboxExec);

    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();

    deviceListWidget->onShowPartitionClicked();

    DMDbusHandler::instance()->onShowPartition("1");
    ASSERT_TRUE(deviceListWidget->isShowSuccess);

    DMDbusHandler::instance()->onShowPartition("0");
    ASSERT_FALSE(deviceListWidget->isShowSuccess);
}

TEST_F(ut_application, deletePartition)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, deletePartition), deletePartition);

    typedef int (*fptr)(DeviceListWidget*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub2;
    stub2.set(foo, MessageboxExec);

    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();

    DiskInfoData oldData = deviceListWidget->m_curDiskInfoData;
    DiskInfoData diskInfoData = deviceListWidget->m_curDiskInfoData;
    qDebug() << diskInfoData.m_mountpoints;
    diskInfoData.m_mountpoints = "/";
    deviceListWidget->m_curDiskInfoData = diskInfoData;

    deviceListWidget->onDeletePartitionClicked();
    ASSERT_FALSE(deviceListWidget->isDeleteSuccess);

    deviceListWidget->m_curDiskInfoData = oldData;

    deviceListWidget->onDeletePartitionClicked();

    DMDbusHandler::instance()->onDeletePartition("1:0");
    ASSERT_TRUE(deviceListWidget->isDeleteSuccess);

    DMDbusHandler::instance()->onDeletePartition("0:0");
    ASSERT_FALSE(deviceListWidget->isDeleteSuccess);
}

TEST_F(ut_application, unmountPartition)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, unmount), unmountPartition);

    typedef int (*fptr)(UnmountDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub2;
    stub2.set(foo, MessageboxExec);
    
    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();

    PartitionInfo oldInfo = DMDbusHandler::instance()->m_curPartitionInfo;
    PartitionInfo info = DMDbusHandler::instance()->m_curPartitionInfo;
    QVector<QString> vector;
    vector.append("/");
    info.m_mountPoints = vector;
    DMDbusHandler::instance()->m_curPartitionInfo = info;

    UnmountDialog *unmount = new UnmountDialog;
    unmount->onButtonClicked(1, "");
    
    DMDbusHandler::instance()->onUnmountPartition("0");
    ASSERT_FALSE(deviceListWidget->isUnmountSuccess);

    info.m_mountPoints = QVector<QString>();
    DMDbusHandler::instance()->m_curPartitionInfo = info;
    unmount->onButtonClicked(1, "");

    DMDbusHandler::instance()->onUnmountPartition("1");
    ASSERT_TRUE(deviceListWidget->isUnmountSuccess);
    
    DMDbusHandler::instance()->m_curPartitionInfo = oldInfo;
}



