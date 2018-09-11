############################################################################
#	  COPYRIGHT: 2017 lingtrack Instruments, Inc. All rights reserved
#	PROJECTNAME: Mx
#		VERSION:
#	   FILENAME: CMakeLists.txt
#		CREATED: 2017.12.12
#		 AUTHOR: hedengfeng
#	DESCRIPTION:
############################################################################
cmake_minimum_required (VERSION 2.6)

MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir "${PROJECT_SOURCE_DIR})

PROJECT(my_test_c++)

# 目标程序的运行环境
set(CMAKE_SYSTEM_NAME Linux)

SET(CMAKE_C_COMPILER "/usr/bin/x86_64-linux-gnu-gcc")
SET(CMAKE_CXX_COMPILER "/usr/bin/x86_64-linux-gnu-g++")
set(STRIP_TOOL /usr/bin/x86_64-linux-gnu-strip)

# 交叉编译时搜索头文件和库的路径
set(CMAKE_FIND_ROOT_PATH /usr)

# 仅在目标机环境下搜索头文件和库
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_SKIP_RPATH TRUE)

# 查看链接时使用的命令, 以便于检查链接了哪些库, 用了哪些优化选项
set(CMAKE_VERBOSE_MAKEFILE TRUE)

INCLUDE_DIRECTORIES(/usr/local/include/google/protobuf)

include_directories(${PROJECT_SOURCE_DIR})

link_directories(${PROJECT_BINARY_DIR}/lib)

link_directories("/usr/local/lib")


#SET(CMAKE_CXX_FLAGS "-g -Wall")
#SET(CMAKE_C_FLAGS "-g -Wall")

ADD_DEFINITIONS(
-std=c++11 -Wno-deprecated-declarations -fpermissive -g -ggdb -Wno-psabi -rdynamic -ldl
)

#ADD_DEFINITIONS(
#-std=c++11 -Wno-deprecated-declarations -fpermissive -O2
#)


#SET(SRCS
#	zermq_rep_test.cpp
#)

ADD_EXECUTABLE(reader  LT.Interaction.pb.cc Reader.cpp)

target_link_libraries(reader  protobuf)

ADD_EXECUTABLE(writer  LT.Interaction.pb.cc Writer.cpp)

target_link_libraries(writer  protobuf)





