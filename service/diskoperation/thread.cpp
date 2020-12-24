
#include "thread.h"
#include "commondef.h"

#include <QDebug>
#include <QProcess>
#include <QTime>
namespace DiskManager {

workthread::workthread(QObject *parent)
{

}

void workthread::runCount()
{
    Sector i = m_blockStart;
    Sector j = m_blockStart+1;
    QProcess proc;
    while(j>0 && j <= m_blockEnd+1)
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
    return;
}

void workthread::runTime()
{

    Sector i = m_blockStart;
    Sector j = m_blockStart+1;
    QProcess proc;
    while(j>0 && j <= m_blockEnd+1)
    {
        QString cmd = QString("badblocks -sv -b %1 %2 %3 %4").arg(m_checkSize).arg(m_devicePath).arg(j).arg(i);

        QTime ctime = QTime::currentTime();
        proc.start(cmd);
        proc.waitForFinished(-1);
        QTime ctime1 = QTime::currentTime();
        qDebug() << "phread:time:" << i << endl;
        cmd = proc.readAllStandardError();
        if (cmd.indexOf("(0/0/0 errors)") != -1 && ctime.msecsTo(ctime1) < m_checkTime.toInt()) {
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "good";
            QString cylinderErrorInfo = "";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else if(ctime.msecsTo(ctime1) > m_checkTime.toInt()) {
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Device Timeout";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else if(cmd.indexOf("(1/0/0 errors)") != -1 && ctime.msecsTo(ctime1) < m_checkTime.toInt()) {
            QString cylinderNumber = QString("%1").arg(i);
            QString cylinderTimeConsuming = QString("%1").arg(ctime.msecsTo(ctime1));
            QString cylinderStatus = "bad";
            QString cylinderErrorInfo = "IO Read Error";

            emit checkBadBlocksInfo(cylinderNumber, cylinderTimeConsuming, cylinderStatus, cylinderErrorInfo);
        } else {
            emit checkBadBlocksDeviceStatusError();
        }
        i++;
        j++;
    }
    return;
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
}
