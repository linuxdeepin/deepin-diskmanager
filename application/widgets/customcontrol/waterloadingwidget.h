/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file waterloadingwidget.h
 *
 * @brief 水滴加载动画窗口
 *
 * @date 2022-03-02 11:24
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
#ifndef WATERLOADINGWIDGET_H
#define WATERLOADINGWIDGET_H

#include <DWaterProgress>

#include <QWidget>
#include <QTimer>

DWIDGET_USE_NAMESPACE

/**
 * @class WaterLoadingWidget
 * @brief 水滴加载动画类
 */

class WaterLoadingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaterLoadingWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置定时器时间
     * @param msec 毫秒
     */
    void setStartTime(int msec);

    /**
     * @brief 停止定时器
     */
    void stopTimer();

signals:

public slots:

private slots:
    /**
     * @brief 水滴动画进度改变响应的槽函数
     */
    void onStartWaterProgress();

private:
    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

private:
    DWaterProgress *m_waterProgress;
    QTimer *m_time;
};

#endif // WATERLOADINGWIDGET_H
