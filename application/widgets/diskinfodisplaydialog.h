/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file diskinfodisplaydialog.h
 *
 * @brief 磁盘信息类
 *
 * @date 2020-08-19 17:01
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
#ifndef DISKINFODISPLAYDIALOG_H
#define DISKINFODISPLAYDIALOG_H

#include <DDialog>
#include <DCommandLinkButton>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

/**
 * @class DiskInfoDisplayDialog
 * @brief 磁盘信息类
 */

class DiskInfoDisplayDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DiskInfoDisplayDialog(const QString &devicePath, QWidget *parent = nullptr);

signals:

public slots:

private slots:
    /**
     * @brief 导出按钮点击响应的槽函数
     */
    void onExportButtonClicked();

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
    QStringList m_diskInfoNameList; // 磁盘属性名称
    QStringList m_diskInfoValueList; // 磁盘属性值
    DCommandLinkButton *m_linkButton; // 导出按钮
    QString m_devicePath; // 当前磁盘路径

};

#endif // DISKINFODISPLAYDIALOG_H
