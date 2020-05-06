#ifndef CENTERWIDGET_H
#define CENTERWIDGET_H

#include <DWidget>


class DMDbusHandler;

class CenterWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit CenterWidget(QWidget *parent = nullptr);

public:
    void HandleQuit();

signals:

public slots:

private:
    DMDbusHandler  *m_handler;
};

#endif // CENTERWIDGET_H
