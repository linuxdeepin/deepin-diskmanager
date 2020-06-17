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
#include "dmframewidget.h"
#include "utils.h"
#include "partitioninfo.h"
#include "partedproxy/dmdbushandler.h"
#include <DFontSizeManager>
#include <DApplicationHelper>
#include <QDebug>

DmFrameWidget::DmFrameWidget(DiskInfoData data, QWidget *parent)
    : DFrame(parent)
    , m_infodata(data)
{
    m_parentPb = DApplicationHelper::instance()->palette(this);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &DmFrameWidget::slothandleChangeTheme);
}

void DmFrameWidget::setFrameData()
{
    //获取首页相关硬盘数据
    PartitionInfo data = DMDbusHandler::instance()->getCurPartititonInfo();
    QString mountstr;
    QString previoustr;
    QString laststr;
    QString strstr;
    DiskInfoData temp;
    m_infodata = temp;
    for (QString strpoint : data.mountpoints)
        strstr.append(strpoint + " ");
    qDebug() << strstr;
    m_infodata.mountpoints = diffMountpoints(d_width, strstr);
    m_infodata.unused = Utils::format_size(data.sectors_unused, data.sector_size);
    if (m_infodata.unused.contains("-")) {
        m_infodata.unused = "-";
    }
    m_infodata.used = Utils::format_size(data.sectors_used, data.sector_size);
    if (m_infodata.used.contains("-")) {
        m_infodata.used = "-";
    }
    QString partitionpath = data.path.remove(0, 5);
    m_infodata.fstype = Utils::FSTypeToString(static_cast<FSType>(data.fstype));
    m_infodata.partitionsize = Utils::format_size(data.sector_end - data.sector_start, data.sector_size);
    if (data.filesystem_label == "") {
        m_infodata.syslabel = "";
    } else {
        m_infodata.syslabel = diskVolumn(partitionpath);
    }
    update();
}

QString DmFrameWidget::diskVolumn(QString partitionpath)
{
    //将gbkｕ盘ｌａｂｅｌ中文乱码转换为正常中文显示
    QProcess app;
    app.start("ls", QStringList() << "-al" << "/dev/disk/by-label/");
    if (!app.waitForStarted()) {
        qWarning() << "Cmd Exec Failed:" << app.errorString();
    }
    if (!app.waitForFinished(-1)) {
        qWarning() << "waitForFinished Failed:" << app.errorString();
    }
    QString standardError = app.readAllStandardOutput();
    QStringList mounts = standardError.split("\n").filter(partitionpath);
    QString sr = mounts.last();
    QString st = sr.mid(40).remove(" -> ../../" + partitionpath);
    std::string s = st.toStdString();
    const char *strstr = s.c_str();
    QString strtem("%1");
    strtem = strtem.arg(st);
    if (strtem.count("\\x") > 0) {
        QByteArray arr = strstr;
        QByteArray ba = strstr;
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
        int idx = link.lastIndexOf("/", link.length() - 1);
        QString stres = link.mid(idx + 1);
        if (strtem.count("\\x") > 0) {
            return  stres;
        }
    } else {
        return  st;
    }
    return "";
}

