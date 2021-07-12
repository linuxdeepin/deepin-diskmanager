/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file messagebox.cpp
 *
 * @brief 提示信息窗口类，主要实现各种样式的弹窗
 *
 * @date 2020-08-19 16:44
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

#include "messagebox.h"

#include <DLabel>

#include <QAbstractButton>

MessageBox::MessageBox(QWidget *parent) : DDialog(parent)
{

}

void MessageBox::setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, const QString &sureAccessibleName, const QString &cancalBtnText, const QString &cancelAccessibleName)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    if (!title.isEmpty()) {
        setTitle(title);
        addSpacing(10);
    }

    if (!warningMsg.isEmpty()) {
        addLabel(warningMsg);
        addSpacing(10);
    }

    if (!cancalBtnText.isEmpty()) {
        int index = addButton(cancalBtnText);
        getButton(index)->setAccessibleName(cancelAccessibleName);
    }

    int index = addButton(sureBtnText);
    getButton(index)->setAccessibleName(sureAccessibleName);
    connect(this, &MessageBox::buttonClicked, this, [=]() {
        close();
    });
}

void MessageBox::setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, ButtonType sureBtnType, const QString &sureAccessibleName, const QString &cancalBtnText, const QString &cancelAccessibleName)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    if (!title.isEmpty()) {
        setTitle(title);
        addSpacing(10);
    }

    if (!warningMsg.isEmpty()) {
        addLabel(warningMsg);
        addSpacing(10);
    }

    if (!cancalBtnText.isEmpty()) {
        int index = addButton(cancalBtnText);
        getButton(index)->setAccessibleName(cancelAccessibleName);
    }

    int index = addButton(sureBtnText, false, sureBtnType);
    getButton(index)->setAccessibleName(sureAccessibleName);
    connect(this, &MessageBox::buttonClicked, this, [=]() {
        close();
    });
}

void MessageBox::setProgressBar(const QString &title, const QString &cancalBtnText)
{
    setIcon(QIcon::fromTheme("://icons/deepin/builtin/exception-logo.svg"));

    if (!title.isEmpty()) {
        setTitle(title);
        addSpacing(10);
    }

    addProgressBar();
    addSpacing(10);

    addButton(cancalBtnText);
    connect(this, &MessageBox::buttonClicked, this, [=]() {
        close();
    });
}

void MessageBox::addLabel(QString text)
{
    DLabel *label= new DLabel(this);
    label->setText(text);
    addContent(label, Qt::AlignHCenter);
}

void MessageBox::addProgressBar()
{
    m_progressBar = new DProgressBar(this);
    m_progressBar->setValue(50);
    m_progressBar->setFixedSize(330, 10);
    addContent(m_progressBar, Qt::AlignHCenter);
}


