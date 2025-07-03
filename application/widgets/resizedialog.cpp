// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only


#include "resizedialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"
#include <QDebug>

#include <DFontSizeManager>

#include <QHBoxLayout>
#if QT_VERSION_MAJOR > 5
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#else
#include <QRegExpValidator>
#include <QRegExp>
#endif

ResizeDialog::ResizeDialog(QWidget *parent)
    : DDBase(parent)
{
    qDebug() << "ResizeDialog initialized";
    initUi();
    initConnection();
    setOnButtonClickedClose(false);
    qDebug() << "ResizeDialog initialized.";
}

void ResizeDialog::initUi()
{
    qDebug() << "Initializing ResizeDialog UI.";
    this->setFixedWidth(380);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
#if QT_VERSION_MAJOR > 5
        qDebug() << "Setting contents margins for Qt6 or higher.";
        mainLayout->setContentsMargins(0, 0, 0, 0);
#else
    qDebug() << "Setting margin for Qt5.";
    mainLayout->setMargin(0);
#endif
    DLabel *tipLabel = new DLabel(tr("It will resize the partitions on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    m_lineEdit = new DLineEdit(this);
#if QT_VERSION_MAJOR > 5
    qDebug() << "Using QRegularExpressionValidator for Qt6 or higher.";
    QRegularExpression regexp("^[0-9]*\\.[0-9]{1,2}");
    QRegularExpressionValidator *pvalidaor = new QRegularExpressionValidator(regexp, this);
#else
    qDebug() << "Using QRegExpValidator for Qt5.";
    QRegExp regexp("^[0-9]*\\.[0-9]{1,2}");
    QRegExpValidator *pvalidaor = new QRegExpValidator(regexp, this);
#endif
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
        qDebug() << "Current level is LOGICALVOLUME, adding LV resize label.";
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
        qDebug() << "Current level is PARTITION, setting title and line edit text for partition resize.";
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        setTitle(tr("Resize %1").arg(info.m_path));

        m_lineEdit->setText(QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, UNIT_GIB), 'f', 2));
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Current level is LOGICALVOLUME, setting title and line edit text for logical volume resize.";
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
    qDebug() << "ResizeDialog UI initialization completed.";
}

void ResizeDialog::initConnection()
{
    qDebug() << "Initializing ResizeDialog connections.";
    connect(this, &ResizeDialog::buttonClicked, this, &ResizeDialog::onButtonClicked);
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboSelectedChanged(int)));
    connect(m_lineEdit, &DLineEdit::textChanged, this, &ResizeDialog::onEditTextChanged);
    qDebug() << "ResizeDialog connections initialized.";
}

