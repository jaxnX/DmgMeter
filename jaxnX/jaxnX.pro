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
    configurator.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    imagereader.h \
    imageattributes.h \
    charactergrid.h \
    characterpattern.h \
    screenrecorder.h \
    dmgmeter.h \
    configurator.h \
    settings.h

FORMS    += mainwindow.ui \
    configurator.ui

RESOURCES += \
    Icons/Icons.qrc

RC_FILE = jaxnX.rc
