#-------------------------------------------------
#
# Project created by QtCreator 2014-03-23T17:47:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gui
TEMPLATE = app


SOURCES +=\
        mainwindow.cpp\
        ../src/OpenCLManager.cpp \
    ../src/Application.cpp

HEADERS  += mainwindow.h\
        ../src/OpenCLManager.h

FORMS    += mainwindow.ui
CONFIG += c++11
QMAKE_CXXFLAGS += -Werror
LIBS += -L/usr/lib/ -lOpenCL

INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include
