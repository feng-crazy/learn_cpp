############################################################################
#	  COPYRIGHT: 2017 lingtrack Instruments, Inc. All rights reserved
#	PROJECTNAME: Mx
#		VERSION:
#	   FILENAME: CMakeLists.txt
#		CREATED: 2017.12.12
#		 AUTHOR: hedengfeng
#	DESCRIPTION:
############################################################################

# 源文件列表
set(SRCS
	SpiDevice.cpp
	SerialDev.cpp
	UdpDevice.cpp
)

#include_directories(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR}/../../LTCommon/LTCommon/Include
  ${CROSS_COMPILE_DIR}/usr/include
  ${CROSS_COMPILE_DIR}/usr/include/arm-linux-gnueabihf
)


ADD_DEFINITIONS(-std=c++11 -Wno-deprecated-declarations -fpermissive -O2)  

link_directories(${PROJECT_BINARY_DIR}/lib)
link_directories(${CROSS_COMPILE_DIR}/usr/lib)

add_library(Device STATIC ${SRCS})

#target_link_libraries(Device)
