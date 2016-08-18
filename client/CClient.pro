#-------------------------------------------------
#
# Project created by QtCreator 2016-07-26T12:03:36
#
#-------------------------------------------------

QT       += core gui network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CClient
TEMPLATE = app


SOURCES += main.cpp\
        client.cpp \
    dialog.cpp

HEADERS  += client.h \
    dialog.h

FORMS    += client.ui \
    dialog.ui

RESOURCES += \
    qrc.qrc