void ResizeDialog::partitionResize()
{
    qDebug() << "partitionResize called.";
    qInfo() << "Starting partition resize operation";
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    double newSize = 0;
    if (0 == m_comboBox->currentIndex()) {
        qDebug() << "Unit is MiB, newSize:" << m_lineEdit->text().toDouble();
        newSize = m_lineEdit->text().toDouble();
    } else {
        qDebug() << "Unit is GiB, newSize:" << m_lineEdit->text().toDouble() * 1024;
        newSize = m_lineEdit->text().toDouble() * 1024;
    }

    Sector newSectorEnd = info.m_sectorStart + static_cast<Sector>(newSize * (MEBIBYTE / info.m_sectorSize));
    if (newSectorEnd < 0) {
        qDebug() << "Calculated newSectorEnd is less than 0, showing alert.";
        m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
        m_lineEdit->showAlertMessage(tr("No more than the maximum capacity please"), m_mainFrame); // 超出最大可用空间
        m_lineEdit->setAlert(true);
        qWarning() << "New size exceeds maximum capacity for partition:" << info.m_path;
        return;
    }

    Sector curSectorEnd = info.m_sectorEnd;
    FS_Limits limits = info.m_fsLimits;
    if (newSectorEnd > curSectorEnd) {
        qDebug() << "Attempting to expand partition.";
        if (limits.max_size == -1) {
            qDebug() << "File system does not support expansion, showing noSupportFSDailog.";
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
                qDebug() << "Found current partition, checking next partition.";
                next = arrInfo.at(index + 1);
                if (next.m_type == TYPE_UNALLOCATED) {
                    qDebug() << "Next partition is unallocated, can expand.";
                    canExpand = true;
                    index += 1;
                    break;
                }
            }
        }

        if (!canExpand) {
            qDebug() << "Cannot expand partition, no unallocated space.";
            m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
            m_lineEdit->showAlertMessage(tr("No more than the maximum capacity please"), m_mainFrame); // 超出最大可用空间
            m_lineEdit->setAlert(true);
            qWarning() << "No unallocated space available for expanding partition:" << info.m_path;
            return;
        } else {
            qDebug() << "Preparing to resize partition.";
            PartitionInfo newInfo = info;
            newInfo.m_sectorEnd = newSectorEnd;
            Sector diff = 0;
            diff = (newInfo.m_sectorEnd + 1) % (MEBIBYTE / newInfo.m_sectorSize);
            if (diff) {
                qDebug() << "Adjusting newInfo.m_sectorEnd due to diff.";
                newInfo.m_sectorEnd -= diff;
            } else {
                // If this is a GPT partition table and the partition ends less than 34 sectors
                // from the end of the device, then reserve at least a mebibyte for the backup
                // partition table.
                DeviceInfo device = DMDbusHandler::instance()->getCurDeviceInfo();
                if (device.m_disktype == "gpt" && device.m_length - newInfo.m_sectorEnd < 34)
                {
                    qDebug() << "GPT partition and near end of device, reserving space for backup partition table.";
                    newInfo.m_sectorEnd -= MEBIBYTE / newInfo.m_sectorSize;
                }
            }

            // 处理输入最大值，没有成功扩容的问题
            if (m_maxSize == QString::number(m_lineEdit->text().toDouble(), 'f', 2)) {
                qDebug() << "Input is max size, adjusting newInfo.m_sectorEnd.";
                DeviceInfo device = DMDbusHandler::instance()->getCurDeviceInfo();
                if (device.m_disktype == "gpt" && device.m_length == (next.m_sectorEnd + 1)) {
                    qDebug() << "GPT disk and next sector is device end, adjusting for GPT.";
                    newInfo.m_sectorEnd = next.m_sectorEnd - 33;
                } else {
                    qDebug() << "Setting newInfo.m_sectorEnd to next.m_sectorEnd.";
                    newInfo.m_sectorEnd = next.m_sectorEnd;
                }
            }

            if (newInfo.m_sectorEnd > next.m_sectorEnd) {
                qDebug() << "New sector end is greater than next sector end, showing alert.";
                m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
                m_lineEdit->showAlertMessage(tr("No more than the maximum capacity please"), m_mainFrame); // 超出最大可用空间
                m_lineEdit->setAlert(true);
                qWarning() << "New size exceeds adjacent unallocated space for partition:" << info.m_path;
                return;
            } else {
                qDebug() << "Resizing partition via D-Bus.";
                newInfo.m_alignment = ALIGN_MEBIBYTE; //ALIGN_MEBIBYTE;
                qDebug() << "diff:" << diff << ", newInfo.m_sectorEnd:" << newInfo.m_sectorEnd;
                DMDbusHandler::instance()->resize(newInfo);
                close();
            }
        }
    } else if (newSectorEnd < curSectorEnd) {
        qDebug() << "Attempting to shrink partition.";
        if (limits.min_size == -1) {
            qDebug() << "File system does not support shrinking, showing warning message box.";
            MessageBox warningBox(this);
            warningBox.setObjectName("messageBox");
            warningBox.setAccessibleName("LVNoSupportFSWidget");
            // 该文件系统不支持空间缩小  确定
            warningBox.setWarings(tr("The file system does not support shrinking space"), "", tr("OK"), "ok");
            warningBox.exec();
            return;
        }

        Sector minReduceSize = info.m_sectorStart + limits.min_size / info.m_sectorSize - 1;

        // 处理输入最小值，弹出错误提示的问题
        if (m_minSize == QString::number(m_lineEdit->text().toDouble(), 'f', 2) && newSectorEnd < minReduceSize) {
            qDebug() << "Input is min size, adjusting newSectorEnd to minReduceSize.";
            newSectorEnd = minReduceSize;
        }

        if (newSectorEnd < minReduceSize) {
            qDebug() << "New sector end is less than minimum reduce size, showing alert.";
            m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
            m_lineEdit->showAlertMessage(tr("No less than the used capacity please"), m_mainFrame); // 不得低于已用空间
            m_lineEdit->setAlert(true);
            return;
        } else {
            qDebug() << "Showing backup warning message box before shrinking.";
            MessageBox messageBox(this);
            messageBox.setObjectName("messageBox");
            messageBox.setAccessibleName("messageBox");
            messageBox.setWarings(tr("To prevent data loss, back up data before shrinking it"), "", tr("OK"), "ok", tr("Cancel"), "cancel");
            if (messageBox.exec() == DDialog::Accepted) {
                qDebug() << "User accepted shrinking, proceeding with resize.";
                PartitionInfo newInfo = info;
                newInfo.m_sectorEnd = newSectorEnd;

                Sector diff = 0;
                diff = (newInfo.m_sectorEnd + 1) % (MEBIBYTE / newInfo.m_sectorSize);
                if (diff) {
                    newInfo.m_sectorEnd -= diff;
                }

                if (newInfo.m_sectorEnd < minReduceSize) {
                    newInfo.m_sectorEnd = minReduceSize;
                }

                DMDbusHandler::instance()->resize(newInfo);
                close();
            }
        }
    }
    qDebug() << "partitionResize completed.";
}

