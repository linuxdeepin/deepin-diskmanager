#!/bin/bash

PROC_NAME='deepin-diskmanager'

#ProcNumber=`pidof $PROC_NAME`
ProcNumber=`ps aux |grep deepin-diskmanager-service |sed '/grep/d' |sed '/tail/d'`
if [ -n "$ProcNumber" ];then
   /usr/bin/dbus-send --system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.updateUsbRemove
else
   exit		
fi

