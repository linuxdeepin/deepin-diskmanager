#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class MessageBox : public DDialog
{
    Q_OBJECT
public:
    explicit MessageBox(DDialog *parent = nullptr);

    /**
     * @brief 设置警告窗口
     * @param warningMsg 文本内容
     * @param surebtntext 确定按钮
     * @param cancalbtntext="" 取消按钮，可不填
    */
    void setWarings(QString warningMsg,QString surebtntext,QString cancalbtntext="");

    /**
     * @brief 设置label
     * @param text label内容
    */
    void addLabel(QString text);

signals:

public slots:
};

#endif // MESSAGEBOX_H
