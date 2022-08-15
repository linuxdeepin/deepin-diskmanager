/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file mountdialog.h
 *
 * @brief 实现分区挂载
 *
 * @date 2020-09-16 17:03
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
#ifndef MOUNTDIALOG_H
#define MOUNTDIALOG_H
#include "customcontrol/ddbase.h"

#include <dfilechooseredit.h>
#include <DComboBox>
#include <DLabel>
#include <commondef.h>

DWIDGET_USE_NAMESPACE

/**
 * @class MountDialog
 * @brief 挂载类
 */

class MountDialog : public DDBase
{
    Q_OBJECT
public:
    explicit MountDialog(QWidget *parent = nullptr);

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
     * @brief 判断挂载点是否存在
     * @return 存在返回true，否则返回false
     */
    bool isExistMountPoint(const QString &mountPoint);

    /**
     * @brief 判断是否是系统目录
     * @return 是返回true，否则返回false
     */
    bool isSystemDirectory(const QString &directory);

    /**
     * @brief 挂载当前选择的分区或者逻辑卷
     */
    void mountCurPath();

private slots:
    /**
     * @brief 挂载点输入框改变信号响应的槽函数
     * @param content 当前改变的文本
     */
    void onEditContentChanged(const QString &content);

    /**
     * @brief 按钮点击响应的槽函数
     * @param index 当前点击按钮
     * @param text 当前点击按钮文本
     */
    void onButtonClicked(int index, const QString &text);

    /**
    * @brief 检查特定分区格式对挂载点的要求
    * @param fsType 文件系统类型
    * @param mountPoint 挂载点
    */
    bool onCheckMountPoint(FSType fsType, const QString &mountPoint);

private:
    DFileChooserEdit *m_fileChooserEdit;
    DLabel *m_warnning;
//    DComboBox *m_ComboBox;
};

#endif // MOUNTDIALOG_H
