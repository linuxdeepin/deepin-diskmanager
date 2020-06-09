/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     linxun <linxun@uniontech.com>
*
* Maintainer: linxun <linxun@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "devicelistwidget.h"
#include "customcontrol/dmdiskinfobox.h"
#include <DPalette>
#include <QVBoxLayout>
#include <QDebug>

DeviceListWidget::DeviceListWidget(QWidget *parent)
    : DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::white));
    setPalette(plt);
    setMaximumWidth(360);
    setMinimumWidth(100);
    initUi();
    initConnection();
}

DeviceListWidget::~DeviceListWidget()
{
    //    delete m_box;
    //    delete m_childbox;
}

void DeviceListWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);
    m_treeview = new DmTreeview(this);
    layout->addWidget(m_treeview);
}

void DeviceListWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigUpdateDeviceInfo, this, &DeviceListWidget::slotUpdateDeviceInfo);
    connect(m_treeview, &DmTreeview::sigCurSelectChanged, DMDbusHandler::instance(), &DMDbusHandler::slotsetCurSelect);
}

QString DeviceListWidget::toUtf8(char *str)
{
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec *gbk = QTextCodec::codecForName("");
    QString strtem("%1");
    strtem = strtem.arg(str);
    qDebug() << str;
    if (strtem.count("\\x") > 0) {
        QByteArray arr = str;
        qDebug() << str ;
        QByteArray ba = str;
        QString link(ba);
        QByteArray t_destByteArray;
        QByteArray t_tmpByteArray;
        for (int i = 0; i < ba.size(); i++) {
            if (92 == ba.at(i)) {
                if (4 == t_tmpByteArray.size()) {
                    t_destByteArray.append(QByteArray::fromHex(t_tmpByteArray));
                } else {
                    if (t_tmpByteArray.size() > 4) {
                        t_destByteArray.append(QByteArray::fromHex(t_tmpByteArray.left(4)));
                        t_destByteArray.append(t_tmpByteArray.mid(4));
                    } else {
                        t_destByteArray.append(t_tmpByteArray);
                    }
                }
                t_tmpByteArray.clear();
                t_tmpByteArray.append(ba.at(i));
                continue;
            } else if (t_tmpByteArray.size() > 0) {
                t_tmpByteArray.append(ba.at(i));
                continue;
            } else {
                t_destByteArray.append(ba.at(i));
            }
        }

        if (4 == t_tmpByteArray.size()) {
            t_destByteArray.append(QByteArray::fromHex(t_tmpByteArray));
        } else {
            if (t_tmpByteArray.size() > 4) {
                t_destByteArray.append(QByteArray::fromHex(t_tmpByteArray.left(4)));
                t_destByteArray.append(t_tmpByteArray.mid(4));
            } else {
                t_destByteArray.append(t_tmpByteArray);
            }
        }

        link = QTextCodec::codecForName("GBK")->toUnicode(t_destByteArray);
        qDebug() << link;
        int idx = link.lastIndexOf("/", link.length() - 1);
        QString stres = link.mid(idx + 1);
        if (strtem.count("\\x") > 0) {
            return stres;


        }
        return  stres;
    }
    return  str;
}

void DeviceListWidget::slotUpdateDeviceInfo()
{
    qDebug() << __FUNCTION__ << "               1";
    //更新DmTreeview  lx
    //设置当前选项
    auto handler = DMDbusHandler::instance();
    num = handler->getCurPartititonInfo().partition_number;
    qDebug() << flag << m_treeview->currentNum();
    devicepath_ = handler->getCurPartititonInfo().device_path;
    m_treeview->m_model->clear();
    DeviceInfoMap infomap = DMDbusHandler::instance()->probDeviceInfo();

    for (auto it = infomap.begin(); it != infomap.end(); it++) {
        DeviceInfo info = it.value();
        QString s_disksize = Utils::format_size(info.length, info.sector_size);
        auto m_box = new DmDiskinfoBox(0, this, info.m_path, s_disksize);
        for (auto it = info.partition.begin(); it != info.partition.end(); it++) {
            QString s_pdisksize = Utils::format_size(it->sector_end - it->sector_start, it->sector_size);
            QString s_partitionpath = it->path.remove(0, 5);
            QString s_unusedstr = Utils::format_size(it->sectors_used, it->sector_size);
            QString s_usedstr = Utils::format_size(it->sectors_unused, it->sector_size);
            qDebug() << it->partition_number;
            qDebug() << s_unusedstr << "unused";
            qDebug() << s_usedstr << "used";
            qDebug() << it->fstype;
            FSType fstype1 = (FSType)it->fstype;
            QString s_fstype = Utils::FSTypeToString(fstype1);
            qDebug() << it->mountpoints;
            QString s_mountpoint;
            QString s_mountpoints;
            for (int o = 0; o < it->mountpoints.size(); o++) {
                s_mountpoint = it->mountpoints[o];
                s_mountpoints += it->mountpoints[o];
                qDebug() << it->mountpoints[o];
            }
            qDebug() << s_mountpoints;
            qDebug() << s_fstype;
            qDebug() << it->filesystem_label;
            QByteArray array = it->filesystem_label.toLatin1();
            char *p = array.data();
            QString s_filesystem_label = toUtf8(p);
            qDebug() << s_filesystem_label << "cnmcnm";
            auto m_childbox = new DmDiskinfoBox(1, this, it->device_path, "", s_partitionpath, s_pdisksize, s_usedstr, s_unusedstr, it->sectors_unallocated,
                                                it->sector_start, it->sector_end, s_fstype, s_mountpoints, s_filesystem_label);
            m_box->childs.append(m_childbox);
        }

        m_treeview->addTopItem(m_box);
    }

    if (flag == 0) {
        m_treeview->setDefaultdmItem();
    } else {
        m_treeview->setRefreshItem(devicepath_, m_treeview->currentNum());
    }
    flag += 1;
    qDebug() << flag << m_treeview->currentNum();
}
