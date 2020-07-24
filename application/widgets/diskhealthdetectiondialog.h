#ifndef DISKHEALTHDETECTIONDIALOG_H
#define DISKHEALTHDETECTIONDIALOG_H

#include <DDialog>
#include <DTableView>
#include <DCommandLinkButton>
#include <DLabel>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class QStandardItemModel;
class DiskStandardItemModel;
class DiskHealthDetectionDelegate;

class DiskHealthDetectionDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DiskHealthDetectionDialog(const QString &devicePath, QWidget *parent = nullptr);

signals:

public slots:

private slots:
    /**
     * @brief 导出按钮点击响应的槽函数
     */
    void onExportButtonClicked();

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
    DTableView *m_tableView;
    QStandardItemModel *m_standardItemModel;
    DCommandLinkButton *m_linkButton;
    DiskHealthDetectionDelegate *m_diskHealthDetectionDelegate;
    QString m_devicePath; // 磁盘路径
    DLabel *m_serialNumberValue; // 序列号值
    DLabel *m_userCapacityValue;// 用户容量值
    DLabel *m_healthStateValue;
    DLabel *m_temperatureValue;

};

#endif // DISKHEALTHDETECTIONDIALOG_H
