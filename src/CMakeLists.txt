cmake_minimum_required (VERSION 2.6)
add_executable(Mgr 
  Application.cpp
  ApplicationManager.cpp
  OpenCLManager.h 
  OpenCLManager.cpp 
  ProcessingImage.cpp
  GUI/wxImagesProcessing.cpp
  IImageSource.h
  FileVideo.h
  FileVideo.cpp
  SourceFactory.h
)

find_library(OPENCL_LIB 
	NAMES OpenCL
	PATHS "$ENV{AMDAPPSDKROOT}/lib/x86" "$ENV{CUDA_PATH}/lib/Win32"
	)
target_link_libraries (Mgr ${OPENCL_LIB})
find_path(OPENCL_INC
	NAMES "CL/cl.hpp"
	PATHS "$ENV{AMDAPPSDKROOT}/include" "$ENV{CUDA_PATH}/include" "../include"
)
include_directories("${OPENCL_INC}")


find_path(OPENCV_INC
	 	NAMES "opencv2/opencv.hpp"
		PATHS "$ENV{OPENCV_DIR}/build/include"
		)
include_directories("${OPENCV_INC}")

find_library(OPENCV_CALIB3D_LIB
		NAMES opencv_calib3d248 opencv_calib3d
		PATHS "$ENV{OPENCV_DIR}/build/x86/vc12/lib"
		)
		
find_library(OPENCV_CORE_LIB
		NAMES opencv_core248d opencv_core
		PATHS "$ENV{OPENCV_DIR}/build/x86/vc12/lib"
		)
		
find_library(OPENCV_HIGHGUI_LIB
		NAMES opencv_highgui248d opencv_highgui
		PATHS "$ENV{OPENCV_DIR}/build/x86/vc12/lib"
		)
find_library(OPENCV_IMGPROC_LIB
		NAMES opencv_imgproc248d opencv_imgproc
		PATHS "$ENV{OPENCV_DIR}/build/x86/vc12/lib"
		)
find_library(OPENCV_VIDEO_LIB
		NAMES opencv_video248d opencv_video
		PATHS "$ENV{OPENCV_DIR}/build/x86/vc12/lib"
		)	

find_package(wxWidgets REQUIRED html adv core base net aui xrc qa richtext )
include(${wxWidgets_USE_FILE})

target_link_libraries*(Mgr ${wxWidgets_LIBRARIES})
target_link_libraries (Mgr ${OPENCV_CALIB3D_LIB})
target_link_libraries (Mgr ${OPENCV_CORE_LIB})
target_link_libraries (Mgr ${OPENCV_IMGPROC_LIB})
target_link_libraries (Mgr ${OPENCV_HIGHGUI_LIB})
target_link_libraries (Mgr ${OPENCV_VIDEO_LIB})
add_definitions(-DQT_NOT_DEFINED)

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11" )
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif(NOT MSVC)

