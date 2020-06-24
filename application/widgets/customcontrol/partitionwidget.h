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
#include <DIconButton>
#include <QToolTip>
#include <QRegExp>
#include <DMessageManager>
#include <QRegExpValidator>
DWIDGET_USE_NAMESPACE

typedef struct STRUCTPART {
    STRUCTPART()
    {
        size = 0.0;
        count = 0;
        labelname = fstype = "";
        blast = false;
    }
    double size;
    Sector count;
    QString labelname;
    QString fstype;
    bool blast;
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
    void initConnection();
    void recPartitionInfo();
    double sumValue();  //已用分区大小的和
private:
    bool max_amount_prim_reached();
    void setEnable(const int &flag, const bool &isExceed);
    void setControlEnable(const bool &isTrue);
    void setLabelColor(const bool &isOk);//颜色
    void setRegValidator();//正则表达
    void setSelectUnallocatesSpace();//选中空闲分区的状态
    void setAddOrRemResult(const bool &isExceed);
signals:

public slots:
    void slotSliderValueChanged(int value);
    void slotSetSliderValue();
    void slotSetPartName(); //选个设置或修改分区名称
    void addPartitionSlot();//"+"新建分区
    void remPartitionSlot();//"-"删除分区
    void applyBtnSlot();
    void revertBtnSlot();
    void cancelBtnSlot();
    void showSelectPathInfo(const int &flag, const int &num, const int &posX);//点击显示tip
    void showTip(const int &hover, const int &num, const int &posX);//悬停显示tip
    void comboxCurTextSlot(int index); //下拉框单位切换
    void judgeLastPartitionSlot();

private:
    QWidget *mainFrame;
    DFrame *topFrame;
    DFrame *midFrame;
    DFrame *botFrame;
    DLabel *partInfoLabel;
    DLabel *partDoLabel;
    DLabel *allMemory;
    DLabel *deviceFormate;
    DLabel *deviceName;
    DLabel *selectedPartition;
    DLabel *partNameLabel;
    DIconButton *addButton;
    DIconButton *remButton;
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
    QString currentEditSize;
    int number = -1;
    double total = 0.00;
    double mTotal = 0.00;
    int mflag = -1;
    int m_value = 0;
    int block = 0;
    QVector<double> sizeInfo;
    QVector<QString> partName;
    QVector<stPart> m_patrinfo;
    bool isExc = true;
};

#endif // PARTITIONWIDGET_H
