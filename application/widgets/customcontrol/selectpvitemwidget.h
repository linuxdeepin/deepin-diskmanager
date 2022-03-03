/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file selectpvitemwidget.h
 *
 * @brief 候选PV信息展示窗口
 *
 * @date 2022-02-24 11:12
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
#ifndef SELECTPVITEMWIDGET_H
#define SELECTPVITEMWIDGET_H

#include "radiuswidget.h"
#include "buttonlabel.h"

#include <DLabel>
#include <DCheckBox>

DWIDGET_USE_NAMESPACE

/**
 * @class SelectPVItemWidget
 * @brief 候选PV信息展示类
 */

class SelectPVItemWidget : public RadiusWidget
{
    Q_OBJECT
public:
    explicit SelectPVItemWidget(PVInfoData pvInfoData, QWidget *parent = nullptr);

    void setCheckBoxState(Qt::CheckState state,  bool isPartiallyChecked = false);

    /**
     * @brief 设置磁盘下的分区数据
     * @param lstData 分区数据
     */
    void setData(const QList<PVInfoData> &lstData);

    /**
     * @brief 获取分区数据
     * @return 分区数据
     */
    QList<PVInfoData> getData();

    /**
     * @brief 获取当前数据
     * @return 当前数据
     */
    PVInfoData getCurInfo();

signals:
    void selectItem();
    void checkBoxStateChange(int state);

protected:
    /**
     * @brief 鼠标点击事件
     */
    virtual void mousePressEvent(QMouseEvent *event) override;

private slots:
    /**
     * @brief 复选框状态改变响应的槽函数
     * @param state 复选框当前状态
     */
    void onCheckBoxStateChange(int state);

private:
    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

    /**
     * @brief 初始化数据
     */
    void initData();

private:
    DCheckBox *m_checkBox;
    DLabel *m_pathLabel;
    DLabel *m_sizeLabel;
    DLabel *m_iconLabel;
    PVInfoData m_pvInfoData;
    QList<PVInfoData> m_lstPVInfoData;
};

#endif // SELECTPVITEMWIDGET_H
