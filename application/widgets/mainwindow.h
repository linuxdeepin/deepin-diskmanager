#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DSpinner>
DWIDGET_USE_NAMESPACE

class CenterWidget;
class BufferWin;
class DMDbusHandler;
class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initUi();
    void initConnection();

signals:

public slots:
    void slotHandleQuitAction();
    void slotshowSpinerWindow(bool bshow = false);

private:
    CenterWidget *m_central;
    BufferWin *m_bufferwin;
    DMDbusHandler *m_handler;
    DSpinner *m_spinner;
};

#endif // MAINWINDOW_H
