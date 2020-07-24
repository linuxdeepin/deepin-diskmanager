#ifndef DISKSTANDARDITEMMODEL_H
#define DISKSTANDARDITEMMODEL_H

#include <QObject>
#include <QStandardItemModel>

class DiskStandardItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    DiskStandardItemModel();
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:

public slots:
};

#endif // DISKSTANDARDITEMMODEL_H
