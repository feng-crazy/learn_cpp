/******************************************************************************
	  COPYRIGHT: 2017 LingTrack Instruments,	Inc. All rights	reserved
	PROJECTNAME: LT
		VERSION: 1.0
	   FILENAME: DebugCtrl.cpp
		CREATED: 2017.06.10
		 AUTHOR: 何登锋
	DESCRIPTION: 打印当前线程的函数栈
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <time.h>
#include "Utility/Misc.h"
#include "Utility/DebugCtrl.h"

// 记录错误输出的日志文件路径
// 路径"/home/edan/etc/error.log"是可读写的，发布测试版时会使用这个路径
// 路径"/var/error.log"是内存中的路径，关机后就会删除，发布正式版时需要关闭调试功能，
// 则指定这个目录
static std::string err_file = "/home/edan/etc/error.log";
static std::string dbg_file = "/home/edan/etc/dbg.log";

static int LOG_FILE_MAX_SIZE = 500; // 单位KB，经验值

/******************************************************************************
AUTHOR: 何登锋, 2017.07.15
DESCRIPTION:
	将错误信息分别输出在终端和日志文件中。
PARAMETERS:
	log: 待输出的错误信息
	log_file: 日志文件的地址
RETURN:
	无
******************************************************************************/
void log_err_to_file(const char *log, FILE *log_file)
{
	// 输出在标准输出
	if(NULL == log)
	{
		return;
	}
	printf("%s", log);

	// 输出在日志文件
	if(NULL == log_file)
	{
		return;
	}
	fputs(log, log_file);
}

void log_dbg_info(const char *log)
{
	if(log == NULL)
	{
		return;
	}

	FILE *dbg_log_file;  // 错误日志文件
	if(::file_size(dbg_file)/1024 < LOG_FILE_MAX_SIZE)
	{
		// 在文件尾部追加
		dbg_log_file = fopen(dbg_file.c_str(), "a+");
	}
	else
	{
		// 清空文件从头记录
		dbg_log_file = fopen(dbg_file.c_str(), "w+");
	}

	log_err_to_file(log, dbg_log_file);
	fclose(dbg_log_file);
}

/******************************************************************************
AUTHOR: 何登锋, 2017.06.10
DESCRIPTION:
	打印当前线程的函数栈，注意使用该功能时，编译参数中不能打开-O选项，且加上-rdynamic -ldl
PARAMETERS:
	无
RETURN:
	无
TODO:
	已知使用arm-linux-gcc-3.4.1+glibc-2.3.2编译出的程序运行时打印不出虚函数
******************************************************************************/
void stack_trace(const char *ptr)
{
	const int max_depth = 32;
	void *trace[max_depth];
	int stack_depth = backtrace(trace, max_depth);
	FILE *err_log_file;  // 错误日志文件
	char log_str[100];   // 每条错误信息不能超过这个长度

	flockfile(stdout);

	////////////////////////////////////////////////////////
	// 打开日志文件
	//
	// 当文件体积大于LOG_FILE_MAX_SIZE时，文件清空重新开始记录
	// 避免文件体积过大影响打开和写入的速度
	////////////////////////////////////////////////////////
	if(::file_size(err_file)/1024 < LOG_FILE_MAX_SIZE)
	{
		// 在文件尾部追加
		err_log_file = fopen(err_file.c_str(), "a+");
	}
	else
	{
		// 清空文件从头记录
		err_log_file = fopen(err_file.c_str(), "w+");
	}

	////////////////////////////////////////////////////////
	// 开始搜集错误信息
	////////////////////////////////////////////////////////
	time_t t = time(NULL);
	strftime(log_str, sizeof(log_str), "\n\n%Y-%m-%d %H:%M:%S\n", localtime(&t));
	log_err_to_file(log_str, err_log_file);

	for(int i=0; i<stack_depth; i++)
	{
		Dl_info dlinfo;
		if(!dladdr(trace[i], &dlinfo))
		{
			snprintf(log_str, sizeof(log_str), "@ %p\n", trace[i]);
			log_err_to_file(log_str, err_log_file);
			continue;
		}

		const char *symname = dlinfo.dli_sname;

		int status;
		char *demangled = __cxxabiv1::__cxa_demangle(symname, NULL, NULL, &status);
		if((status == 0) && demangled)
		{
			symname = demangled;
		}

		if(symname)
		{
			snprintf(log_str, sizeof(log_str), "@ %s\n", symname);
			log_err_to_file(log_str, err_log_file);
		}
		else
		{
			snprintf(log_str, sizeof(log_str), "@ %p\n", trace[i]);
			log_err_to_file(log_str, err_log_file);
		}

		if(demangled)
		{
			free(demangled);
		}
	}

	log_err_to_file(ptr, err_log_file);
	fclose(err_log_file);

	funlockfile(stdout);
}

