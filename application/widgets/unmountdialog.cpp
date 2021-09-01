/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file unmountdialog.cpp
 *
 * @brief 实现分区卸载
 *
 * @date 2020-09-17 09:12
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
#include "unmountdialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DLabel>
#include <DFrame>
#include <DFontSizeManager>

#include <QVBoxLayout>

UnmountDialog::UnmountDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void UnmountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();

    setTitle(tr("Unmount %1").arg(info.m_path));

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    DLabel *tipLabel = new DLabel(tr("Make sure there are no programs running on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    mainLayout->addWidget(tipLabel);

    int index = addButton(tr("Cancel"), false, ButtonNormal);
    m_okCode = addButton(tr("Unmount"), false, ButtonWarning);

    getButton(index)->setAccessibleName("cancel");
    getButton(m_okCode)->setAccessibleName("unmountButton");
}

void UnmountDialog::initConnection()
{
    connect(this, &UnmountDialog::buttonClicked, this, &UnmountDialog::onButtonClicked);
}

void UnmountDialog::onButtonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (m_okCode == index) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        QString mountpoints;

        for (int i = 0; i < info.m_mountPoints.size(); i++) {
            mountpoints += info.m_mountPoints[i];
        }

        if (mountpoints == "/boot/efi" || mountpoints == "/boot" || mountpoints == "/"
                || mountpoints == "/data/home/opt/root/var" || mountpoints == "/recovery"
                || mountpoints == "/deepin/userdata/home/opt/root/var") {
            MessageBox firstWarning(this);
            firstWarning.setObjectName("firstWarning");
            firstWarning.setAccessibleName("firstWarning");
            // 卸载该系统盘可能会引起系统崩溃，请确认是否继续操作  继续  取消
            QString title = tr("Unmounting system disk may result in system crash,\n please confirm before proceeding");
            firstWarning.setWarings(title, "", tr("Continue"), DDialog::ButtonWarning, "continueBtn", tr("Cancel"), "cancelButton");
            if (firstWarning.exec() == 1) {
                MessageBox secondWarning(this);
                secondWarning.setObjectName("secondWarning");
                secondWarning.setAccessibleName("secondWarning");
                // 继续执行卸载操作，后续引发的风险将由您自行承担  卸载  取消
                QString title = tr("You will take subsequent risks if you continue to unmount the system disk");
                secondWarning.setWarings(title, "", tr("Unmount"), DDialog::ButtonWarning, "unmountBtn", tr("Cancel"), "cancelBtn");
                if (secondWarning.exec() == 1) {
                    DMDbusHandler::instance()->unmount();
                }
            }
        } else {
            DMDbusHandler::instance()->unmount();
        }
    }
}
