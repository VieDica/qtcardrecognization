#-------------------------------------------------
#
# Project created by QtCreator 2016-07-29T09:22:21
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CServer
TEMPLATE = app


SOURCES += main.cpp\
        cserver.cpp \
    cthread.cpp \
    ctcpserver.cpp \
    processor.cpp \
    usermanage.cpp \
    mysqlmodel.cpp

HEADERS  += cserver.h \
    cthread.h \
    ctcpserver.h \
    processor.h \
    usermanage.h \
    mysqlmodel.h

FORMS    += cserver.ui \
    usermanage.ui

RESOURCES += \
    qrc.qrc

DISTFILES +=

