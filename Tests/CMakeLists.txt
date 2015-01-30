cmake_minimum_required (VERSION 2.6)
PROJECT(Mgr)
INCLUDE(../CMakeLists.common)
SET(TESTS
   ProcessingImages/tst_processingimagestest.cpp
   ../src/OpenCLManager.cpp
   ../src/ProcessingImage.cpp
)

find_package(Qt5Test)
set(CMAKE_AUTOMOC ON)
ADD_EXECUTABLE(Tests ${TESTS})
TARGET_LINK_LIBRARIES( Tests ${OPENCV_CALIB3D_LIB})
TARGET_LINK_LIBRARIES( Tests ${OPENCV_CORE_LIB})
TARGET_LINK_LIBRARIES( Tests ${OPENCV_IMGPROC_LIB})
TARGET_LINK_LIBRARIES( Tests ${OPENCV_HIGHGUI_LIB})
TARGET_LINK_LIBRARIES( Tests ${OPENCV_VIDEO_LIB})
TARGET_LINK_LIBRARIES( Tests ${OPENCL_LIB})
TARGET_LINK_LIBRARIES( Tests Qt5::Test )
include_directories(${Qt5Core_INCLUDE_DIRS})
if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -Wextra -std=c++11 -fPIC" )
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif(NOT MSVC)