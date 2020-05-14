#ifndef DMTABLEVIEW_H
#define DMTABLEVIEW_H

#include <QWidget>
#include <DTableView>
DWIDGET_USE_NAMESPACE
class DmTableView : public DTableView
{
    Q_OBJECT
public:
    explicit DmTableView(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // DMTABLEVIEW_H
