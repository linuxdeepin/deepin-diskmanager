#ifndef CUSAPPLICATION_H
#define CUSAPPLICATION_H

#include <DApplication>
DWIDGET_USE_NAMESPACE

class CusApplication : public DApplication
{
    Q_OBJECT
public:
    CusApplication(int &argc, char **argv);

protected:
    void handleQuitAction() override;

signals:
    void sighandleQuitAction();
};

#endif // CUSAPPLICATION_H
