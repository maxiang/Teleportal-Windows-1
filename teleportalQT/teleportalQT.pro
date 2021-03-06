QT       += core gui quickwidgets qml network positioning gamepad multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG +=  c++17
#subdir
INCLUDEPATH += $$PWD/src/inc
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp\
    src/pingsensor.cpp \
    src/qFlightInstruments.cpp\
    src/VideoReceiver.cpp \
    src/securefileuploader.cpp

HEADERS += \
    src/inc/mainwindow.h\
    src/inc/VideoReceiver.h\
    src/inc/qFlightInstruments.h \
    src/inc/pingsensor.h\
    src/inc/securefileuploader.h

FORMS += \
    ui/mainwindow.ui

TRANSLATIONS += \
    translation/en_AU.ts
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    qrc/assets.qrc

GstreamerDir=$$(GSTREAMER_1_0_ROOT_X86_64)

INCLUDEPATH += $${GstreamerDir}/include/glib-2.0
DEPENDPATH += $${GstreamerDir}/include/glib-2.0
INCLUDEPATH += $${GstreamerDir}/include/gstreamer-1.0
DEPENDPATH += $${GstreamerDir}/include/gstreamer-1.0

LIBS += $${GstreamerDir}/lib/glib-2.0.lib
LIBS += $${GstreamerDir}/lib/gstreamer-1.0.lib
LIBS += $${GstreamerDir}/lib/gstvideo-1.0.lib
LIBS += $${GstreamerDir}/lib/gobject-2.0.lib
LIBS += $${GstreamerDir}/lib/gthread-2.0.lib
LIBS += $${GstreamerDir}/lib/gio-2.0.lib
LIBS += $${GstreamerDir}/lib/intl.lib

INCLUDEPATH += $$PWD/dependencies/inc
DEPENDPATH  += $$PWD/dependencies/inc
#LIBS += $$PWD/dependencies/lib/libardusub.dll.a
include($$PWD/dependencies/ping-cpp.pri)
include($$PWD/dependencies/link.pri)
include($$PWD/dependencies/ardusub_api.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/dependencies/qssh/lib/ -lQSsh
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/dependencies/qssh/lib/ -lQSshd
else:unix: LIBS += -L$$PWD/dependencies/qssh/lib/ -lQSsh

INCLUDEPATH += $$PWD/dependencies/qssh/include
DEPENDPATH += $$PWD/dependencies/qssh/include

#windbg crash dump
QMAKE_CXXFLAGS_RELEASE += -g
QMAKE_CFLAGS_RELEASE += -g
QMAKE_LFLAGS_RELEASE = -mthreads
LIBS    += -lDbgHelp