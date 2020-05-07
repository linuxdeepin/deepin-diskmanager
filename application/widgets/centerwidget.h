#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

class TitleWidget;
class DMDbusHandler;
class MainSplitter;

class CenterWidget : public DWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(DWidget *parent = nullptr);

public:
    void HandleQuit();
    TitleWidget *titlewidget();

signals:

public slots:

private:
    DMDbusHandler  *m_handler;
    TitleWidget     *m_titlewidget;
    MainSplitter    *m_mainspliter;
};

#endif // CENTERWIDGET_H
