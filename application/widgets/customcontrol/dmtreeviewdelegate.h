#ifndef DMTREEVIEWDELEGATE_H
#define DMTREEVIEWDELEGATE_H

#include <QObject>
#include <QWidget>
#include <DStyledItemDelegate>
#include <QPainter>
#include <QRect>
#include <QAbstractItemView>
#include <QImage>
struct DiskInfoData {
    QString disklabel;
    QString disksize;
    QImage iconImage;
    QString partitonlabel;
    QString partitionsize;
    int level;
};
Q_DECLARE_METATYPE(DiskInfoData)
DWIDGET_USE_NAMESPACE
class DmTreeviewDelegate : public DStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DmTreeviewDelegate(QAbstractItemView *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
signals:
//    QAbstractItemView *m_parentView {nullptr};
//    DPalette m_parentPb;
public slots:
private:

};

#endif // DMTREEVIEWDELEGATE_H
