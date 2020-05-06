#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>

class CenterWidget;
class MainWindow : public Dtk::Widget::DMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

signals:

public slots:

private:
    CenterWidget *m_central;

};

#endif // MAINWINDOW_H
