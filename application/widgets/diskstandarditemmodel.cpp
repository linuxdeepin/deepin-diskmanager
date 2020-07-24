#include "diskstandarditemmodel.h"

#include <QDebug>

DiskStandardItemModel::DiskStandardItemModel()
{

}

QVariant DiskStandardItemModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "++++++++++++++++++++++++++" << role << index.row() << index.column();
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (role == Qt::DisplayRole)
    {
        return QStandardItemModel::data(index, role);
    }

    if(role == Qt::BackgroundColorRole )
    {
//        return QBrush(Qt::green);
        if(index.row() % 2 == 0)
        {qDebug() << index << index.row() % 2 << role << "BackgroundColorRole";
            return QColor("red");
        }
        else
        {qDebug() << index << index.row() % 2 << "++++++++++++++++++++++++++" << role << "BackgroundColorRole";
            return QColor("green");
        }
    }

    return QVariant();
}

