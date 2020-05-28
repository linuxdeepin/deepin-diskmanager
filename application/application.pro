QT += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-diskmanager
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

LIBS += -L$$OUT_PWD/../basestruct/ -lbasestruct

INCLUDEPATH += $$PWD/../basestruct
DEPENDPATH += $$PWD/../basestruct

include (partedproxy/partedproxy.pri)
include (widgets/widgets.pri)
include (utils/utils.pri)

SOURCES += \
        main.cpp \
    cusapplication.cpp


TRANSLATIONS += \
    translations/deepin-diskmanager_en_US.ts\
    translations/deepin-diskmanager_zh_CN.ts

APPICONDIR = $$PREFIX/share/icons/deepin/apps/scalable

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-diskmanager

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/icons/deepin-diskmanager.desktop

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/icons/deepin/builtin/deepin-diskmanager.svg

CONFIG(release, debug|release) {
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    TRANSLATIONFILES= $$files($$PWD/translations/*.ts)
    for(tsfile, TRANSLATIONFILES) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
    #将qm文件添加到安装包
    dtk_translations.path = /usr/share/$$TARGET/translations
    dtk_translations.files = $$PWD/translations/*.qm
    INSTALLS += dtk_translations
}

INSTALLS += target desktop icon_files

RESOURCES += \
    resource.qrc \
    icons.qrc

HEADERS += \
    cusapplication.h

