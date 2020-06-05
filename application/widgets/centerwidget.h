#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include "partedproxy/dmdbushandler.h"
#include "devicelistwidget.h"
#include <DWidget>

DWIDGET_USE_NAMESPACE

class TitleWidget;
class MainSplitter;

class CenterWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(DWidget *parent = nullptr);
    ~CenterWidget();

public:
    void HandleQuit();
    TitleWidget *titlewidget();

private:
    void initUi();

private:
    TitleWidget *m_titlewidget;
    MainSplitter *m_mainspliter;
};

#endif // CENTERWIDGET_H
