cmake_minimum_required (VERSION 2.6)
PROJECT(Mgr)

INCLUDE(../CMakeLists.common)
SET(COMMON_SOURCES
  OpenCLManager.cpp 
  ProcessingImage.cpp
  csvFileUtils.cpp
)
SET(SOURCES
  GUI/mainwindow.cpp
  GUI/cvImageWindow.cpp
  GUI/ApplicationManagerGUI.cpp
  GUI/vtkview.cpp
  GUI/VTKData.cpp
  Application.cpp
  ApplicationManager.cpp
  ImageSource/FileVideo.cpp
  ImageSource/Directory.cpp
  Image3d.cpp 
  Processing3dImage.cpp
  Normalization.cpp
)

SET(HEADERS_Qt

  GUI/mainwindow.h
  GUI/cvImageWindow.h
  GUI/ApplicationManagerGUI.h
  GUI/vtkview.h
)
SET(FORMS GUI/mainwindow.ui)
SET(FORMS_HEADERS GUI/mainwindow.h) 
INCLUDE(${QT_USE_FILE})
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS_Qt})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
ADD_LIBRARY(COMMON_LIB ${COMMON_SOURCES})
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${UIFILES} ${HEADERS_MOC} ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} COMMON_LIB)
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_CALIB3D_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_CORE_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_IMGPROC_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_HIGHGUI_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_VIDEO_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCL_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${VTK_LIBRARIES}) 

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
