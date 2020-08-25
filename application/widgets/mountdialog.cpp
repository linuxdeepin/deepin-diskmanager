#include "mountdialog.h"
#include "partedproxy/dmdbushandler.h"
#include "messagebox.h"
#include <DLabel>
#include <QVBoxLayout>
#include <DFrame>
#include <DFontSizeManager>
#include <QDir>

MountDialog::MountDialog(QWidget *parent)
    : DDBase(parent)
{
    initUi();
    initConnection();
}

void MountDialog::initUi()
{
    PartitionInfo info = DMDbusHandler::instance()->getCurPartititonInfo();
    setTitle(tr("Mount %1").arg(info.path));
    QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
    DLabel *tipLabel = new DLabel(tr("Choose a mount point please"), this);
    tipLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(tipLabel, DFontSizeManager::T6);
    DLabel *mountLabel = new DLabel(tr("Mount point:"));
    DFontSizeManager::instance()->bind(mountLabel, DFontSizeManager::T6);
//    pedit = new DFileChooserEdit(this);
//    pedit->setFileMode(QFileDialog::Directory);
//    pedit->fileDialog()->setOption(QFileDialog::ShowDirsOnly);
//    pedit->fileDialog()->setAcceptMode(QFileDialog::AcceptOpen);

    m_ComboBox = new DComboBox;
    m_ComboBox->setEditable(true);
    m_ComboBox->addItem("/mnt");
    m_ComboBox->addItem("/boot");
    m_ComboBox->addItem("/");
    m_ComboBox->addItem("/tmp");
    m_ComboBox->addItem("/var");
    m_ComboBox->addItem("/srv");
    m_ComboBox->addItem("/opt");
    m_ComboBox->addItem("/usr");
    m_ComboBox->addItem("/local");
    m_ComboBox->addItem("/media");

    mainLayout->addWidget(tipLabel);
    mainLayout->addWidget(mountLabel);
    mainLayout->addWidget(m_ComboBox);
    addButton(tr("Cancel"), true, ButtonNormal);
    okcode = addButton(tr("Mount"), false, ButtonRecommend);
//    getButton(okcode)->setDisabled(true);
//    setOnButtonClickedClose(false);
}

void MountDialog::initConnection()
{
    connect(m_ComboBox->lineEdit(), &QLineEdit::textChanged, this, &MountDialog::slotEditContentChanged);
    connect(this, &MountDialog::buttonClicked, this, &MountDialog::slotbuttonClicked);
}

void MountDialog::slotEditContentChanged(const QString &content)
{
    if (content.isEmpty()) {
        getButton(okcode)->setDisabled(true);
    } else {
        QDir dir(content);
        if (dir.exists()) {
            getButton(okcode)->setDisabled(false);
//            pedit->setAlert(false);
        } else {
            getButton(okcode)->setDisabled(true);
//            pedit->setAlert(true);
        }
    }
}

void MountDialog::slotbuttonClicked(int index, const QString &text)
{
    Q_UNUSED(text);
    if (index == okcode) {
        QDir dir(m_ComboBox->currentText());
        if (!dir.isEmpty()) {
            MessageBox messageBox;
            messageBox.setWarings(tr("The data under this mount point would be lost, please mount the directory to another location"), "", tr("OK"), tr("Cancel"));
            if (messageBox.exec() == 1) {
                DMDbusHandler::instance()->mount(m_ComboBox->currentText());
//                close();
            }
        } else {
            DMDbusHandler::instance()->mount(m_ComboBox->currentText());
//            close();
        }
    }/* else {
        close();
    }*/
}
