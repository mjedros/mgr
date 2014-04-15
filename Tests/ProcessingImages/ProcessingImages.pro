#-------------------------------------------------
#
# Project created by QtCreator 2014-04-14T23:58:37
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_processingimagestest
CONFIG   += console
CONFIG   += c++0x
QMAKE_CXXFLAGS += -std=c++0x
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_processingimagestest.cpp\
../../src/OpenCLManager.cpp\
../../src/ProcessingImage.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
unix {
    LIBS += -L/usr/lib/ -lOpenCL -lopencv_highgui -lopencv_core -lopencv_imgproc
    INCLUDEPATH += /usr/include
    DEPENDPATH += /usr/include
}
win32 {
    INCLUDEPATH += "$$(CUDA_PATH)"/include
    DEPENDPATH += "$$(CUDA_PATH)"/lib/Win32

    LIBS += -L"$$(CUDA_PATH)"/lib/Win32 -lOpenCL
    LIBS += -L$$(OPENCV_DIR)/build/x86/mingw/bin -llibopencv_core248 -llibopencv_highgui248 -llibopencv_imgproc248

    INCLUDEPATH += $(OPENCV_DIR)/build/include
}
