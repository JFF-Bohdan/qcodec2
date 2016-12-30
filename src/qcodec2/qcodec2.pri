#-------------------------------------------------
# Wrappers for codec2 by Bohdan Danishevsky (2017)
#-------------------------------------------------


QT *= core \
    multimedia

DEPENDPATH *= \
    $$PWD

INCLUDEPATH *= \
    $$PWD

HEADERS  *= \
    $$PWD/qcodec2shared.h \
    $$PWD/qcodec2wrappersbase.h \
    $$PWD/qcodec2wrappers.h \
    $$PWD/qcodec2decoder.h \
    $$PWD/qcodec2encoder.h

SOURCES *= \
    $$PWD/qcodec2shared.cpp \
    $$PWD/qcodec2wrappersbase.cpp \
    $$PWD/qcodec2wrappers.cpp \
    $$PWD/qcodec2decoder.cpp \
    $$PWD/qcodec2encoder.cpp

win32 {
    LIBS *= -llibcodec2
}

unix {
    LIBS *= -lcodec2
}
