#-------------------------------------------------
#
# Project created by QtCreator 2014-03-23T17:47:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        ../src/OpenCLManager.cpp

HEADERS  += mainwindow.h\
        ../src/OpenCLManager.h

FORMS    += mainwindow.ui
CONFIG += c++11

LIBS += -L$$PWD/../../../../usr/lib/ -lOpenCL

INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include
