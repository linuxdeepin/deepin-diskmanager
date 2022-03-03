/**
 * @copyright 2020-2022 Uniontech Technology Co., Ltd.
 *
 * @file createvgwidget.h
 *
 * @brief VG创建窗口
 *
 * @date 2022-02-18 11:05
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
#ifndef CREATEVGWIDGET_H
#define CREATEVGWIDGET_H

#include "customcontrol/ddbase.h"
#include "partedproxy/dmdbushandler.h"
#include "customcontrol/selectpvitemwidget.h"
#include "customcontrol/selecteditemwidget.h"
#include "customcontrol/waterloadingwidget.h"

#include <DComboBox>
#include <DLineEdit>
#include <DLabel>
#include <DPushButton>
#include <DWarningButton>
#include <DSpinner>
#include <DStackedWidget>
#include <DSuggestButton>
#include <DScrollArea>

#include <QWidget>

DWIDGET_USE_NAMESPACE

/**
 * @class CreateVGWidget
 * @brief VG创建类
 */

class CreateVGWidget : public DDBase
{
    Q_OBJECT
public:
    explicit CreateVGWidget(QWidget *parent = nullptr);

signals:

private slots:
    /**
     * @brief 下一步按钮点击响应的槽函数
     */
    void onNextButtonClicked();

    /**
     * @brief 上一步按钮点击响应的槽函数
     */
    void onPreviousButtonClicked();

    /**
     * @brief 完成按钮点击响应的槽函数
     */
    void onDoneButtonClicked();

    /**
     * @brief 取消按钮点击响应的槽函数
     */
    void onCancelButtonClicked();

    /**
     * @brief 磁盘信息条目点击响应的槽函数
     */
    void onDiskItemClicked();

    /**
     * @brief 磁盘信息选中响应的槽函数
     */
    void onDiskCheckBoxStateChange(int state);

    /**
     * @brief 分区信息选中响应的槽函数
     */
    void onPartitionCheckBoxStateChange(int state);

    /**
     * @brief 删除选择的磁盘分区信息响应的槽函数
     */
    void onDeleteItemClicked(PVInfoData pvInfoData);

    /**
     * @brief 下拉框容量单位切换
     * @param index 当前选择下标
     */
    void onCurrentIndexChanged(int index);

    /**
     * @brief 当前文本改变
     * @param text 当前文本
     */
    void onTextChanged(const QString &text);

    /**
     * @brief VG创建结果
     * @param vgMessage vg创建信息
     */
    void onVGCreateMessage(const QString &vgMessage);

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
     * @brief 显示加载界面
     */
    void showLoadingWidget(const QString &vgName);

    /**
     * @brief 更新数据
     */
    void updateData();

    /**
     * @brief 可创建VG的磁盘数据
     * @return 磁盘数据列表
     */
    QList<DeviceInfo> availableDiskData();

    /**
     * @brief 更新分区数据
     */
    void updatePartitionData(const QList<PVInfoData> &lstData);

    /**
     * @brief 更新已选择数据
     */
    void updateSelectedData();

    /**
     * @brief 获取当前容量大小单位
     * @return 返回当前容量大小单位
     */
    SIZE_UNIT getCurSizeUnit();

    /**
     * @brief 获取逻辑卷组名称
     * @return 逻辑卷组名称
     */
    QString getVGName();

private:
   DStackedWidget *m_stackedWidget;
   DLabel *m_seclectedLabel;
   DStackedWidget *m_firstCenterStackedWidget;
   DPushButton *m_firstCancelButton;
   DSuggestButton *m_nextButton;
   DFrame *m_partitionFrame;
   DStackedWidget *m_partitionStackedWidget;
   DFrame *m_diskFrame;
   DScrollArea *m_diskScrollArea;
   DScrollArea *m_partitionScrollArea;
   DPushButton *m_addButton;
   DPushButton *m_secondCancelButton;
   DPushButton *m_previousButton;
   DSuggestButton *m_doneButton;
   SelectPVItemWidget *m_curDiskItemWidget;
   QList<PVInfoData> m_curSeclectData;
   DFrame *m_selectedFrame;
   DScrollArea *m_selectedScrollArea;
   DStackedWidget *m_selectedStackedWidget;
   DLabel *m_selectSpaceLabel;
   DLineEdit *m_selectSpaceLineEdit;
   DComboBox *m_selectSpaceComboBox;
   DStackedWidget *m_selectSpaceStackedWidget;
   DLabel *m_selectedSpaceLabel;
   double m_minSize = 0;
   double m_maxSize = 0;
   long long m_curSize;
   QWidget *m_loadingWidget;
   long long m_sumSize;
   WaterLoadingWidget *m_waterLoadingWidget;
};

#endif // CREATEVGWIDGET_H
