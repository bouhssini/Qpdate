#-------------------------------------------------
#
# Project created by QtCreator 2013-05-02T22:57:38
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ../bin

TARGET = Qpdate
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

TRANSLATIONS = arabic.ts french.ts english.ts

RESOURCES += res.qrc
win32:RC_FILE = MyApp.rc

