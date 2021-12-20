/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file dmframewidget.h
 *
 * @brief 主界面右侧信息展示下部分实现类
 *
 * @date 2020-09-17 15:52
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DMFRAMEWIDGET_H
#define DMFRAMEWIDGET_H

#include "dmtreeviewdelegate.h"

#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
#include <DFontSizeManager>
#include <dirent.h>

#include <QWidget>
#include <QPainter>
#include <QTextOption>
#include <QTextCodec>
#include <QByteArray>
#include <QDebug>
#include <QProcess>

DWIDGET_USE_NAMESPACE

/**
 * @class DmFrameWidget
 * @brief 分区信息下部分实现类
 */

class DmFrameWidget : public DFrame
{
    Q_OBJECT
public:
    explicit DmFrameWidget(DiskInfoData data, QWidget *parent = nullptr);

    /**
     * @brief 设置分区信息，更新界面显示
     */
    void setFrameData();

    /**
     * @brief 设置磁盘信息，更新界面显示
     */
    void setDiskFrameData(const QString &path, const QString &diskType, const QString &used,
                          const QString &unused, const QString &capacity, const QString &interface);

signals:
private:
    /**
     * @brief 获取分区卷标
     * @param partitionPath 分区路径
     */
    QString diskVolumn(QString partitionPath);

private slots:
    /**
     * @brief 主题切换信号响应的槽函数
     */
    void onHandleChangeTheme();

    /**
     * @brief 实现挂载点过长，显示时中间省略
     * @param width 宽
     * @param mountpoints 挂载点
     */
//    QString diffMountpoints(int width, QString mountpoints);

protected:
    /**
     * @brief 重写绘画事件
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 重写调整事件
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    DiskInfoData m_infoData;
    DPalette m_parentPb;
    int m_width = 0;
    QString m_str;
};

#endif // DMFRAMEWIDGET_H
