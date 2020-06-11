/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     zhangkai <zhangkai@uniontech.com>
* Maintainer: zhangkai <zhangkai@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PARTITIONWIDGET_H
#define PARTITIONWIDGET_H

#include "partitioninfo.h"
#include "utils.h"
#include "partchartshowing.h"
#include "partedproxy/dmdbushandler.h"
#include <DDialog>
#include <DLabel>
#include <DHorizontalLine>
#include <DMessageManager>
#include <DSlider>
#include <DLineEdit>
#include <DComboBox>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
#include <DPushButton>
#include <DFontSizeManager>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractButton>
#include <DIconButton>
#include <QSizePolicy>
#include <QToolTip>

DWIDGET_USE_NAMESPACE

typedef struct STRUCTPART {
    STRUCTPART() {}
    double size;
    Sector count;
    QString name;
    QString fstype;
} stPart;

class PartitionWidget : public DDialog
{
    Q_OBJECT

public:
    explicit PartitionWidget(QWidget *parent = nullptr);
    ~PartitionWidget() override;
    void initUi();
    void topFrameSetting();
    void midFrameSetting();
    void botFrameSetting();
    void partInfoShowing();
    void getPartitionInfo();
    void setEnable();
    void setUseEnable();
    void setEnable2();
    void setLabelColor();
    void setLabelColorGray();
    void initConnection();
    double leaveSpace();
    void setSelectValue();
    bool blockSignals(bool block);

private:
    bool max_amount_prim_reached();

signals:
    void senderData(int x, int y, int z);
public slots:
    void slotSliderValueChanged(int value);
    void slotSetSliderValue();
    void slotSetPartName();
    void addPartitionSlot();
    void remPartitionSlot();
    void applyBtnSlot();
    void revertBtnSlot();
    void cancelBtnSlot();
    void showSelectPathInfo(const int &flag, const int &num, const int &posX);
    void showTip(const int &hover, const int &num, const int &posX);
    void comboxCurTextSlot();
    void judgeLastPartitionSlot();

private:
    QWidget *mainFrame;
    DFrame *topFrame;
    DFrame *midFrame;
    DFrame *botFrame;
    //    DFrame *parInfoFrame;
    DLabel *partInfoLabel;
    DLabel *partDoLabel;
    DLabel *allMemory;
    DLabel *deviceFormate;
    DLabel *deviceName;
    DLabel *selectedPartition;
    DLabel *partNameLabel;
    DIconButton *addButton;
    DIconButton *remButton;
    //    DScrollArea *scrollArea;
    QWidget *partWidget;
    DPushButton *applyBtn;
    DPushButton *cancleBtn;
    DPushButton *reveBtn;
    PartChartShowing *partChartWidget;
    DLineEdit *partNameEdit;
    DComboBox *partFormateCombox;
    DLineEdit *partSizeEdit;
    DComboBox *partComCobox;
    DSlider *hSlider;
    QString partSize;
    QString selectedpartName;

    int number = -1;
    double total = 0.00;
    double mTotal = 0.00;
    int mflag = -1;
    double currentSize = 0.00;
    int GM = 0;
    int m_value = 0;
    int block = 0;
    QVector<double> sizeInfo;
    QVector<QString> partName;
    QVector<stPart> m_patrinfo;

};

#endif // PARTITIONWIDGET_H
