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
    ../src/ProcessingImage.cpp \
    ../src/FileVideo.cpp

HEADERS  += mainwindow.h\
        ../src/OpenCLManager.h \
    ../src/ProcessingImage.h \
    ../src/FileVideo.h \
    ../src/SourceFactory.h \
    ../src/IImageSource.h

FORMS    += mainwindow.ui
CONFIG += c++0x
QMAKE_CXXFLAGS += -std=c++0x
unix:QMAKE_CXXFLAGS += -Werror

unix {
    LIBS += -L/usr/lib/ -lOpenCL -lopencv_highgui -lopencv_core -lopencv_imgproc
    INCLUDEPATH += /usr/include
    DEPENDPATH += /usr/include
}
win32 {
    INCLUDEPATH += "$$(CUDA_PATH)"/include
    DEPENDPATH += "$$(CUDA_PATH)"/lib/Win32

    LIBS += -L"$$(CUDA_PATH)"/lib/Win32 -lOpenCL
    LIBS += -L$$(OPENCV_DIR)/sources/mingw/bin -llibopencv_core248 -llibopencv_highgui248 -llibopencv_imgproc248

    INCLUDEPATH += $(OPENCV_DIR)/build/include
}

