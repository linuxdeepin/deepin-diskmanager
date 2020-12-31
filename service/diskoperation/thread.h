#ifndef THREAD_H
#define THREAD_H
#include <QObject>

namespace DiskManager {

class workthread :public QObject
{
     Q_OBJECT
public:
    workthread(QObject *parent = nullptr);

    /**
     * @brief 设置检测次数信息
     * @param devicePath：设备路径
     * @param blockStart：开始柱面信息号
     * @param blockEnd：检测结束柱面号
     * @param checkConut：检测次数
     * @param checkSize：检测柱面范围大小
     */
    void setConutInfo(const QString &devicePath, int blockStart, int blockEnd, int checkConut, int checkSize);

    /**
     * @brief 设置检测超时时间信息
     * @param devicePath：设备路径
     * @param blockStart：开始柱面信息号
     * @param blockEnd：检测结束柱面号
     * @param checkTime：检测超时时间
     * @param checkSize：检测柱面范围大小
     */
    void setTimeInfo(const QString &devicePath, int blockStart, int blockEnd, QString checkTime, int checkSize);

    /**
     * @brief 设置修复数据
     * @param devicePath：设备路径
     * @param list: 修复柱面集合
     * @param checkSize：检测柱面范围大小
     */
    void setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize);

    /**
     * @brief 设置停止状态
     * @param flag：停止状态
     */
    void setStopFlag(int flag);

public slots:

    /**
     * @brief 坏道检测线程(检测次数方式)
     */
    void runCount();

    /**
     * @brief 坏道检测线程(超时时间方式)
     */
    void runTime();

signals:

    /**
     * @brief 坏道检测检测信息信号(次数检测)
     * @param cylinderNumber：检测柱面号
     * @param cylinderTimeConsuming：柱面耗时
     * @param cylinderStatus：柱面状态
     * @param cylinderErrorInfo：柱面错误信息
     */
    void checkBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);

    /**
     * @brief 坏道检测完成信号
     */
    void checkBadBlocksFinished();

    /**
     * @brief 坏道检测线程回收信号
     */
    void checkBadBlocksDeviceStatusFinished();

private:
    QString m_devicePath;   //设备路径
    int m_blockStart;       //开始检测柱面号
    int m_blockEnd;         //检测结束柱面号
    int m_checkConut;       //检测次数
    int m_checkSize;        //检测柱面大小
    QString m_checkTime;    //检测超时时间
    int m_stopFlag;         //暂停状态
};


class fixthread :public QObject
{
    Q_OBJECT
public:
    fixthread(QObject *parent = nullptr);

    /**
     * @brief 设置停止状态
     * @param flag：停止状态
     */
    void setStopFlag(int flag);

    /**
     * @brief 设置修复数据
     * @param devicePath：设备路径
     * @param list: 修复柱面集合
     * @param checkSize：检测柱面范围大小
     */
    void setFixBadBlocksInfo(const QString &devicePath, QStringList list, int checkSize);

public slots:
    /**
     * @brief 坏道修复线程
     */
    void runFix();

signals:

    /**
     * @brief 坏道修复信息信号
     * @param cylinderNumber：检测柱面号
     * @param cylinderStatus：柱面状态
     */
    void fixBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderStatus, const QString &cylinderTimeConsuming);

    /**
     * @brief 坏道检测线程回收信号
     */
    void checkBadBlocksDeviceStatusFinished();

    /**
     * @brief 坏道修复完成信号
     */
    void fixBadBlocksFinished();

private:
    QString m_devicePath;   //设备路径
    int m_stopFlag;         //暂停状态
    QStringList m_list;     //需要修复柱面集合
    int m_checkSize;        //检测柱面大小
};
}
#endif // THREAD_H