void ResizeDialog::lvResize()
{
    qInfo() << "Starting LV resize operation";
    VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
    int peSize = vgInfo.m_PESize;

    LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
    Sector curSize = lvInfo.m_lvLECount * lvInfo.m_LESize;
    Sector unUsedSize = static_cast<Sector>(vgInfo.m_PESize * vgInfo.m_peUnused);
    FS_Limits limits = lvInfo.m_fsLimits;
    if (lvInfo.m_lvFsType == FS_UNKNOWN || lvInfo.m_lvFsType == FS_UNALLOCATED) { //如果没有文件系统 不限制放大缩小
        qInfo() << "LV does not have a file system, no resize limits.";
        limits = FS_Limits();
    }

    // 如果当前lv是系统盘，最小值为lv的大小
    if (DMDbusHandler::instance()->getIsSystemDisk(lvInfo.m_vgName)) {
        qInfo() << "LV is a system disk, setting min size to LV size.";
        limits.min_size = curSize;
    }

    Sector newSize = 0;
    if (0 == m_comboBox->currentIndex()) {
        qInfo() << "Resizing LV to MiB.";
        newSize = static_cast<Sector>(m_lineEdit->text().toDouble() * MEBIBYTE);
    } else {
        qInfo() << "Resizing LV to GiB.";
        newSize = static_cast<Sector>(m_lineEdit->text().toDouble() * GIBIBYTE);
    }

    // 处理输入最小值，弹出错误提示的问题
    if (m_minSize == QString::number(m_lineEdit->text().toDouble(), 'f', 2) && newSize < limits.min_size) {
        qInfo() << "Input is min size, adjusting newSize to minSize.";
        newSize = limits.min_size;
    }

    if (newSize < limits.min_size) {
        qInfo() << "New size is less than minimum size, showing alert.";
        m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
        m_lineEdit->showAlertMessage(tr("No less than the used capacity please"), m_mainFrame); // 不得低于已用空间
        m_lineEdit->setAlert(true);
        qWarning() << "New size exceeds maximum capacity for LV:" << vgInfo.m_vgName;
        return;
    }

    Sector peCount = newSize / peSize;
    if ((peCount * peSize) < newSize) {
        qInfo() << "New size is not a multiple of PE size, rounding up.";
        peCount += 1;
    }
    newSize = peCount * peSize;

    // 处理输入最大值，没有成功扩容或者直接弹出错误提示的问题
    if (m_maxSize == QString::number(m_lineEdit->text().toDouble(), 'f', 2) && newSize > curSize) {
        qInfo() << "Input is max size, adjusting newSize to maxSize.";
        newSize = curSize + unUsedSize;
    }

    if (newSize < 0 || (newSize - curSize) > unUsedSize) {
        qInfo() << "New size exceeds maximum capacity for LV, showing alert.";
        m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
        m_lineEdit->showAlertMessage(tr("No more than the maximum capacity please"), m_mainFrame); // 超出最大可用空间
        m_lineEdit->setAlert(true);
        return;
    }

    if (newSize < curSize) {
        qInfo() << "New size is less than current size, checking min size.";
        if (limits.min_size == -1) {
            qInfo() << "File system does not support shrinking, showing warning message box.";
            noSupportFSDailog();
            return;
        }

        Sector usedSize = lvInfo.m_fsUsed;
        if (limits.min_size < usedSize) {
            qInfo() << "File system used size is less than minimum size, showing warning message box.";
            m_lineEdit->setAlertMessageAlignment(Qt::AlignTop);
            m_lineEdit->showAlertMessage(tr("Unmount it before shrinking its space"), m_mainFrame); // 缩小空间前请先卸载
            m_lineEdit->setAlert(true);
            return;
        }

        MessageBox messageBox(this);
        messageBox.setObjectName("messageBox");
        messageBox.setAccessibleName("messageBox");
        if (lvInfo.m_busy) {
            qInfo() << "LV is busy, showing warning message box.";
            //当前设备已挂载，将会自动卸载，请对其内的数据做好备份，以防数据丢失
            messageBox.setWarings(tr("The current device has been mounted and will be unmounted automatically. Please back up data in it to prevent data loss"), "", tr("OK"), "ok", tr("Cancel"), "cancel");
        } else {
            qInfo() << "LV is not busy, showing warning message box.";
            //缩减逻辑卷前，请对其内的数据做好备份，以防数据丢失
            messageBox.setWarings(tr("To prevent data loss, back up data in the logical volume before shrinking it"), "", tr("OK"), "ok", tr("Cancel"), "cancel");
        }

        if (messageBox.exec() == DDialog::Accepted) {
            qInfo() << "User accepted shrinking, proceeding with resize.";
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
        qInfo() << "New size is greater than current size, checking max size.";
        if (limits.max_size == -1) {
            qInfo() << "File system does not support extending, showing warning message box.";
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
    qDebug() << "LV resize completed.";
}

void ResizeDialog::noSupportFSDailog()
{
    qDebug() << "Showing warning message box for file system does not support space adjustment.";
    MessageBox warningBox(this);
    warningBox.setObjectName("messageBox");
    warningBox.setAccessibleName("LVNoSupportFSWidget");
    // 该文件系统不支持调整空间  确定
    warningBox.setWarings(tr("The file system does not support space adjustment"), "", tr("OK"), "ok");
    warningBox.exec();
}

void ResizeDialog::onButtonClicked(int index, const QString &)
{
    qDebug() << "Button clicked with index:" << index;
    if (m_okCode == index) {
        qDebug() << "OK button clicked, starting resize operation.";
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
            qDebug() << "Partition resize operation started.";
            partitionResize();
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
            qDebug() << "LV resize operation started.";
            lvResize();
        }
    } else {
        qDebug() << "Cancel button clicked, closing dialog.";
        done(index);
    }
}

void ResizeDialog::onComboSelectedChanged(int index)
{
    qDebug() << "Unit selection changed to index:" << index;
    if (index == 0) {
        qDebug() << "Selected unit is MiB.";
        //MiB
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
            qDebug() << "Partition resize unit is MiB.";
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
            m_lineEdit->setText(QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, UNIT_MIB), 'f', 2));
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
            qDebug() << "LV resize unit is MiB.";
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            m_lineEdit->setText(QString::number(Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, UNIT_MIB), 'f', 2));
        }

        getButton(m_okCode)->setDisabled(true);
    } else if (index == 1) {
        qDebug() << "Selected unit is GiB.";
        //GiB
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
            qDebug() << "Partition resize unit is GiB.";
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
            m_lineEdit->setText(QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, UNIT_GIB), 'f', 2));
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
            qDebug() << "LV resize unit is GiB.";
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            m_lineEdit->setText(QString::number(Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, UNIT_GIB), 'f', 2));
        }

        getButton(m_okCode)->setDisabled(true);
    }

    updateInputRange();
}

