#!/bin/bash

PROC_NAME='deepin-diskmanager'

ProcNumber=`pidof $PROC_NAME`
if [ $ProcNumber > 0 ];then
   /usr/bin/dbus-send --system --type=method_call --dest=com.deepin.diskmanager /com/deepin/diskmanager com.deepin.diskmanager.updateUsb
else
   exit		
fi

