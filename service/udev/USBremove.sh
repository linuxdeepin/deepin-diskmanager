#!/bin/bash

PROC_NAME='deepin-diskmanager'

ProcNumber=`pidof $PROC_NAME`
if [ $ProcNumber ];then
   /usr/bin/dbus-send --system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.updateUsbRemove
else
   exit		
fi

