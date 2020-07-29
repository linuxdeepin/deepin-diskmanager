#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <DDialog>
#include <DProgressBar>

DWIDGET_USE_NAMESPACE

class MessageBox : public DDialog
{
    Q_OBJECT
public:
    explicit MessageBox(DDialog *parent = nullptr);

    /**
     * @brief 设置警告窗口
     * @param title 标题，没有标题写空字符串
     * @param warningMsg 文本内容
     * @param sureBtnText 确定按钮
     * @param cancalBtnText="" 取消按钮，可不填
    */
    void setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, const QString &cancalBtnText = "");

    /**
     * @brief 设置警告窗口
     * @param title 标题，没有标题写空字符串
     * @param warningMsg 文本内容
     * @param sureBtnText 确定按钮
     * @param sureBtnType=ButtonNormal 确定按钮类型
     * @param cancalBtnText="" 取消按钮，可不填
    */
    void setWarings(const QString &title, const QString &warningMsg, const QString &sureBtnText, ButtonType sureBtnType, const QString &cancalBtnText = "");

    /**
     * @brief 设置进度窗口
     * @param title 标题，没有标题写空字符串
     * @param cancalBtnText 取消按钮
    */
    void setProgressBar(const QString &title, const QString &cancalBtnText);

    /**
     * @brief 设置label
     * @param text label内容
    */
    void addLabel(QString text);

    /**
     * @brief 设置进度条
    */
    void addProgressBar();

signals:

public slots:

private:
    DProgressBar *m_progressBar;
};

#endif // MESSAGEBOX_H
