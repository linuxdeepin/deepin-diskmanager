/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file resizedialog.cpp
 *
 * @brief 实现分区空间调整
 *
 * @date 2020-09-17 09:25
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
#include "resizedialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QRegExpValidator>
#include <QRegExp>

ResizeDialog::ResizeDialog(QWidget *parent)
    : DDBase(parent)
{

    initUi();
    initConnection();
    setOnButtonClickedClose(false);
}

void ResizeDialog::initUi()
{
    this->setFixedWidth(380);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    mainLayout->setMargin(0);
    DLabel *tipLabel = new DLabel(tr("It will resize the partitions on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    m_lineEdit = new DLineEdit(this);
    QRegExp regexp("^[0-9]*\\.[0-9]{1,2}");
    QRegExpValidator *pvalidaor = new QRegExpValidator(regexp, this);
    m_lineEdit->lineEdit()->setValidator(pvalidaor);
    m_lineEdit->setAccessibleName("resizeSize");

    QFont font;
    font.setWeight(QFont::Normal);
    font.setFamily("Source Han Sans");
    font.setPixelSize(12);

    m_comboBox = new DComboBox(this);
    m_label = new DLabel(this);
    m_label->setText(tr("New capacity:"));
    m_label->setAlignment(Qt::AlignLeft);
    m_label->setFont(font);
    //DFontSizeManager::instance()->bind(m_label, DFontSizeManager::T6);

    QStringList stritems;
    stritems << "MiB"
             << "GiB";
    m_comboBox->addItems(stritems);
    m_comboBox->setCurrentIndex(1);
    m_comboBox->setFixedWidth(70);
    m_comboBox->setAccessibleName("unit");

    hLayout->addWidget(m_lineEdit);
    hLayout->addWidget(m_comboBox);
    vboxLayout->addWidget(m_label);
    vboxLayout->addLayout(hLayout);

    if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        DPalette palette;
        palette.setColor(DPalette::Text, QColor("#526A7F"));

        m_lvResizeLable = new DLabel(this);
        m_lvResizeLable->setText(tr("Auto adjusted to integral multiples of 4 MiB"));//自动调整为4MiB的倍数
        m_lvResizeLable->setAlignment(Qt::AlignLeft);
        m_lvResizeLable->setFont(font);
        m_lvResizeLable->setPalette(palette);
        vboxLayout->addWidget(m_lvResizeLable);
    }

    mainLayout->addWidget(tipLabel);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(vboxLayout);

    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        setTitle(tr("Resize %1").arg(info.m_path));

        m_lineEdit->setText(QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, UNIT_GIB), 'f', 2));
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        setTitle(tr("Resize %1").arg(lvInfo.m_lvName));

        tipLabel->setText(tr("It will resize the logical volume space"));
        m_lineEdit->setText(QString::number(Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, UNIT_GIB), 'f', 2));
    }

    updateInputRange();

    int index = addButton(tr("Cancel"), false, ButtonNormal);
    m_okCode = addButton(tr("Confirm"), true, ButtonRecommend);

    getButton(index)->setAccessibleName("cancel");
    getButton(m_okCode)->setAccessibleName("confirm");
    getButton(m_okCode)->setDisabled(true);
}

void ResizeDialog::initConnection()
{
    connect(this, &ResizeDialog::buttonClicked, this, &ResizeDialog::onButtonClicked);
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboSelectedChanged(int)));
    connect(m_lineEdit, &DLineEdit::textChanged, this, &ResizeDialog::onEditTextChanged);
}

