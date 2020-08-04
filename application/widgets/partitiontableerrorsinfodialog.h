#ifndef PARTITIONTABLEERRORSINFODIALOG_H
#define PARTITIONTABLEERRORSINFODIALOG_H

#include <DDialog>
#include <DLabel>
#include <DTableView>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class QStandardItemModel;
class PartitionTableErrorsInfoDelegate;

class PartitionTableErrorsInfoDialog : public DDialog
{
    Q_OBJECT
public:
    explicit PartitionTableErrorsInfoDialog(const QString &deviceInfo, QWidget *parent = nullptr);

signals:

public slots:

private:
    /**
     * @brief 初始化界面
     */
    void initUI();

    /**
     * @brief 初始化信号连接
     */
    void initConnections();

private:
    DLabel *m_Label;
    QString m_deviceInfo;
    DTableView *m_tableView;
    QStandardItemModel *m_standardItemModel;
    PartitionTableErrorsInfoDelegate *m_partitionTableErrorsInfoDelegatee;
    DPushButton *pushButton;
};

#endif // PARTITIONTABLEERRORSINFODIALOG_H
