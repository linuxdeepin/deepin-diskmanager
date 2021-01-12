/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mainwindow.h
 *
 * @brief 主窗口类
 *
 * @date 2020-09-04 10:00
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DSpinner>
#include <DPushButton>

DWIDGET_USE_NAMESPACE

class CenterWidget;
class BufferWin;
class DMDbusHandler;

/**
 * @class MainWindow
 * @brief 主窗口类
 */

class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static MainWindow *instance();

protected:

    /**
     * @brief 窗口关闭事件
     */
    void closeEvent(QCloseEvent *event) override;

private slots:

    /**
     * @brief 刷新按钮点击响应的槽函数
     */
    void onRefreshButtonClicked();

private:

    /**
     * @brief 初始化界面
     */
    void initUi();

    /**
     * @brief 初始化连接
     */
    void initConnection();

signals:

public slots:

    /**
     * @brief Handle退出槽函数
     */
    void onHandleQuitAction();

    /**
     * @brief 接收是否显示动画的槽函数
     * @param isShow ture显示，false不显示
     */
    void onShowSpinerWindow(bool isShow = false);

private:
    CenterWidget *m_central;
//    BufferWin *m_bufferWin;
    DMDbusHandler *m_handler;
    DSpinner *m_spinner;
    DPushButton *m_btnRefresh; // 刷新按钮
};

#endif // MAINWINDOW_H
