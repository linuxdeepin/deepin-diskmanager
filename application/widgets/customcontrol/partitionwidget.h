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

#include <DDialog>
#include <DLabel>
#include <DLineEdit>
#include <DComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DFrame>
#include <DPalette>
#include <DApplicationHelper>
#include <QAbstractButton>
#include <DIconButton>
#include <DPushButton>
#include <DScrollArea>
#include <QSizePolicy>
#include <DHorizontalLine>
#include <DMessageManager>
#include <DSlider>
#include "partitioninfo.h"
#include "utils.h"
#include "partchartshowing.h"
#include "partedproxy/dmdbushandler.h"

DWIDGET_USE_NAMESPACE



class PartitionWidget : public DDialog
{
    Q_OBJECT
public:
    explicit PartitionWidget(QWidget *parent = nullptr);
    void initUi();
//    void initConnection();
    void topFrameSetting();
    void midFrameSetting();
    void botFrameSetting();
    void partInfoShowing();
    void partedInfo();
    void getPartitionInfo(const PartitionInfo &data, const QString &disksize);
    void initTopFrameData();
    void initConnection();


signals:
    void senderData(int x, int y, int z);
    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
public slots:
    void slotSliderValueChanged(int value);
    void slotSetSliderValue();
    void addPartitionSlot();
    void remPartitionSlot();
    void applyBtnSlot();
    void revertBtnSlot();
    void cancelBtnSlot();
private:
    QWidget *mainFrame;
    DFrame *topFrame;
    DFrame *midFrame;
    DFrame *botFrame;
//    DFrame *parInfoFrame;
    DLabel *deviceInfoLabel;
    DLabel *deviceNameLabel;
    DLabel *deviceName;
    DLabel *deviceFormateLabel;
    DLabel *deviceFormate;
    DLabel *selectedPartLabel;
    DLabel *selectedPartition;
    DLabel *allMemoryLabel;
    DLabel *allMemory;
    DLabel *partInfoLabel;
    DIconButton *addButton;
    DIconButton *remButton;
//    DScrollArea *scrollArea;
    QWidget *partWidget;


    DLabel *picLabel;
    DLabel *titleLabel;
    DLabel *tipLabel;

    DPushButton *applyBtn;
    DPushButton *cancleBtn;
    DPushButton *reveBtn;

    PartChartShowing *partChartWidget;

    DLabel *partNameLabel;
    DLineEdit *partNameEdit;
    DLabel *partFormateLabel;
    DComboBox *partFormateCombox;
    DLabel *partSizeLabel;
    DLineEdit *partSizeEdit;
    DComboBox *partComCobox;
    DLabel *partDoLabel;
    DSlider *hSlider;

    QString partUsed;
    QString partUnused;
    QString devicePath;
    QString deviceSize;
    QString partPath;
    QString partSize;
    QString partFstype;


    int count = 0;


//    QVector<QString> partPaths;
//    QHash<int, QVector<double>> sizeinfos;
    QVector<double> sizeInfo;
    QVector<QString> partName;

};

#endif // PARTITIONWIDGET_H
