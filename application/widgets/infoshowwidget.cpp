#include "infoshowwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>


DWIDGET_USE_NAMESPACE

InfoShowWidget::InfoShowWidget(DWidget *parent): DWidget(parent)
{
    setAutoFillBackground(true);
    auto plt = this->palette();
    plt.setBrush(QPalette::Background, QBrush(Qt::lightGray));
    setPalette(plt);
    initUi();
    initConnection();
}



void InfoShowWidget::paintEvent(QPaintEvent *event)
{
//    DWidget::paintEvent(event);

    DPalette pa = DApplicationHelper::instance()->palette(pframetop);
    pa.setBrush(DPalette::Background, pa.background());
    DApplicationHelper::instance()->setPalette(pframetop, pa);
//    QPainter painter(this);
//    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

}



void InfoShowWidget::initUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    pframe = new DFrame;
    layout->addWidget(pframe);

    QVBoxLayout *framelayout = new QVBoxLayout(pframe);
    framelayout->setContentsMargins(10, 10, 10, 10);
    pframetop = new DFrame;
    pframetop->setFixedHeight(100);
    framelayout->addWidget(pframetop);
    topFrameSettings();


    pframemid = new DFrame;
    pframemid->setFixedHeight(200);
    pframemid->setFrameShape(QFrame::NoFrame);
    framelayout->addWidget(pframemid);
    midFramSettings();

    DFrame *pframebottom = new DFrame;
    // pframebottom->setFixedHeight(300);
    framelayout->addWidget(pframebottom);
    framelayout->addStretch();

}

void InfoShowWidget::initConnection()
{
    connect(DMDbusHandler::instance(), &DMDbusHandler::sigCurSelectChanged, this, &InfoShowWidget::slotCurSelectChanged);
}


void InfoShowWidget::topFrameSettings()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(pframetop);
//    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 0, 20, 0);

    QVBoxLayout *v = new QVBoxLayout();
    v->addStretch();
    v->setSpacing(30);
    v->setMargin(20);
    picLabel = new DLabel(pframetop);
    picLabel->setMinimumSize(50, 60);
    picLabel->setStyleSheet("border:1px solid gray");
    v->addWidget(picLabel);
    v->addStretch();
    mainLayout->addLayout(v);

    QVBoxLayout *v1 = new QVBoxLayout();
    v1->addStretch();
    nameLabel = new DLabel(pframetop);
    typeLabel = new DLabel(pframetop);

    QFont nameFont;
    nameFont.setBold(true);
    nameFont.setPointSize(12);
    nameLabel->setFont(nameFont);
    QFont formatFont;
    formatFont.setPointSize(8);
    typeLabel->setFont(formatFont);

    nameLabel->setAlignment(Qt::AlignLeft);
    typeLabel->setAlignment(Qt::AlignLeft);

    v1->addWidget(nameLabel);
    v1->addWidget(typeLabel);
    v1->addStretch();

    DPalette pa = DApplicationHelper::instance()->palette(typeLabel);
    pa.setBrush(DPalette::Text, pa.textTips());
    DApplicationHelper::instance()->setPalette(typeLabel, pa);
    mainLayout->addLayout(v1);

    mainLayout->addStretch();


    QVBoxLayout *v2 = new QVBoxLayout();
    v2->addStretch();
    allnameLabel = new DLabel(pframetop);
    allmemoryLabel = new DLabel(pframetop);
    QFont font;
    font.setBold(true);
    allnameLabel->setFont(font);

    QFont memoryFont;
    memoryFont.setPointSize(20);
    allmemoryLabel->setFont(memoryFont);

    allnameLabel->setText(tr("Total capacity"));
    allnameLabel->setAlignment(Qt::AlignRight);
    allmemoryLabel->setAlignment(Qt::AlignRight);

    v2->addWidget(allnameLabel);
    v2->addWidget(allmemoryLabel);
    v2->addStretch();
    mainLayout->addLayout(v2);

    slotShowDiskInfo("", "", "");
}


void InfoShowWidget::midFramSettings()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(pframemid);
    m_infowidget = new SizeInfoWidget(100, 200, 300);
    mainLayout->addWidget(m_infowidget);
//    totaluseLabel = new DLabel(pframemid);
//    QFont  font;
//    font.setPointSize(20);
//    QPixmap pixmap;
//    pixmap.load(":/icons/deepin/builtin/exception-logo.svg");
//    totaluseLabel->setPixmap(pixmap);
//    totaluseLabel->setGeometry(0, 100, 10, 10);
//    totaluseLabel->setFixedSize(20, 20);
//    mainLayout->addSpacing(40);
//    mainLayout->addWidget(totaluseLabel, 0, Qt::AlignLeft);
//    mainLayout->addStretch();




}
void InfoShowWidget::slotShowDiskInfo(QString diskname, QString diskformat, QString diskmemory)
{
//    nameLabel->setText(diskname);
//    QString format = "格式: " + diskformat;
//    typeLabel->setText(format);
//    allmemoryLabel->setText(diskmemory);

    nameLabel->setText("Boot");
    QString format = "格式: EXT3";
    typeLabel->setText(format);
    allmemoryLabel->setText("256GB");

}

void InfoShowWidget::slotCurSelectChanged(const QString &devicepath, const QString &partitionpath, Sector start, Sector end)
{
    qDebug() << __FUNCTION__ << "-0--0-";
    DeviceInfoMap infomap = DMDbusHandler::instance()->probDeviceInfo();

}
