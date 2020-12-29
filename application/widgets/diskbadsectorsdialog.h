/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskbadsectorsdialog.h
 *
 * @brief 坏道检测与修复主界面窗口
 *
 * @date 2020-12-22 15:56
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
#ifndef DISKBADSECTORSDIALOG_H
#define DISKBADSECTORSDIALOG_H

#include <DDialog>
#include <DComboBox>
#include <DLineEdit>
#include <DLabel>
#include <DSlider>
#include <DPushButton>
#include <DProgressBar>
#include <DSuggestButton>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class QStackedWidget;
class CylinderInfoWidget;
class QSettings;

/**
 * @class DiskBadSectorsDialog
 * @brief 坏道检测与修复主界面类
 */

class DiskBadSectorsDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DiskBadSectorsDialog(QWidget *parent = nullptr);

    enum StatusType {
        Normal,
        Check,
        StopCheck,
        Repair,
        StopRepair
    };

signals:

public slots:

private slots:
    /**
     * @brief 接收检测范围切换的槽函数
     * @param index 当前检测范围
     */
    void onVerifyChanged(int index);

    /**
     * @brief 接收检测方式切换的槽函数
     * @param index 当前检测方式
     */
    void onMethodChanged(int index);

    /**
     * @brief 滑动条数值改变响应的槽函数
     * @param value 检测次数
     */
    void onSliderValueChanged(int value);

    /**
     * @brief 检测次数输入框改变响应的槽函数
     * @param text 检测次数
     */
    void oncheckTimesChanged(const QString &text);

    /**
     * @brief 开始检测按钮点击响应的槽函数
     */
    void onStartVerifyButtonClicked();

    /**
     * @brief 退出按钮点击响应的槽函数
     */
    void onExitButtonClicked();

    /**
     * @brief 完成按钮点击响应的槽函数
     */
    void onDoneButtonClicked();

    /**
     * @brief 停止按钮点击响应的槽函数
     */
    void onStopButtonClicked();

    /**
     * @brief 继续按钮点击响应的槽函数
     */
    void onContinueButtonClicked();

    /**
     * @brief 重新检测按钮点击响应的槽函数
     */
    void onAgainVerifyButtonClicked();

    /**
     * @brief 复位按钮点击响应的槽函数
     */
    void onResetButtonClicked();

    /**
     * @brief 坏道检测实时信息
     * @param cylinderNumber 柱面号
     * @param cylinderTimeConsuming 柱面耗时
     * @param cylinderStatus 柱面状态
     * @param cylinderErrorInfo 当前检测错误信息
     */
    void onCheckBadBlocksInfo(const QString &cylinderNumber, const QString &cylinderTimeConsuming, const QString &cylinderStatus, const QString &cylinderErrorInfo);

    /**
     * @brief 检测完成响应的槽函数
     */
    void onCheckCoomplete(int badSectorsCount);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    /**
     * @brief 初始化界面
     */
    void initUI();

    /**
     * @brief 初始化信号连接
     */
    void initConnections();

private:
    DComboBox *m_verifyComboBox;
    DLineEdit *m_startLineEdit;
    DLineEdit *m_endLineEdit;
    DLabel *m_verifyLabel;
    DComboBox *m_methodComboBox;
    DSlider *m_slider;
    DLineEdit *m_checkTimesEdit;
    DLineEdit *m_timeoutEdit;
    QStackedWidget *m_methodStackedWidget;
    DLabel *m_checkInfoLabel;
    DPushButton *m_exitButton; // 退出
    DPushButton *m_resetButton; // 复位
    DPushButton *m_repairButton; // 尝试修复
    DSuggestButton *m_startButton; // 开始检测
    DPushButton *m_stopButton; // 停止
    DPushButton *m_continueButton; // 继续
    DPushButton *m_againButton; // 重新检测
    DSuggestButton *m_doneButton; // 完成
    QStackedWidget *m_buttonStackedWidget;
    DProgressBar *m_progressBar;
    DLabel *m_usedTimeLabel;
    DLabel *m_unusedTimeLabel;
    QWidget *m_progressWidget;
    CylinderInfoWidget *m_cylinderInfoWidget;
    StatusType m_curType;
    int m_totalCheckNumber = 0;
    int m_curCheckNumber = 0;
    int m_curCheckTime = 0;
    QSettings *m_settings;
};

#endif // DISKBADSECTORSDIALOG_H
