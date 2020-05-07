#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

DWIDGET_USE_NAMESPACE

class CenterWidget;
class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initUi();

signals:

public slots:
    void slotHandleQuitAction();
private:
    CenterWidget *m_central;

};

#endif // MAINWINDOW_H
