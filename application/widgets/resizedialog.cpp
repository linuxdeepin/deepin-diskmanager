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
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainFrame);
    setTitle(tr("Resize %1").arg(info.m_path));
    DLabel *tipLabel = new DLabel(tr("It will resize the partitions on the disk"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);

    QHBoxLayout *hLayout = new QHBoxLayout;
    m_lineEdit = new DLineEdit(this);
    QRegExp regexp("^[0-9]*\\.[0-9]{1,2}");
    QRegExpValidator *pvalidaor = new QRegExpValidator(regexp, this);
    m_lineEdit->lineEdit()->setValidator(pvalidaor);
    m_lineEdit->setAccessibleName("resizeSize");

    m_comboBox = new DComboBox(this);
    m_label = new DLabel(this);
    m_label->setText(tr("Reserved size:"));
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_label, DFontSizeManager::T6);
    QStringList stritems;
    stritems << "MiB"
             << "GiB";
    m_comboBox->addItems(stritems);
    m_comboBox->setCurrentIndex(0);
    m_comboBox->setFixedWidth(70);
    m_comboBox->setAccessibleName("unit");
    hLayout->addWidget(m_label);
    hLayout->addWidget(m_lineEdit);
    hLayout->addWidget(m_comboBox);

    mainLayout->addWidget(tipLabel);
    mainLayout->addLayout(hLayout);

    addButton(tr("Cancel"), false, ButtonNormal);
    m_okCode = addButton(tr("Confirm"), true, ButtonRecommend);
}

void ResizeDialog::initConnection()
{
    connect(this, &ResizeDialog::buttonClicked, this, &ResizeDialog::onButtonClicked);
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboSelectedChanged(int)));
    connect(m_lineEdit, &DLineEdit::textChanged, this, &ResizeDialog::onEditTextChanged);
}

void ResizeDialog::onButtonClicked(int index, const QString &)
{
    if (m_okCode == index) {
        auto handler = DMDbusHandler::instance();
        Q_ASSERT(handler != nullptr);
        bool canExpand = false;
        Sector expandSpace = 0;
        PartitionInfo curInfo = handler->getCurPartititonInfo();
        PartitionInfo next;
        PartitionVec arrInfo = handler->getCurDevicePartitionArr();
        int index = 0;
        for (index = 0; index < arrInfo.size(); index++) {
            PartitionInfo tem = arrInfo.at(index);
            if (curInfo == tem && index + 1 < arrInfo.size()) {
                next = arrInfo.at(index + 1);
                if (next.m_type == TYPE_UNALLOCATED) {
                    QString space = m_lineEdit->text();
                    double total = 0;
                    //目前使用GB为单位精度损失太大，如果扩容全部空闲分区会出现空隙，按当前单位转换后如果差值小于0.01默认全选
                    if (0 == m_comboBox->currentIndex()) {
                        total = Utils::sectorToUnit(next.getSectorLength(), curInfo.m_sectorSize, UNIT_MIB);
                        expandSpace = static_cast<Sector>(space.toFloat() * (MEBIBYTE / curInfo.m_sectorSize));
                        if (space.toDouble() - total >= 0.01) {
                            break;
                        }
                        if (total - space.toDouble() < 0.01)
                            expandSpace = next.getSectorLength();

                    } else {
                        total = Utils::sectorToUnit(next.getSectorLength(), curInfo.m_sectorSize, UNIT_GIB);
                        expandSpace = static_cast<Sector>(space.toFloat() * (GIBIBYTE / curInfo.m_sectorSize));
                        if (space.toDouble() - total >= 0.01) {
                            break;
                        }
                        if (total - space.toDouble() < 0.01)
                            expandSpace = next.getSectorLength();
                    }

                    qDebug() << curInfo.m_sectorSize * 1.0 << GIBIBYTE / curInfo.m_sectorSize << GIBIBYTE / (curInfo.m_sectorSize * 1.0);
                    qDebug() << Utils::formatSize(expandSpace, curInfo.m_sectorSize) << Utils::formatSize(next.getSectorLength(), curInfo.m_sectorSize);
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
            return ;

        } else {
            PartitionInfo newInfo = curInfo;
            qDebug() << Utils::formatSize(newInfo.m_sectorEnd - newInfo.m_sectorStart, curInfo.m_sectorSize) << next.m_sectorStart << next.m_sectorEnd << expandSpace << next.getSectorLength();
            //  newinfo.sector_end = expandspace > next.get_sector_length() ? next.sector_end : curinfo.sector_end + expandspace;

            newInfo.m_sectorEnd = expandSpace + next.m_sectorStart > next.m_sectorEnd ? next.m_sectorEnd : next.m_sectorStart + expandSpace;

            Sector diff = 0;
            diff = (newInfo.m_sectorEnd + 1) % (MEBIBYTE / newInfo.m_sectorSize);
            if (diff)
                newInfo.m_sectorEnd -= diff;
            else {
                // If this is a GPT partition table and the partition ends less than 34 sectors
                // from the end of the device, then reserve at least a mebibyte for the backup
                // partition table.
                DeviceInfo device = handler->getCurDeviceInfo();
                if (device.m_disktype == "gpt" && device.m_length - newInfo.m_sectorEnd < 34)
                    newInfo.m_sectorEnd -= MEBIBYTE / newInfo.m_sectorSize;
            }

            qDebug() << Utils::formatSize(newInfo.m_sectorEnd - newInfo.m_sectorStart, curInfo.m_sectorSize) << newInfo.m_sectorStart << newInfo.m_sectorEnd;
            newInfo.m_alignment = ALIGN_MEBIBYTE; //ALIGN_MEBIBYTE;
            handler->resize(newInfo);
            done(index);
        }
    } else {
        done(index);
    }
}

void ResizeDialog::onComboSelectedChanged(int index)
{
    QString space = m_lineEdit->text();
    QString expandSpace;
    if (!space.isEmpty()) {
        if (index == 0) {
            //MiB
            expandSpace = QString().setNum(space.toFloat() * 1024, 'f', 2);

        } else if (index == 1) {
            //GiB
            expandSpace = QString().setNum(space.toFloat() / 1024, 'f', 2);
        }
        m_lineEdit->setText(expandSpace);
    }
}

void ResizeDialog::onEditTextChanged(const QString &)
{
    if (m_lineEdit->isAlert())
        m_lineEdit->setAlert(false);
}
