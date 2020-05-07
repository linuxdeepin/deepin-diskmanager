#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <DWidget>
#include <DPushButton>
DWIDGET_USE_NAMESPACE


class TitleWidget: public DWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(DWidget *parent = nullptr);
    void initUi();
    DPushButton *createBtn(const QString &btnName, bool bCheckable = false);

private:
    DPushButton *m_btnparted;
    DPushButton *m_btnformat;
    DPushButton *m_btnmount;
    DPushButton *m_btnunmount;
    DPushButton *m_btnresize;
};

#endif // TITLEWIDGET_H
