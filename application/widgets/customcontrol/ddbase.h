#ifndef DDBASE_H
#define DDBASE_H
#include <DDialog>
#include <DFrame>
#include <QAbstractButton>
#include <QVBoxLayout>
DWIDGET_USE_NAMESPACE

class DDBase : public DDialog
{
    Q_OBJECT
public:
    explicit DDBase(QWidget *parent = nullptr);

    DFrame *mainFrame = nullptr;
    int okcode = -1;
};

#endif // DDBASE_H
