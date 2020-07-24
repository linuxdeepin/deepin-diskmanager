#ifndef DISKHEALTHDETECTIONDELEGATE_H
#define DISKHEALTHDETECTIONDELEGATE_H

#include <DDialog>
#include <QStyledItemDelegate>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class DiskHealthDetectionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DiskHealthDetectionDelegate(DDialog *dialog);

    /**
     * @brief 设置表格item字体颜色
     * @param color 颜色
     */
    void setTextColor(const QColor &color);

signals:

public slots:

protected:
    /**
     * @brief 重写绘画事件
     * @param painter 画笔
     * @param option 视图小部件中绘制项的参数
     * @param index 数据模型中的数据
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;      //绘制事件

private:
    DDialog *m_dialog;      //父类窗口指针
    QColor m_color;
};

#endif // DISKHEALTHDETECTIONDELEGATE_H
