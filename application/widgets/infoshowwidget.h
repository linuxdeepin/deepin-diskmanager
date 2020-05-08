#ifndef INFOSHOWWIDGET_H
#define INFOSHOWWIDGET_H
#include <DWidget>

DWIDGET_USE_NAMESPACE
class InfoShowWidget: public DWidget
{
    Q_OBJECT
public:
    explicit InfoShowWidget(DWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void initUi();


};

#endif // INFOSHOWWIDGET_H
