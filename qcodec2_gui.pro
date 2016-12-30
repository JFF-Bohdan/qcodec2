#-------------------------------------------------
# qcodec2 GUI example by Bohdan Danishevsky (2017)
#-------------------------------------------------

QT *= core gui multimedia

DEFINES *= QT_NO_CAST_FROM_ASCII
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qcodec2
TEMPLATE = app
CONFIG *= c++11

DEPENDPATH *= \
    $$PWD/src/qcodec2

INCLUDEPATH *= \
    $$PWD/src/qcodec2

SOURCES *= \
    $$PWD/src/qcodec2_gui_main.cpp\
    $$PWD/src/gui/mainwindow.cpp

HEADERS  *= \
    $$PWD/src/gui/mainwindow.h

RESOURCES *= \
    $$PWD/res/qcodec2_gui.qrc

FORMS *= \
    $$PWD/src/gui/mainwindow.ui

#for application icon
win32 {
    RC_FILE = $$PWD/res/qcodec2_gui.rc
}

#including headers for codec2 library
win32 {
    INCLUDEPATH *= $$PWD/src/include
    LIBS += -L$$PWD/codec2-lib
}

include($$PWD/src/qcodec2/qcodec2.pri)

