#ifndef DMFRAMEWIDGET_H
#define DMFRAMEWIDGET_H

#include <QWidget>
#include <DFrame>
#include <QPainter>
#include <DPalette>
#include <DApplicationHelper>
#include <DFontSizeManager>
#include "dmtreeviewdelegate.h"
#include <QTextOption>
DWIDGET_USE_NAMESPACE


class DmFrameWidget : public DFrame
{
    Q_OBJECT
public:
    explicit DmFrameWidget(DiskInfoData data, QWidget *parent = nullptr);

signals:

public:
    void setFrameData();

public slots:
    void slothandleChangeTheme();
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event)override;
private:
    DiskInfoData m_infodata;
    DPalette m_parentPb;
    int d_width = 0;
    int w = 0;
};

#endif // DMFRAMEWIDGET_H
