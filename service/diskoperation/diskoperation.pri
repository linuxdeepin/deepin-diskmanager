include(filesystems/filesystems.pri)

HEADERS += \
    $$PWD/partedcore.h \
    $$PWD/blockspecial.h \
    $$PWD/partition.h\
     $$PWD/device.h \
    $$PWD/fsinfo.h \
    $$PWD/procpartitionsinfo.h \
    $$PWD/supportedfilesystems.h \
    $$PWD/mountinfo.h

SOURCES += \
    $$PWD/partedcore.cpp \
    $$PWD/blockspecial.cpp \
    $$PWD/partition.cpp\
    $$PWD/device.cpp \
    $$PWD/fsinfo.cpp \
    $$PWD/procpartitionsinfo.cpp \
    $$PWD/supportedfilesystems.cpp \
    $$PWD/mountinfo.cpp
