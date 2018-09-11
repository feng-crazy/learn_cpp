############################################################################
#	  COPYRIGHT: 2015 Edan Instruments, Inc. All rights reserved
#	PROJECTNAME: Mx
#		VERSION:
#	   FILENAME: CMakeLists.txt
#		CREATED: 2015.09.25
#		 AUTHOR: 何登锋
#	DESCRIPTION:
############################################################################

# 源文件列表
set(SRCS KeyboardFbdump.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(CMAKE_SKIP_RPATH TRUE)

add_executable(KeyboardFbdump ${SRCS})



if(CMAKE_CROSSCOMPILING)
	# 去除可执行文件中的符号信息, 减小体积
	add_custom_command(TARGET KeyboardFbdump POST_BUILD
		COMMAND ${STRIP_TOOL} ./KeyboardFbdump
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		COMMENT "${STRIP_TOOL} ./KeyboardFbdump")

else(CMAKE_CROSSCOMPILING)
	# 去除可执行文件中的符号信息, 减小体积
	add_custom_command(TARGET KeyboardFbdump POST_BUILD
		COMMAND strip ./KeyboardFbdump
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		COMMENT "strip ./KeyboardFbdump")
endif(CMAKE_CROSSCOMPILING)


# 查看链接时使用的命令, 以便于检查链接了哪些库, 用了哪些优化选项
#set(CMAKE_VERBOSE_MAKEFILE TRUE)

