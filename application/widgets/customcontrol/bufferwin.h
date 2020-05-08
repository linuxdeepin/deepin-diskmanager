#ifndef BUFFERWIN_H
#define BUFFERWIN_H
#include <DSpinner>
#include <DWidget>
DWIDGET_USE_NAMESPACE

class BufferWin : public DWidget
{
    Q_OBJECT
public:
    explicit BufferWin(DWidget *parent = nullptr);
    void Start();
    void Stop();

private:
    void initUi();

private:
    DSpinner *m_spinner = nullptr;
};

#endif // BUFFERWIN_H