void ResizeDialog::partitionResize()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    double newSize = 0;
    if (0 == m_comboBox->currentIndex()) {
        newSize = m_lineEdit->text().toDouble();
    } else {
        newSize = m_lineEdit->text().toDouble() * 1024;
    }

    Sector newSectorEnd = info.m_sectorStart + static_cast<Sector>(newSize * (MEBIBYTE / info.m_sectorSize));
    if (newSectorEnd < 0) {
        m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
        m_lineEdit->showAlertMessage(tr("Space limit exceeded"), m_mainFrame);
        m_lineEdit->setAlert(true);
        return;
    }

    Sector curSectorEnd = info.m_sectorEnd;
    FS_Limits limits = info.m_fsLimits;
    if (newSectorEnd > curSectorEnd) {
        if (limits.max_size == -1) {
            noSupportFSDailog();
            return;
        }

        bool canExpand = false;
        PartitionInfo next;
        PartitionVec arrInfo = DMDbusHandler::instance()->getCurDevicePartitionArr();
        int index = 0;
        for (index = 0; index < arrInfo.size(); index++) {
            PartitionInfo tem = arrInfo.at(index);
            if (info == tem && index + 1 < arrInfo.size()) {
                next = arrInfo.at(index + 1);
                if (next.m_type == TYPE_UNALLOCATED) {
                    canExpand = true;
                    index += 1;
                    break;
                }
            }
        }

        if (!canExpand) {
            m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
            m_lineEdit->showAlertMessage(tr("Space limit exceeded"), m_mainFrame);
            m_lineEdit->setAlert(true);
            return;
        } else {
            PartitionInfo newInfo = info;
            newInfo.m_sectorEnd = newSectorEnd;
            Sector diff = 0;
            diff = (newInfo.m_sectorEnd + 1) % (MEBIBYTE / newInfo.m_sectorSize);
            if (diff) {
                newInfo.m_sectorEnd -= diff;
            } else {
                // If this is a GPT partition table and the partition ends less than 34 sectors
                // from the end of the device, then reserve at least a mebibyte for the backup
                // partition table.
                DeviceInfo device = DMDbusHandler::instance()->getCurDeviceInfo();
                if (device.m_disktype == "gpt" && device.m_length - newInfo.m_sectorEnd < 34)
                    newInfo.m_sectorEnd -= MEBIBYTE / newInfo.m_sectorSize;
            }

            if (newInfo.m_sectorEnd > next.m_sectorEnd) {
                m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
                m_lineEdit->showAlertMessage(tr("Space limit exceeded"), m_mainFrame);
                m_lineEdit->setAlert(true);
                return;
            } else {
                newInfo.m_alignment = ALIGN_MEBIBYTE; //ALIGN_MEBIBYTE;
                qDebug() << diff << newInfo.m_sectorEnd;
                DMDbusHandler::instance()->resize(newInfo);
                close();
            }
        }
    } else if (newSectorEnd < curSectorEnd) {
        if (limits.min_size == -1) {
            MessageBox warningBox(this);
            warningBox.setObjectName("messageBox");
            warningBox.setAccessibleName("LVNoSupportFSWidget");
            // 该文件系统不支持空间缩小  确定
            warningBox.setWarings(tr("The file system does not support shrinking space"), "", tr("OK"), "ok");
            warningBox.exec();
            return;
        }

        Sector minReduceSize = info.m_sectorStart + limits.min_size / info.m_sectorSize - 1;
        if (newSectorEnd < minReduceSize) {
            MessageBox messageBox(this);
            messageBox.setObjectName("messageBox");
            messageBox.setAccessibleName("PartitionReduceDialog");
            messageBox.setWarings(tr("Not enough space, please clear data in it"), "", tr("OK"), "ok");
            messageBox.exec();
        } else {
            MessageBox messageBox(this);
            messageBox.setObjectName("messageBox");
            messageBox.setAccessibleName("messageBox");
            messageBox.setWarings(tr("To prevent data loss, back up data before shrinking it"), "", tr("OK"), "ok", tr("Cancel"), "cancel");
            if (messageBox.exec() == DDialog::Accepted) {
                PartitionInfo newInfo = info;
                newInfo.m_sectorEnd = newSectorEnd;

                Sector diff = 0;
                diff = (newInfo.m_sectorEnd + 1) % (MEBIBYTE / newInfo.m_sectorSize);
                if (diff) {
                    newInfo.m_sectorEnd -= diff;
                }

                DMDbusHandler::instance()->resize(newInfo);
                close();
            }
        }
    }
}

