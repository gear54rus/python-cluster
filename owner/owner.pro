#-------------------------------------------------
#
# Project created by QtCreator 2014-09-25T18:45:03
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = owner
TEMPLATE = app


SOURCES += \
    ui/mainwindow.cpp \
    ui/listenwindow.cpp \
    core/core.cpp \
    core/task.cpp \
    core/event.cpp \
    network/node.cpp \
    ui/assignwindow.cpp \
    main.cpp

HEADERS  += ui/mainwindow.h \
    ui/listenwindow.h \
    core/core.h \
    core/task.h \
    core/event.h \
    network/node.h \
    global.h \
    ui/assignwindow.h

FORMS    += \
    ui/listenwindow.ui \
    ui/mainwindow.ui \
    ui/assignwindow.ui

CONFIG += c++11

RESOURCES += \
    resources/resources.qrc

RC_FILE = resources/icon.rc