void ResizeDialog::onEditTextChanged(const QString &)
{
    qDebug() << "Size input changed to:" << m_lineEdit->text();
    if (!m_lineEdit->text().isEmpty() && m_lineEdit->text().toDouble() != 0.00) {
        qDebug() << "Size input is valid.";
        getButton(m_okCode)->setDisabled(false);
        if (m_lineEdit->isAlert()) {
            qDebug() << "Size input is in alert state.";
            m_lineEdit->setAlert(false);
        }

        SIZE_UNIT unit = (0 == m_comboBox->currentIndex()) ? UNIT_MIB : UNIT_GIB;
        double setSize = 0;
        double curSize = 0;
        if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) { //分区调整
            qDebug() << "Partition resize unit is:" << unit;
            PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo(); //获取磁盘
            setSize = m_lineEdit->text().toDouble();                                //获取当前设置大小
            curSize = Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, unit);//获取磁盘大小
        } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) { //lv调整
            qDebug() << "LV resize unit is:" << unit;
            LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
            double tmpUnit = (unit == UNIT_MIB) ? MEBIBYTE : GIBIBYTE;
            Byte_Value tmp = static_cast<Byte_Value>(m_lineEdit->text().toDouble() * tmpUnit); //获取输入大小
            setSize = (tmp % lvInfo.m_LESize == 0) ? tmp / tmpUnit : ((tmp / lvInfo.m_LESize) + 1) * lvInfo.m_LESize / tmpUnit;
            curSize = Utils::LVMSizeToUnit(lvInfo.m_lvLECount * lvInfo.m_LESize, unit);
        }
        getButton(m_okCode)->setDisabled(QString::number(setSize, 'f', 2) == QString::number(curSize, 'f', 2));

    } else {
        qDebug() << "Size input is invalid.";
        getButton(m_okCode)->setDisabled(true);
    }
}

