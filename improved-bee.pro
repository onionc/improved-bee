#-------------------------------------------------
#
# Project created by QtCreator 2022-12-06T09:30:06
#
#-------------------------------------------------


QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = improved-bee
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    datatype.cpp \
    util.cpp \
    XString.cpp \
    serialPort/serialport.cpp \
    serialPort/serialcombox.cpp \
    ui/comboboxdelegate.cpp \
    ui/tablewidgetdrag.cpp \
    ui/ledlabel.cpp \
    parse.cpp \
    luaScript/LuaScript.cpp

HEADERS += \
        mainwindow.h \
    datatype.h \
    util.h \
    XString.h \
    serialPort/serialport.h \
    serialPort/serialcombox.h \
    ui/comboboxdelegate.h \
    ui/tablewidgetdrag.h \
    ui/ledlabel.h \
    parse.h \
    luaScript/LuaScript.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc

INCLUDEPATH += include


LIBS += ./include/lua/liblualib.a



#debug
QMAKE_CXXFLAGS_DEBUG += -gstabs+
