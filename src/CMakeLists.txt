cmake_minimum_required (VERSION 2.6)
PROJECT(Mgr)

INCLUDE(../CMakeLists.common)
SET(SOURCES
  GUI/mainwindow.cpp
  GUI/cvImageWindow.cpp
  GUI/ApplicationManagerGUI.cpp
  Application.cpp
  ApplicationManager.cpp
  OpenCLManager.cpp 
  ProcessingImage.cpp
  ImageSource/FileVideo.cpp
  ImageSource/Directory.cpp
  Image3d.cpp 
  Normalization.cpp
)
SET (HEADERS
  ImageSource/SourceFactory.h  
  ImageSource/IImageSource.h
  ImageSource/FileVideo.h
  ImageSource/Directory.h
  OpenCLManager.h
  Image3d.h
  Normalization.h
)
SET(HEADERS_Qt

  GUI/mainwindow.h
  GUI/cvImageWindow.h
  GUI/ApplicationManagerGUI.h
)

QT5_WRAP_CPP(HEADERS_MOC ${HEADERS_Qt})
qt5_wrap_ui(UIFILES GUI/mainwindow.ui)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${UIFILES} ${HEADERS_MOC})

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_CALIB3D_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_CORE_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_IMGPROC_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_HIGHGUI_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_VIDEO_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCL_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} Qt5::Widgets )
if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -std=c++11 -fPIC" )
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif(NOT MSVC)
set(KERNELS_DIR "${PROJECT_SOURCE_DIR}/../Kernels/")
configure_file (
  "Paths.h.in"
  "${PROJECT_BINARY_DIR}/include/Paths.h"
  )
include_directories("${PROJECT_BINARY_DIR}/include")