void ResizeDialog::updateInputRange()
{
    qDebug() << "Updating input range.";
    SIZE_UNIT unit = (0 == m_comboBox->currentIndex()) ? UNIT_MIB : UNIT_GIB;
    m_minSize = "0";
    m_maxSize = "0";
    if (DMDbusHandler::PARTITION == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "Partition resize unit is:" << unit;
        PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
        FS_Limits limits = info.m_fsLimits;

        if (limits.min_size == -1) {
            m_minSize = QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, unit), 'f', 2);
        } else {
            m_minSize = QString::number(Utils::LVMSizeToUnit(limits.min_size, unit), 'f', 2);
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
            m_maxSize = QString::number(Utils::sectorToUnit(info.m_sectorEnd - info.m_sectorStart + 1, info.m_sectorSize, unit), 'f', 2);
        } else {
            m_maxSize = QString::number(Utils::sectorToUnit(next.m_sectorEnd - info.m_sectorStart + 1, next.m_sectorSize, unit), 'f', 2);
        }
    } else if (DMDbusHandler::LOGICALVOLUME == DMDbusHandler::instance()->getCurLevel()) {
        qDebug() << "LV resize unit is:" << unit;
        VGInfo vgInfo = DMDbusHandler::instance()->getCurVGInfo();
        LVInfo lvInfo = DMDbusHandler::instance()->getCurLVInfo();
        Sector curSize = lvInfo.m_lvLECount * lvInfo.m_LESize;
        int peSize = vgInfo.m_PESize;
        Sector unUsedSize = static_cast<Sector>(peSize * vgInfo.m_peUnused);
        FS_Limits limits = lvInfo.m_fsLimits;

        if (limits.min_size == -1 || DMDbusHandler::instance()->getIsSystemDisk(lvInfo.m_vgName)) {
            m_minSize = QString::number(Utils::LVMSizeToUnit(curSize, unit), 'f', 2);
        } else if (limits.min_size == 0) {
            m_minSize = QString::number(Utils::LVMSizeToUnit(peSize, unit), 'f', 2);
        } else {
            m_minSize = QString::number(Utils::LVMSizeToUnit(limits.min_size, unit), 'f', 2);
        }

        m_maxSize = QString::number(Utils::LVMSizeToUnit(curSize + unUsedSize, unit), 'f', 2);
    }

    m_lineEdit->lineEdit()->setPlaceholderText(QString("%1-%2").arg(m_minSize).arg(m_maxSize));
    qDebug() << "Input range updated.";
}