void ResizeDialog::lvResize()
{
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    int peSize = vgInfo.m_PESize;

    LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
    Sector curSize = lvInfo.m_lvLECount * lvInfo.m_LESize;
    Sector unUsedSize = static_cast<Sector>(vgInfo.m_PESize * vgInfo.m_peUnused);
    FS_Limits limits = lvInfo.m_fsLimits;
    if (lvInfo.m_lvFsType == FS_UNKNOWN || lvInfo.m_lvFsType == FS_UNALLOCATED) { //如果没有文件系统 不限制放大缩小
        limits = FS_Limits();
    }

    Sector newSize = 0;
    if (0 == m_comboBox->currentIndex()) {
        newSize = static_cast<Sector>(m_lineEdit->text().toDouble() * MEBIBYTE);
    } else {
        newSize = static_cast<Sector>(m_lineEdit->text().toDouble() * GIBIBYTE);
    }

    Sector peCount = newSize / peSize;
    if ((peCount * peSize) < newSize) {
        peCount += 1;
    }
    newSize = peCount * peSize;

    if ((newSize - curSize) > unUsedSize) {
        m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
        m_lineEdit->showAlertMessage(tr("Space limit exceeded"), m_mainFrame);
        m_lineEdit->setAlert(true);
        return;
    }

    if (newSize < 0 || newSize < limits.min_size) {
        MessageBox messageBox(this);
        messageBox.setObjectName("messageBox");
        messageBox.setAccessibleName("LVResizeWarning");
        messageBox.setWarings(tr("Not enough space, please clear data in it"), "", tr("OK"), "ok");
        messageBox.exec();

        return;
    }

    if (newSize < curSize) {
        if (limits.min_size == -1) {
            noSupportFSDailog();
            return;
        }

        MessageBox messageBox(this);
        messageBox.setObjectName("messageBox");
        messageBox.setAccessibleName("messageBox");
        if (lvInfo.m_busy) {
            //当前设备已挂载，将会自动卸载，请对其内的数据做好备份，以防数据丢失
            messageBox.setWarings(tr("The current device has been mounted and will be unmounted automatically. Please back up data in it to prevent data loss"), "", tr("OK"), "ok", tr("Cancel"), "cancel");
        } else {
            //缩减逻辑卷前，请对其内的数据做好备份，以防数据丢失
            messageBox.setWarings(tr("To prevent data loss, back up data in the logical volume before shrinking it"), "", tr("OK"), "ok", tr("Cancel"), "cancel");
        }


        if (messageBox.exec() == DDialog::Accepted) {
            LVAction info;
            info.m_vgName = lvInfo.m_vgName;
            info.m_lvName = lvInfo.m_lvName;
            info.m_lvByteSize = newSize;
            info.m_lvAct = LVMAction::LVM_ACT_LV_REDUCE;
            info.m_lvFs = lvInfo.m_lvFsType;
            DMDbusHandler::instance()->resizeLV(info);
            close();
        }
    } else if (newSize > curSize) {
        if (limits.max_size == -1) {
            noSupportFSDailog();
            return;
        }

        LVAction info;
        info.m_vgName = lvInfo.m_vgName;
        info.m_lvName = lvInfo.m_lvName;
        info.m_lvByteSize = newSize;
        info.m_lvAct = LVMAction::LVM_ACT_LV_EXTEND;
        info.m_lvFs = lvInfo.m_lvFsType;
        DMDbusHandler::instance()->resizeLV(info);
        close();
    }
}

void ResizeDialog::noSupportFSDailog()
{
    MessageBox warningBox(this);
    warningBox.setObjectName("messageBox");
    warningBox.setAccessibleName("LVNoSupportFSWidget");
    // 该文件系统不支持调整空间  确定
    warningBox.setWarings(tr("The file system does not support space adjustment"), "", tr("OK"), "ok");
    warningBox.exec();
}

void ResizeDialog::onButtonClicked(int index, const QString &)
{
    if (m_okCode == index) {
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
            partitionResize();
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
            lvResize();
        }
    } else {
        done(index);
    }
}

