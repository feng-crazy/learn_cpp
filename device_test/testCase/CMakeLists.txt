############################################################################
#	  COPYRIGHT: 2017 lingtrack Instruments, Inc. All rights reserved
#	PROJECTNAME: Mx
#		VERSION:
#	   FILENAME: CMakeLists.txt
#		CREATED: 2017.12.12
#		 AUTHOR: hedengfeng
#	DESCRIPTION:
############################################################################



set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


INCLUDE_DIRECTORIES(../../LTCommon/LTCommon/Include)

include_directories(${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/Device)

include_directories(${PROJECT_SOURCE_DIR})

link_directories(${PROJECT_BINARY_DIR}/lib)


LINK_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/../../LTCommon/LTCommon/Lib
  ${CROSS_COMPILE_DIR}/usr/local/lib
)


#link_directories("/usr/local/arm/lib")

link_directories("${PROJECT_SOURCE_DIR}/../../LTCommon/LTCommon/Lib")


##############################################################

SET(SRCS
spi_test.cpp
)

ADD_EXECUTABLE(spi_test  ${SRCS})

add_dependencies(spi_test Device)

# 交叉编译时, 所有库使用动态链接，使用静态链接会报错，原因未知
set_target_properties(spi_test PROPERTIES LINK_FLAGS "-rdynamic")

set_target_properties(spi_test PROPERTIES COMPILE_FLAGS "-fstack-check")

target_link_libraries(spi_test  -Wl,-Bdynamic -lpthread -ldl)

target_link_libraries(spi_test  zmq)

target_link_libraries(spi_test Device)


##############################################################

ADD_EXECUTABLE(spi_stl_test  spi_stl_test.cpp)

add_dependencies(spi_stl_test Device)

# 交叉编译时, 所有库使用动态链接，使用静态链接会报错，原因未知
set_target_properties(spi_stl_test PROPERTIES LINK_FLAGS "-rdynamic")

set_target_properties(spi_stl_test PROPERTIES COMPILE_FLAGS "-fstack-check")

target_link_libraries(spi_stl_test  -Wl,-Bdynamic -lpthread -ldl)

target_link_libraries(spi_stl_test  zmq)

target_link_libraries(spi_stl_test Device )

##############################################################

ADD_EXECUTABLE(check_data  check_data.cpp)

add_dependencies(check_data Device)

# 交叉编译时, 所有库使用动态链接，使用静态链接会报错，原因未知
set_target_properties(check_data PROPERTIES LINK_FLAGS "-rdynamic")

set_target_properties(check_data PROPERTIES COMPILE_FLAGS "-fstack-check")

target_link_libraries(check_data  -Wl,-Bdynamic -lpthread -ldl)

target_link_libraries(check_data  zmq)

target_link_libraries(check_data Device )


ADD_EXECUTABLE(spd_test  spd_test.cpp)
target_link_libraries(spd_test  -Wl,-Bdynamic -lpthread -ldl)

ADD_EXECUTABLE(zeromq_client zeromq_client.cpp)
target_link_libraries(zeromq_client  zmq)

ADD_EXECUTABLE(zeromq_server zeromq_server.cpp)
target_link_libraries(zeromq_server  zmq)


##############################################################

ADD_EXECUTABLE(SerialDev_test  SerialDev_test.cpp)

add_dependencies(SerialDev_test Device)

target_link_libraries(SerialDev_test Device )

target_link_libraries(SerialDev_test  wiringPi)


##############################################################

ADD_EXECUTABLE(UdpDevice_test  UdpDevice_test.cpp)

add_dependencies(UdpDevice_test Device)

target_link_libraries(UdpDevice_test Device )


INSTALL(TARGETS spi_test spi_stl_test SerialDev_test UdpDevice_test
RUNTIME DESTINATION Bin/test
)
