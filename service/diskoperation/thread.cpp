
#include "thread.h"
#include "commondef.h"

#include <QDebug>
#include <QProcess>
#include <QTime>
#include <QThread>
namespace DiskManager {

workthread::workthread(QObject *parent)
{
    m_stopFlag = 0;
}

void workthread::setStopFlag(int flag)
{
    m_stopFlag = flag;
}

void workthread::runCount()
{
    Sector i = m_blockStart;
    Sector j = m_blockStart+1;
    QProcess proc;
    while(j <= m_blockEnd+1 && m_stopFlag != 2)
    {
        QString cmd = QString("badblocks -sv -c %1 -b %2 %3 %4 %5").arg(m_checkConut).arg(m_checkSize).arg(m_devicePath).arg(j).arg(i);

        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();

        cmd = proc.readAllStandardError();

        if (cmd.indexOf("(0/0/0 errors)") != -1) {

            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "good";
            QString cylinderErrorInfo = "";
            qDebug() << "pthread" << endl;
            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Read Error";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        }

        i++;
        j++;
    }
    qDebug() << QThread::currentThreadId() << endl;
    emit checkBadBlocksDeviceStatusFinished();
}

void workthread::runTime()
{

    Sector i = m_blockStart;
    Sector j = m_blockStart+1;
    QProcess proc;
    while(j <= m_blockEnd+1 && m_stopFlag != 2)
    {
        QString cmd = QString("badblocks -sv -b %1 %2 %3 %4").arg(m_checkSize).arg(m_devicePath).arg(j).arg(i);

        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();
        cmd = proc.readAllStandardError();
        if(ctime.msecsTo(ctime1) > m_checkTime.toInt()) {
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Device Timeout";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {
            if (cmd.indexOf("(0/0/0 errors)") != -1) {
                QString cylinderNumber = QString("%1").arg(i);
                QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
                QString cylinderStatus = "good";
                QString cylinderErrorInfo = "";

                emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
            } else {
                QString cylinderNumber = QString("%1").arg(i);
                QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
                QString cylinderStatus = "bad";
                QString cylinderErrorInfo = "IO Read Error";

                emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
            }
        }
        i++;
        j++;
    }
    emit checkBadBlocksDeviceStatusFinished();
}

void workthread::runFix()
{
    int i = 0;
    QProcess proc;
    while(i < m_list.size() && m_stopFlag != 2) {
        Sector j = m_list.at(i).toInt();
        Sector k = m_list.at(i).toInt() + 1;
        QString cmd = QString("badblocks -sv -b %1 -w %2 %3 %4").arg(m_checkSize).arg(m_devicePath).arg(k).arg(j);
        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();
        cmd = proc.readAllStandardError();

        if (cmd.indexOf("(0/0/0 errors)") != -1) {

            QString cylinderNumber = QString("%1").arg(j);
            QString cylinderStatus = "good";
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            qDebug() << "pthread" << endl;
            emit fixBadBlocksInfo(cylinderNumber, cylinderStatus, cylinderTimeConsuming);
        } else {
            QString cylinderNumber = QString("%1").arg(j);
            QString cylinderStatus = "bad";
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            emit fixBadBlocksInfo(cylinderNumber, cylinderStatus, cylinderTimeConsuming);
        }
        i++;
    }

    emit checkBadBlocksDeviceStatusFinished();
}

void workthread::setConutInfo(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize)
{
    m_devicePath = devicePath;
    m_blockStart = blockStart;
    m_blockEnd = blockEnd;
    m_checkConut = checkConut;
    m_checkSize = checkSize;
}

void workthread::setTimeInfo(const QString &devicePath, int blockStart, int blockEnd, QString checkTime, int checkSize)
{
    m_devicePath = devicePath;
    m_blockStart = blockStart;
    m_blockEnd = blockEnd;
    m_checkTime = checkTime;
    m_checkSize = checkSize;
}

void workthread::setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize)
{
    m_devicePath = devicePath;
    m_list = list;
    m_checkSize = checkSize;
}
}
