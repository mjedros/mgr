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
    ../src/Application.cpp \
    ../src/ProcessingImage.cpp

HEADERS  += mainwindow.h\
        ../src/OpenCLManager.h \
    ../src/ProcessingImage.h

FORMS    += mainwindow.ui
CONFIG += c++11
QMAKE_CXXFLAGS += -Werror
LIBS += -L/usr/lib/ -lOpenCL -lopencv_highgui -lopencv_core -lopencv_imgproc
#-lopencv_calib3d
INCLUDEPATH += /usr/include
DEPENDPATH += /usr/include
