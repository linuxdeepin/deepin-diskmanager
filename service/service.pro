QT += core dbus
QT -= gui

TARGET = deepin-diskmanager-service

CONFIG += c++11 console link_pkgconfig
CONFIG -= app_bundle

PKGCONFIG += dtkcore

TEMPLATE = app

LIBS +=-lparted
LIBS += -L$$OUT_PWD/../basestruct/ -lbasestruct

INCLUDEPATH += $$PWD/../basestruct
DEPENDPATH += $$PWD/../basestruct

include(diskoperation/diskoperation.pri)


SOURCES += \
        main.cpp \
    diskmanagerservice.cpp

HEADERS += \
    diskmanagerservice.h

isEmpty(PREFIX):PREFIX = /usr

binary.path = $${PREFIX}/lib/deepin-daemon/
binary.files = $${OUT_PWD}/deepin-diskmanager-service

service.path = $${PREFIX}/share/dbus-1/system-services/
service.files = $$PWD/data/com.deepin.diskmanager.service

dbus.path = $${PREFIX}/share/dbus-1/system.d/
dbus.files = $$PWD/data/com.deepin.diskmanager.conf

INSTALLS += service dbus binary







