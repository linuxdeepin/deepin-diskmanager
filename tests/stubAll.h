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

void start();

void getDeviceInfo();

void setCurSelect();

void hide();

void show();

void deletePartition();

void unmountPartition();

int MessageboxExec();

#endif // STUBALLTHREAD_H
