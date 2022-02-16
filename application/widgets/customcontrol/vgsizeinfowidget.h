/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file vgsizeinfowidget.h
 *
 * @brief 逻辑卷组信息中间部分实现类
 *
 * @date 2022-01-20 09:23
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
#ifndef VGSIZEINFOWIDGET_H
#define VGSIZEINFOWIDGET_H

#include "partitioninfo.h"
#include "deviceinfo.h"

#include <DPalette>
#include <DApplicationHelper>

#include <QWidget>
#include <QRect>
#include <QPainterPath>
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include <QPointF>
#include <QLine>
#include <QColor>
#include <QVector>

DWIDGET_USE_NAMESPACE

/**
 * @class VGSizeInfoWidget
 * @brief 逻辑卷组信息中间部分实现类
 */

class VGSizeInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VGSizeInfoWidget(QWidget *parent = nullptr);

    /**
     * @brief 设置数据
     * @param info 当前磁盘信息
     */
    void setData(const DeviceInfo &info);

    /**
     * @brief 设置数据
     * @param info 当前逻辑卷组信息
     */
    void setData(const VGInfo &info);

    /**
     * @brief 设置数据
     * @param info 当前磁盘下逻辑卷组信息
     */
    void setData(const QVector<VGData> &vglist);

    /**
     * @brief 设置数据
     * @param info 当前分区下逻辑卷组信息
     */
    void setData(const VGData &vgData);

signals:
    void enterWidget(QRect rect, const QList< QMap<QString, QVariant> > &lstInfo);

protected:
    /**
     * @brief 重写绘画事件
     */
    virtual void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 鼠标移动事件
     */
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief 离开事件
     */
    virtual void leaveEvent(QEvent *event) override;

private slots:
    /**
     * @brief 主题切换信号响应的槽函数
     */
    void onHandleChangeTheme();

private:
    double m_totalSize = 0.00;
    QVector<QColor> m_colorInfo;
    QVector<double> m_sizeInfo;
    QVector<QString> m_pathInfo;
    QVector<QRectF> m_reectInfo;
    DPalette m_parentPb;
    QVector<QString> m_pathList;
    QVector<QString> m_pathSizeInfo;
    QVector<QColor> m_pathColorInfo;
    QRect m_rectEllipsis;
    bool m_isEllipsis = false;
    int m_showNumber = 0;
};

#endif // VGSIZEINFOWIDGET_H
