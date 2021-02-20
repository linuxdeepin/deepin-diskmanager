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
#include "partchartshowing.h"

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

    MainWindow::instance()->show();

    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();
    DmTreeview *view = deviceListWidget->findChild<DmTreeview *>();
    QTest::qWait(1000);


    view->setRefreshItem(0, 0);
    QTest::qWait(1000);
    EXPECT_EQ(view->getCurrentNum(), 0);
    EXPECT_EQ(view->getCurrentTopNum(), 0);

    QRect rect = view->visualRect(view->currentIndex());
    QTest::mouseClick(view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());
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

TEST_F(ut_application, mountPartition)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, mount), mountPartition);

    typedef int (*fptr)(UnmountDialog*);
    fptr foo = (fptr)(&MessageBox::exec);
    Stub stub2;
    stub2.set(foo, MessageboxExec);

    MountDialog *mount = new MountDialog;
    mount->m_ComboBox->setCurrentText("/");
    mount->onButtonClicked(1, "");

    mount->m_ComboBox->setCurrentText("/mnt");
    mount->onButtonClicked(1, "");

    mount->onButtonClicked(0, "");
}

TEST_F(ut_application, formatPartition)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, format), formatPartition);

    Stub stub2;
    stub2.set(ADDR(DMDbusHandler, getAllSupportFileSystem), getAllSupportFileSystem);

    FormateDialog *formatDialog = new FormateDialog;
    formatDialog->m_fileNameEdit->setText("一二三四五六七");
    formatDialog->m_fileNameEdit->setText("一二三四五");

    formatDialog->onButtonClicked(1, "");
}

TEST_F(ut_application, newPartition)
{
    Stub stub;
    stub.set(ADDR(DMDbusHandler, onSetCurSelect), setCurSelectOcated);

    Stub stub2;
    stub2.set(ADDR(DMDbusHandler, create), createPartition);

    Stub stub3;
    stub3.set(ADDR(DMDbusHandler, getAllSupportFileSystem), getAllSupportFileSystem);

    CenterWidget *centerWidget = MainWindow::instance()->findChild<CenterWidget *>();
    MainSplitter *mainSplitter = centerWidget->findChild<MainSplitter *>();
    DeviceListWidget *deviceListWidget = mainSplitter->findChild<DeviceListWidget *>();
    DmTreeview *view = deviceListWidget->findChild<DmTreeview *>();
    QTest::qWait(1000);

    view->setRefreshItem(0, 2);
    QTest::qWait(1000);
    EXPECT_EQ(view->getCurrentNum(), 2);
    EXPECT_EQ(view->getCurrentTopNum(), 0);

    QRect rect = view->visualRect(view->currentIndex());
    QTest::mouseClick(view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), rect.center());

    DTitlebar *titlebar = MainWindow::instance()->findChild<DTitlebar *>();
    QWidget *widget = titlebar->findChild<QWidget *>();
    TitleWidget *titleWidget = widget->findChild<TitleWidget *>();
    QPushButton *partition = titleWidget->findChild<QPushButton *>("partition");

    ASSERT_TRUE(partition->isEnabled());

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        QWidgetList l = QApplication::topLevelWidgets();
        foreach (QWidget *w, l) {
            if (w->objectName() == "partitionDialog") {
                QTest::keyClick(w, Qt::Key_Enter);
            }
            if (w->objectName() == "partitionWidget") {
                timer->stop();

                DLineEdit *partName = w->findChild<DLineEdit *>("partName");
                DLineEdit *partSize = w->findChild<DLineEdit *>("partSize");
                DIconButton *addButton = w->findChild<DIconButton *>("addButton");
                DIconButton *removeButton = w->findChild<DIconButton *>("removeButton");
                DPushButton *confirmButton = w->findChild<DPushButton *>("confirm");
                DPushButton *revertButton = w->findChild<DPushButton *>("revert");
                PartChartShowing *partChartShowing = w->findChild<PartChartShowing *>();
                PartitionWidget *widget = qobject_cast<PartitionWidget*>(w);

                QTest::mouseMove(partChartShowing, QPoint(20, 20));
                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(20, 20));
                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(-1, -1));

                partName->setText("一二三四五六七");
                ASSERT_FALSE(addButton->isEnabled());
                QTest::qWait(1000);

                partName->setText("1111");
                ASSERT_TRUE(addButton->isEnabled());
                QTest::qWait(1000);

                partSize->setText("1");
                QTest::mouseClick(addButton, Qt::LeftButton);
                QTest::qWait(1000);
                ASSERT_EQ(widget->partCount(), 1);

                QTest::qWait(1000);
                partSize->setText("2");
                QTest::mouseClick(addButton, Qt::LeftButton);
                QTest::qWait(1000);
                ASSERT_EQ(widget->partCount(), 2);

                QTest::qWait(1000);
                QTest::mouseClick(removeButton, Qt::LeftButton);
                QTest::qWait(1000);
                ASSERT_EQ(widget->partCount(), 1);

                widget->m_partComboBox->setCurrentText("MiB");
                widget->m_partComboBox->setCurrentText("GiB");

                widget->m_slider->setValue(0);
                widget->m_slider->setValue(20);
                widget->m_slider->setValue(50);
                widget->m_slider->setValue(100);

                QTest::qWait(1000);
                QTest::mouseClick(revertButton, Qt::LeftButton);
                QTest::qWait(1000);
                ASSERT_EQ(widget->partCount(), 0);

                QTest::mouseClick(addButton, Qt::LeftButton);
                QTest::qWait(1000);

                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(400, 20));

                QTest::qWait(1000);
                QTest::mouseClick(revertButton, Qt::LeftButton);
                QTest::qWait(1000);

                partSize->setText("1");
                QTest::mouseClick(addButton, Qt::LeftButton);
                QTest::qWait(1000);

                partSize->setText("1");
                QTest::mouseClick(addButton, Qt::LeftButton);
                QTest::qWait(1000);

                QTest::mouseMove(partChartShowing, QPoint(150, 28));
                QTest::qWait(1000);
                QTest::mouseMove(partChartShowing, QPoint(350, 28));
                QTest::qWait(1000);
                QTest::mouseMove(partChartShowing, QPoint(90, 75));

                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(24, 40));
                QTest::qWait(1000);
                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(15, 40));
                QTest::qWait(1000);
                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(400, 20));

                QTest::mouseClick(addButton, Qt::LeftButton);
                QTest::qWait(1000);

                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(20, 20));
                QTest::mouseClick(partChartShowing, Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(400, 20));

                QTest::mouseMove(partChartShowing, QPoint(30, 20));

                QTest::mouseClick(confirmButton, Qt::LeftButton);
                QTest::qWait(1000);

                break;
            }
        }
    });
    timer->start(1000);

    QTest::mouseClick(partition, Qt::LeftButton);
}

TEST_F(ut_application, resize)
{

}


