cmake_minimum_required (VERSION 2.6)
PROJECT(Mgr)

INCLUDE(../CMakeLists.common)
SET(COMMON_SOURCES
  OpenCLManager.cpp 
  ProcessingImage.cpp
  ProcessingImage3d.cpp
  Image3d.cpp 
  Logger.cpp
  csvFileUtils.cpp
  CpuSkeletonize.cpp
)
SET(SOURCES
  GUI/mainwindow.cpp
  GUI/cvImageWindow.cpp
  GUI/ApplicationManagerGUI.cpp
  GUI/vtkview.cpp
  GUI/VTKData.cpp
  Application.cpp
  ApplicationManager.cpp
  ContinuousProcessingMananger.cpp
  ImageSource/FileVideo.cpp
  ImageSource/Directory.cpp
  ImageSource/Camera.cpp
  Processing3dImage.cpp
  Normalization.cpp
)

SET(HEADERS_Qt

  GUI/mainwindow.h
  GUI/cvImageWindow.h
  GUI/ApplicationManagerGUI.h
  GUI/vtkview.h
  ContinuousProcessingMananger.h
)
SET(FORMS GUI/mainwindow.ui)
SET(FORMS_HEADERS GUI/mainwindow.h) 
INCLUDE(${QT_USE_FILE})
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS_Qt})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})

ADD_LIBRARY(COMMON_LIB ${COMMON_SOURCES})
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${UIFILES} ${HEADERS_MOC} ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} COMMON_LIB)
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_CALIB3D_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_CORE_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_IMGPROC_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_HIGHGUI_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCV_VIDEO_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${OPENCL_LIB})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTOPENGL_LIBRARY})
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${VTK_LIBRARIES}) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -std=c++14 -fPIC" )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(KERNELS_DIR "${PROJECT_SOURCE_DIR}/../Kernels/")
configure_file (
  "Paths.h.in"
  "${PROJECT_SOURCE_DIR}/include/Paths.h"
  )
include_directories("${PROJECT_BINARY_DIR}/include")