void DmFrameWidget::paintEvent(QPaintEvent *event)//绘制首页信息展示表格
{
    QPainter painter(this);
    QWidget::paintEvent(event);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        m_parentPb = DApplicationHelper::instance()->palette(this);
        QBrush brush = DApplicationHelper::instance()->palette(this).itemBackground();
        painter.setBrush(brush);
        QColor outsidecolor(qRgba(0, 0, 0, 1));
        outsidecolor.setAlphaF(0.1);
        painter.setPen(outsidecolor);
        QRect rect = this->rect();
        rect.setWidth(rect.width());
        rect.setHeight(rect.height() - 1);
        painter.drawRoundedRect(rect, 15, 15);
        QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
        m_parentPb = DApplicationHelper::instance()->palette(this);
        QBrush brush1 = DApplicationHelper::instance()->palette(this).textLively();
        painter.setBrush(brush1);
        painter.fillRect(paintRect, brush1);
        painter.drawLine(paintRect.width() / 2, rect.topLeft().y(), paintRect.width() / 2, rect.bottomLeft().y());
        QRect textRect = QRect(rect.width() / 2 - 300, rect.topLeft().y() + 12, 240, 35);
        QColor textcolor = this->palette().color(DPalette::Normal, DPalette::Text);
        QTextOption option;
        option.setTextDirection(Qt::LeftToRight);
        option.setAlignment(Qt::AlignRight);
        QTextOption option1;
        option1.setAlignment(Qt::AlignRight);
        painter.setPen(textcolor);
        QRect textRect1 = QRect(rect.width() / 2 - 275, rect.topLeft().y() + 10, 257, 40);
        painter.drawText(textRect1, m_infodata.mountpoints, option);
        textRect.moveTo(rect.width() / 2 - 260, rect.topLeft().y() + 62);
        painter.drawText(textRect, m_infodata.unused, option1);
        textRect.moveTo(rect.width() / 2 - 260, rect.topLeft().y() + 113);
        painter.drawText(textRect, m_infodata.used, option1);
        textRect.moveTo(rect.width() - 260, rect.topLeft().y() + 10);
        painter.drawText(textRect, m_infodata.fstype, option1);
        textRect.moveTo(rect.width() - 260, rect.topLeft().y() + 60);
        painter.drawText(textRect, m_infodata.partitionsize, option1);
        textRect.moveTo(rect.width() - 260, rect.topLeft().y() + 110);
        painter.drawText(textRect, m_infodata.syslabel, option1);
        painter.restore();
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        m_parentPb = DApplicationHelper::instance()->palette(this);
        QColor color = m_parentPb.color(DPalette::Normal, DPalette::TextLively);
        color.setAlphaF(0.05);
        painter.setBrush(QBrush(color));
        QColor outsidecolor(qRgba(0, 0, 0, 1));
        outsidecolor.setAlphaF(0.1);
        painter.setPen(outsidecolor);
        QRect rect = this->rect();
        rect.setWidth(rect.width() - 1);
        rect.setHeight(rect.height() - 1);
        painter.drawRoundedRect(rect, 15, 15);
        QRect paintRect = QRect(rect.topLeft().x() + 1, rect.topLeft().y() + (rect.height() / 3), rect.width() - 2, rect.height() / 3);
        QColor midcolor = QColor("#252525");
        painter.setBrush(QBrush(midcolor));
        painter.fillRect(paintRect, midcolor);
        painter.drawLine(paintRect.width() / 2, rect.topLeft().y(), paintRect.width() / 2, rect.bottomLeft().y());
        QRect textRect = QRect(rect.width() / 2 - 400, rect.topLeft().y() + 10, 240, 35);
        QFont font = DFontSizeManager::instance()->get(DFontSizeManager::T8);
        QColor textcolor = this->palette().color(DPalette::Normal, DPalette::WindowText);
        QTextOption option;
        option.setTextDirection(Qt::LeftToRight);
        option.setAlignment(Qt::AlignRight);
        QTextOption option1;
        option1.setAlignment(Qt::AlignRight);
        painter.setPen(textcolor);
        QRect textRect1 = QRect(rect.width() / 2 - 275, rect.topLeft().y() + 10, 257, 40);
        painter.drawText(textRect1, m_infodata.mountpoints, option);
        textRect.moveTo(rect.width() / 2 - 260, rect.topLeft().y() + 62);
        painter.drawText(textRect, m_infodata.unused, option1);
        textRect.moveTo(rect.width() / 2 - 260, rect.topLeft().y() + 113);
        painter.drawText(textRect, m_infodata.used, option1);
        textRect.moveTo(rect.width() - 260, rect.topLeft().y() + 10);
        painter.drawText(textRect, m_infodata.fstype, option1);
        textRect.moveTo(rect.width() - 260, rect.topLeft().y() + 60);
        painter.drawText(textRect, m_infodata.partitionsize, option1);
        textRect.moveTo(rect.width() - 260, rect.topLeft().y() + 110);
        painter.drawText(textRect, m_infodata.syslabel, option1);
        painter.restore();
    }
}

void DmFrameWidget::resizeEvent(QResizeEvent *event)
{
    //实时获取整体的大小
    this->QWidget::resizeEvent(event);
    d_width = this->width();
    setFrameData();
}
void DmFrameWidget::slothandleChangeTheme()
{
    m_parentPb = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
}

QString DmFrameWidget::diffMountpoints(int width, QString mountpoints)
{
    //区分过长的挂载点，中间做...显示
    QString previoustr;
    QString laststr;

    if (mountpoints.size() > 21) {
        if (width < 1000) {
            for (int i = 0; i < 9; i++) {
                previoustr += mountpoints.at(i);
            }
            previoustr += "...";
            for (int p = mountpoints.size() - 9; p < mountpoints.size(); p++) {
                laststr += mountpoints.at(p);
            }
            mountpoints = previoustr + laststr;
            return mountpoints;
        } else {
            return mountpoints;
        }
    } else {
        return mountpoints;
    }
}
