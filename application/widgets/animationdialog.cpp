/**
 * @copyright 2020-2021 Uniontech Technology Co., Ltd.
 *
 * @file animationdialog.cpp
 *
 * @brief 动画窗口类
 *
 * @date 2021-12-10 16:10
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
#include "animationdialog.h"

#include <DWindowCloseButton>

#include <QVBoxLayout>
#include <QKeyEvent>

AnimationDialog::AnimationDialog(QWidget *parent) : DDialog(parent)
{
    initUi();
}

void AnimationDialog::initUi()
{
    setFixedSize(380, 160);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    DFrame *m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameRounded(false);
    m_mainFrame->setFrameStyle(DFrame::NoFrame);
    addContent(m_mainFrame);

    m_label = new DLabel(this);
    m_label->setAccessibleName("animationTitle");
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    m_spinner = new DSpinner(m_mainFrame);
    m_spinner->setFixedSize(28, 28);
    m_spinner->start();
    m_spinner->hide();

    mainLayout->addWidget(m_label, 0, Qt::AlignCenter | Qt::AlignTop);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_spinner, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void AnimationDialog::setShowSpinner(bool isShow, const QString &title)
{
    if (isShow) {
        m_label->setText(title);
        m_spinner->show();
    } else {
        m_spinner->hide();
    }
}

void AnimationDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Escape) {
        event->ignore();
    } else {
        DDialog::keyPressEvent(event);
    }
}






