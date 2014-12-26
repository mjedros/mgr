#-------------------------------------------------
#
# Project created by QtCreator 2014-03-23T17:47:42
#
#-------------------------------------------------

TEMPLATE = app

QT      += core gui

contains(QT_VERSION, ^5\\.[0-8]\\..*) {
        message("* Using Qt $${QT_VERSION}.")
        QT += widgets
}


TARGET = Gui
TEMPLATE = app


SOURCES +=\
        ../src/GUI/mainwindow.cpp\
        ../src/OpenCLManager.cpp \
    ../src/Application.cpp \
    ../src/ProcessingImage.cpp \
    ../src/FileVideo.cpp \
    ../src/ApplicationManager.cpp \
    ../src/GUI/cvImageWindow.cpp \
    ../src/Image3d.cpp

HEADERS  += ../src/GUI/mainwindow.h\
        ../src/OpenCLManager.h \
    ../src/ProcessingImage.h \
    ../src/FileVideo.h \
    ../src/SourceFactory.h \
    ../src/IImageSource.h \
    ../src/ApplicationManager.h \
    ../src/GUI/cvImageWindow.h \
    ../src/Image3d.h

FORMS    += ../src/GUI/mainwindow.ui
CONFIG += c++0x
QMAKE_CXXFLAGS += -std=c++0x

unix {
    QMAKE_CXXFLAGS += -Werror
}
win32:QMAKE_CXXFLAGS += -fpermissive

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

