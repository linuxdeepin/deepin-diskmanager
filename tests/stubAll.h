#ifndef STUBALL_H
#define STUBALL_H

#include <QThread>
#include <QThreadPool>
#include <QDebug>
//#include <DDialog>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include "../basestruct/commondef.h"
#include "../basestruct/deviceinfo.h"

void start();

void getDeviceInfo();

void setCurSelect();

void setCurSelectSecond();

void setCurSelectOcated();

QStringList getAllSupportFileSystem();

void hide();

void show();

void deletePartition();

void unmountPartition();

void mountPartition();

void formatPartition();

void createPartition();

void resizePartition();

int MessageboxExec();

HardDiskInfo getDiskInfo();

QString filePath();

QString emptyFilePath();

QString errorFilePath();

QString noPermissionFilePath();

QString passedDeviceStatus();

QString failureDeviceStatus();

QString unknownDeviceStatus();

HardDiskStatusInfoList deviceCheckHealthInfo();

#endif // STUBALLTHREAD_H
