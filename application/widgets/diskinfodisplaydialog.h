#ifndef DISKINFODISPLAYDIALOG_H
#define DISKINFODISPLAYDIALOG_H

#include <DDialog>
#include <DCommandLinkButton>

#include <QWidget>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DTK_USE_NAMESPACE

class DiskInfoDisplayDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DiskInfoDisplayDialog(const QString &devicePath, QWidget *parent = nullptr);

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
    QStringList m_diskInfoNameList;
    QStringList m_diskInfoValueList;
    DCommandLinkButton *m_linkButton;
    QString m_devicePath;

};

#endif // DISKINFODISPLAYDIALOG_H
