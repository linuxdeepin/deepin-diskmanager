#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <DWidget>
#include "partedproxy/dmdbushandler.h"
#include "devicelistwidget.h"
DWIDGET_USE_NAMESPACE

class TitleWidget;
class DMDbusHandler;
class MainSplitter;
class BufferWin;

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
    void initConnection();


signals:

public slots:
    void slotshowSpinerWindow(bool bshow = false);
private:
    DMDbusHandler  *m_handler;
    TitleWidget     *m_titlewidget;
    MainSplitter    *m_mainspliter;
    BufferWin       *m_bufferwin;
};

#endif // CENTERWIDGET_H