void ResizeDialog::onComboSelectedChanged(int index)
{
    if (index == 0) {
        //MiB
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
            m_lineEdit->setText(QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, UNIT_MIB), 'f', 2));
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            m_lineEdit->setText(QString::number(Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, UNIT_MIB), 'f', 2));
        }

        getButton(m_okCode)->setDisabled(true);
    } else if (index == 1) {
        //GiB
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
            m_lineEdit->setText(QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, UNIT_GIB), 'f', 2));
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            m_lineEdit->setText(QString::number(Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, UNIT_GIB), 'f', 2));
        }

        getButton(m_okCode)->setDisabled(true);
    }

    updateInputRange();
}

void ResizeDialog::onEditTextChanged(const QString &)
{
    if (!m_lineEdit->text().isEmpty() && m_lineEdit->text().toDouble() != 0.00) {
        getButton(m_okCode)->setDisabled(false);
        if (m_lineEdit->isAlert()) {
            m_lineEdit->setAlert(false);
        }

        SIZE_UNIT unit = (0 == m_comboBox->currentIndex()) ? UNIT_MIB : UNIT_GIB;
        double setSize = 0;
        double curSize = 0;
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) { //分区调整
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo(); //获取磁盘
            setSize = m_lineEdit->text().toDouble();                                //获取当前设置大小
            curSize = Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, unit);//获取磁盘大小
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) { //lv调整
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            double tmpUnit = (unit == UNIT_MIB) ? MEBIBYTE : GIBIBYTE;
            Byte_Value tmp = static_cast<Byte_Value>(m_lineEdit->text().toDouble() * tmpUnit); //获取输入大小
            setSize = (tmp % lvInfo.m_LESize == 0) ? tmp / tmpUnit : ((tmp / lvInfo.m_LESize) + 1) * lvInfo.m_LESize / tmpUnit;
            curSize = Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, unit);
        }
        getButton(m_okCode)->setDisabled(QString::number(setSize, 'f', 2) == QString::number(curSize, 'f', 2));

    } else {
        getButton(m_okCode)->setDisabled(true);
    }
}

void ResizeDialog::updateInputRange()
{
    SIZE_UNIT unit = (0 == m_comboBox->currentIndex()) ? UNIT_MIB : UNIT_GIB;
    QString minSize = "0";
    QString maxSize = "0";
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        FS_Limits limits = info.m_fsLimits;

        if (limits.min_size == -1) {
            minSize = QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, unit), 'f', 2);
        } else {
            minSize = QString::number(Utils::LVMSizeToUnit(limits.min_size, unit), 'f', 2);
        }

        bool canExpand = false;
        PartitionInfo next;
        PartitionVec arrInfo = DMDbusHandler::instance()->getCurDevicePartitionArr();
        int index = 0;
        for (index = 0; index < arrInfo.size(); index++) {
            PartitionInfo tem = arrInfo.at(index);
            if (info == tem && index + 1 < arrInfo.size()) {
                next = arrInfo.at(index + 1);
                if (next.m_type == TYPE_UNALLOCATED) {
                    canExpand = true;
                    index += 1;
                    break;
                }
            }
        }

        if (!canExpand) {
            maxSize = QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, unit), 'f', 2);
        } else {
            maxSize = QString::number(Utils::sectorToUnit(next.m_sectorEnd - next.m_sectorStart + 1, next.m_sectorSize, unit), 'f', 2);
        }
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        Sector curSize = lvInfo.m_lvLECount * lvInfo.m_LESize;
        int peSize = vgInfo.m_PESize;
        Sector unUsedSize = static_cast<Sector>(peSize * vgInfo.m_peUnused);
        FS_Limits limits = lvInfo.m_fsLimits;

        if (limits.min_size == -1) {
            minSize = QString::number(Utils::LVMSizeToUnit(curSize, unit), 'f', 2);
        } else if (limits.min_size == 0) {
            minSize = QString::number(Utils::LVMSizeToUnit(peSize, unit), 'f', 2);
        } else {
            minSize = QString::number(Utils::LVMSizeToUnit(limits.min_size, unit), 'f', 2);
        }

        maxSize = QString::number(Utils::LVMSizeToUnit(curSize + unUsedSize, unit), 'f', 2);
    }

    m_lineEdit->lineEdit()->setPlaceholderText(QString("%1-%2").arg(minSize).arg(maxSize));
}
