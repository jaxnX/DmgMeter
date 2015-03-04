#-------------------------------------------------
#
# Project created by QtCreator 2015-02-21T02:45:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jaxnX
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagereader.cpp \
    imageattributes.cpp \
    charactergrid.cpp \
    characterpattern.cpp \
    screenrecorder.cpp \
    dmgmeter.cpp \
    screenrecorderthread.cpp

HEADERS  += mainwindow.h \
    imagereader.h \
    imageattributes.h \
    charactergrid.h \
    characterpattern.h \
    screenrecorder.h \
    dmgmeter.h \
    screenrecorderthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    Icons/Icons.qrc

RC_FILE = jaxnX.rc
